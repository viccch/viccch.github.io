#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <iostream>
#include <fstream>
#include <string>
// #include <bits/stdc++.h>
#include <vector>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <nlohmann/json.hpp>

struct point // 点
{
    int id;            // 点号
    std::string name;  // 点名
    double value;      // 高程
    int is_known = 0L; // 是否是已知点
};

struct observation // 观测边值
{
    int from_id; // 起始点号
    int to_id;   // 终止点号
    std::string from_name;
    std::string to_name;
    double value;  // 观测值高差
    double length; // 距离观测值
};

struct pc_point : point
{
    double value_pc; // 平差后高程
    double v;        // 改正数
    double m;        // 中误差
};

struct pc_observation : observation
{
    double value_pc; // 高差平差值
    double v;        // 改正数
    double p;        // 观测权
    double m;        // 中误差
};

struct pc_point *get_point(int id, std::vector<struct pc_point> &m_point, int m_point_num_total)
{
    struct pc_point *ptr = NULL;
    for (int i = 0; i < m_point_num_total; i++)
    {
        if (m_point[i].id == id)
        {
            ptr = &m_point[i];
            break;
        }
        else
        {
            continue;
        }
    }
    if (ptr == NULL)
    {
        printf("查找id为%d的点时出错！\n", id);
    }
    return ptr;
}

void j_read(nlohmann::json &j, nlohmann::json &out)
{
    //
    int num_known = j["已知点数"].get<int>();      // 已知点数目
    int num_unknown = j["未知点数"].get<int>();    // 未知点个数
    int num_observations = j["观测数"].get<int>(); // 观测值数目
    int num_total = num_known + num_unknown;       // 总点位数目

    //
    std::vector<struct pc_observation> obsv;
    std::vector<struct pc_point> pt;

    for (auto &i : j["点库"])
    {
        pc_point t_pt;
        t_pt.id = i["id"].get<int>();
        t_pt.name = i["name"].get<std::string>();
        pt.push_back(t_pt);
    }

    for (auto &i : j["已知点数据"])
    {
        int t_id = i["id"].get<int>();
        auto ptr = get_point(t_id, pt, num_total);
        ptr->value = i["高程"].get<double>();
        ptr->is_known = true;
    }

    for (auto &i : j["观测数据"])
    {
        pc_observation t_obsv;
        t_obsv.from_id = i["from"].get<int>();
        t_obsv.to_id = i["to"].get<int>();
        t_obsv.from_name = i["from_name"].get<std::string>();
        t_obsv.to_name = i["to_name"].get<std::string>();
        t_obsv.value = i["观测高程差"].get<double>();
        t_obsv.length = i["距离"].get<double>();
        obsv.push_back(t_obsv);
    }

    //
    Eigen::MatrixXd L;   // 观测值
    Eigen::MatrixXd D;   // 已知点值
    Eigen::MatrixXd B;   // 系数
    Eigen::MatrixXd X_0; // 设未知点的近似值
    Eigen::MatrixXd P;   // 权值阵
    Eigen::MatrixXd l;   //

    // L观测值
    L.resize(num_observations, 1);
    L.setZero();

    for (int i = 0; i < L.rows(); i++)
    {
        L(i, 0) = obsv[i].value;
    }

    for (int i = 0; i < num_total; i++)
    {
        if (pt[i].is_known)
            continue;
        for (int j = 0; j < num_observations; j++)
        {
            auto &from_id = obsv[j].from_id;
            auto &to_id = obsv[j].to_id;
            auto &id = pt[i].id;
            if (from_id == id || to_id == id)
            {
                /**为x选取近似值*/
                if (from_id == id)
                    pt[i].value = -obsv[j].value + get_point(to_id, pt, num_total)->value;
                else
                    pt[i].value = obsv[j].value + get_point(from_id, pt, num_total)->value;
                break;
            }
        }
    }

    // X_0观测值近似值
    X_0.resize(num_unknown, 1);
    X_0.setZero();
    for (int i = 0; i < num_unknown; i++)
    {
        X_0(i, 0) = pt[i + num_known].value;
    }
    // 设置常数项D和系数阵

    B.resize(num_observations, num_unknown);
    B.setZero();
    D.resize(num_observations, 1);
    D.setZero();
    for (int i = 0; i < num_observations; i++)
    {
        /***
         * 遍历所有观测边，根据观测边的起始点号和终止点号，匹配对应的观测点
         */
        auto ptr_from = get_point(obsv[i].from_id, pt, num_total);
        auto ptr_to = get_point(obsv[i].to_id, pt, num_total);

        if (ptr_from->is_known && (ptr_from->id <= num_known))
        {
            D(i, 0) = -ptr_from->value;
        }
        if (ptr_to->is_known && (ptr_to->id <= num_known))
        {
            D(i, 0) = ptr_to->value;
        }

        auto num_t1 = ptr_from - &pt[0] - num_known;
        auto num_t2 = ptr_to - &pt[0] - num_known;
        if (num_t1 >= 0)
            B(i, num_t1) = -1;
        if (num_t2 >= 0)
            B(i, num_t2) = 1;
    }

    P.resize(num_observations, num_observations); // 权矩阵是方阵
    P.setZero();
    for (int i = 0; i < num_observations; i++)
    {
        P(i, i) = obsv[i].length;
    }

    /**
     * 对于4*4及一下大小的矩阵，使用Q.inverse();
     * 对于大矩阵，使用 Eigen::PartialPivLU<Eigen::MatrixXd> lu(P);
     * auto x = lu.inverse();
     */

    Eigen::PartialPivLU<Eigen::MatrixXd> lu(P); // 使用LU头文件
    P = lu.inverse();

    l.resize(num_observations, 1);
    l = L - D - B * X_0;

    auto N = B.transpose() * P * B;
    auto W = B.transpose() * P * l;

    Eigen::PartialPivLU<Eigen::MatrixXd> N_lu(N);
    auto x_hat = N_lu.inverse() * W;
    auto X_hat = X_0 + x_hat;
    auto V = B * x_hat - l;
    auto L_hat = L + V;

    auto _M = N_lu.inverse();
    Eigen::MatrixXd D__;
    D__.resize(_M.rows(), 1);
    for (int i = 0; i < _M.rows(); i++)
    {
        D__(i, 0) = sqrt(_M(i, i));
    }
    auto sigma_2 = (V.transpose() * P * V / (num_observations - num_unknown));
    double sigma = sigma_2(0, 0);
    auto m = D__ * sigma; // 高程平差中误差

    // std::cout << "V=\n"
    //           << V << std::endl;
    // std::cout << "L_h=\n"
    //           << L_hat << std::endl;

    out["高程平差"] = j["点库"];
    out["观测值平差"] = j["观测数据"];

    for (int i = 0; i < num_total; i++)
    {

        if (pt[i].is_known)
        {
            out["高程平差"][i]["高程"] = pt[i].value;
            out["高程平差"][i]["平差后高程"] = pt[i].value;
            out["高程平差"][i]["改正数"] = 0.0;
            out["高程平差"][i]["中误差"] = 0.0;
        }
        else
        {
            out["高程平差"][i]["高程"] = pt[i].value;
            out["高程平差"][i]["平差后高程"] = pt[i].value + x_hat(i - num_known, 0);
            out["高程平差"][i]["改正数"] = x_hat(i - num_known, 0);
            out["高程平差"][i]["中误差"] = m(i - num_known, 0);
        }
    }
    for (int i = 0; i < num_observations; i++)
    {
        out["观测值平差"][i]["观测权"] = P(i, i);
        out["观测值平差"][i]["改正数"] = V(i, 0);
        out["观测值平差"][i]["高差平差值"] = L_hat(i, 0);
        // out["观测值平差"][i]["中误差"] = m(i, '0);
    }
}

/**
 * Qt界面部分
 */

// #include "qlabel.h"
// #include <QWidget>

#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextEdit>
#include <QObject>
// #include <QTextCodec> //中文路径问题！
QWidget *w;
QPushButton *btn_1;
QPushButton *btn_2;
QTextEdit *text_1;
QTextEdit *text_2;
QHBoxLayout layout_h1;
QHBoxLayout layout_h2;
QVBoxLayout layout_v;

nlohmann::json jf;    // 输入的json
nlohmann::json out_j; // 输出的json

void btn_1_clicked()
{
    QString path = QFileDialog::getOpenFileName(w, "", ".", "");
    if(path==nullptr)return;

    // std::ifstream j_file(path.toStdString());//中文路径问题！
    // QTextCodec *code = QTextCodec::codecForName("GB2312");
    // std::string f_name = code->fromUnicode(path).data();
    std::string f_name = path.toLocal8Bit().data();
    std::ifstream j_file(f_name);
    j_file>> jf;                                // 读文件json
    text_1->setText(QString(jf.dump(2).data())); // 将读取的json打印到文本框1
}
void btn_2_clicked()
{
    j_read(jf, out_j); // 读取和计算
    std::ofstream o_file("out.json");
    o_file << out_j.dump(2);                        // 输出计算结果到本地文件out.json
    text_2->setText(QString(out_j.dump(2).data())); // 将计算结果json打印到文本框1
}

void w_init()
{
    /*控件初始化*/
    w->setMinimumSize(800, 600);
    w->setWindowTitle("测量平差：水准网间接平差");
    btn_1->setMinimumHeight(30);
    btn_2->setMinimumHeight(30);
    layout_h1.addWidget(btn_1);
    layout_h1.addWidget(btn_2);
    layout_h2.addWidget(text_1);
    layout_h2.addWidget(text_2);
    layout_v.addItem(&layout_h1);
    layout_v.addItem(&layout_h2);

    w->setLayout(&layout_v);
    w->show();
    /*绑定事件*/
    btn_1->connect(btn_1, &QPushButton::clicked, &btn_1_clicked);
    btn_2->connect(btn_2, &QPushButton::clicked, &btn_2_clicked);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new QWidget();
    btn_1 = new QPushButton("加载数据");
    btn_2 = new QPushButton("平差计算");
    text_1 = new QTextEdit("输入窗口:点击“加载数据”按钮，选择文件data1.json或data2.json，然后点击“平差计算”按钮\t不要编辑");
    text_2 = new QTextEdit("输出窗口：");

    w_init();
    return a.exec();
}

/**
 * 程序说明：
 * @author:张世杰 CUMT
 * @file:文件包括 CMakeLists.txt、main.cpp、data1.json、data2.json，共四个文件
 * @include:使用的开源项目：
 * 矩阵库：eigen3(https://gitlab.com/libeigen/eigen)；
 * json库：nlohmann-json(https://github.com/nlohmann/json)；
 * 可视化：Qt(https://www.qt.io/)
 */