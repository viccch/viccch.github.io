#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <stack>
#include <vector>
#include <algorithm>
#include <memory>
#include <bits/stdc++.h>
// #include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image_write.h>
// #include <eigen3/Eigen/Core>
// #include <eigen3/Eigen/Dense>

/*前视声明*/
struct Vec2;
struct Line;
struct Triangle;
struct Polygon;
struct Tin;
struct Shader;
struct UI;
struct Related;
/*点类*/
using Point = struct Vec2;

/*存放点集数据*/
// using Data = std::vector<Point>;

/*平面向量类*/
struct Vec2
{
public:
    double x;
    double y;
    double z;
    std::string id;

    Vec2() : Vec2(0, 0, 0, "") {}
    Vec2(double _x, double _y) : Vec2(_x, _y, 0, "") {}
    Vec2(double _x, double _y, double _z) : Vec2(_x, _y, _z, "") {}
    Vec2(double _x, double _y, double _z, const std::string &str_id) : x(_x), y(_y), z(_z), id(str_id) {}
    // Vec2(const Vec2 &a, const Vec2 &b) : x(b.x - a.x), y(b.y - a.y) {}
    Vec2(const Vec2 &a) : Vec2(a.x, a.y, a.z, a.id) {}
    Vec2 &operator=(const Vec2 &a)
    {
        x = a.x, y = a.y, z = a.z, id = a.id;
        return *this;
    }
    Vec2 operator+(const Vec2 &b) const
    {
        return {x + b.x, y + b.y};
    }
    Vec2 operator-(const Vec2 &b) const
    {
        return {x - b.x, y - b.y};
    }
    Vec2 operator*(double scale) const
    {
        return {x * scale, y * scale};
    }
    Vec2 operator/(double div) const
    {
        return {x / div, y / div};
    }
    Vec2 &operator+=(const Vec2 &b)
    {
        x += b.x;
        y += b.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &b)
    {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    Vec2 &operator*=(double scale)
    {
        x *= scale;
        y *= scale;
        return *this;
    }
    Vec2 &operator/=(double div)
    {
        x /= div;
        y /= div;
        return *this;
    }
    bool operator==(const Vec2 &b) const
    {
        return (x == b.x && y == b.y);
    }
    bool operator<(const Vec2 &p) const
    {
        return (x != p.x) ? (x > p.x) : (y > p.y);
    };
    /*平面向量叉积*/
    static double cross(const Vec2 &a, const Vec2 &b) { return a.x * b.y - b.x * a.y; }
    /*点积*/
    static double dot(const Vec2 &a, const Vec2 &b) { return a.x * b.x - a.y * b.y; }
    /*返回模长*/
    double get_norm() const
    {
        return std::sqrt(x * x + y * y);
    }
    friend std::ostream &operator<<(std::ostream &out, const Vec2 &p)
    {
        out << std::fixed << std::setprecision(3) << "(id,x,y,z)=(" << p.id << "," << p.x << "," << p.y << "," << p.z << ")";
        return out;
    }
};

/*线段类*/
struct Line
{
public:
    Point a; /*from*/
    Point b; /*to*/

    Line(const Line &l) : a(l.a), b(l.b) {}
    Line(const Point &_a, const Point &_b) : a(_a), b(_b) {}
    Line &operator=(const Line &l)
    {
        a = l.a, b = l.b;
        return *this;
    }
    Vec2 vec() const { return Vec2(b - a); }

    /*判断点p是否在线段内*/
    bool if_point_in(const Point &p) const
    {
        double xmax = std::max(a.x, b.x);
        double xmin = std::min(a.x, b.x);
        double ymax = std::max(a.y, b.y);
        double ymin = std::min(a.y, b.y);

        Line ap{a, p};
        // Line &ab = *this;

        if (Vec2::cross((*this).vec(), ap.vec()) != 0)
            return false;
        if ((p.x >= xmin && p.x <= xmax) && (p.y >= ymin && p.y <= ymax))
            return true;
        else
            return false;
    }
};

/*三角形类*/
struct Triangle
{
public:
    Triangle() = delete;
    /*保证构造的三角形都是凸包！！！*/
    Triangle(const Point &a, const Point &b, const Point &c)
    {
        auto ab = b - a;
        auto bc = c - b;
        if (Vec2::cross(ab, bc) < 0)
        {
            pts[0] = a;
            pts[1] = c;
            pts[2] = b;
        }
        else
        {
            pts[0] = a;
            pts[1] = b;
            pts[2] = c;
        }
    }
    Point pts[3]; /*顶点*/

    Triangle(const Triangle &t) { pts[0] = t.pts[0], pts[1] = t.pts[1], pts[2] = t.pts[2]; }
    Triangle &operator=(const Triangle &t)
    {
        pts[0] = t.pts[0], pts[1] = t.pts[1], pts[2] = t.pts[2];
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &out, const Triangle &tri)
    {
        out << "point 1:" << tri.pts[0] << "\n";
        out << "point 2:" << tri.pts[1] << "\n";
        out << "point 3:" << tri.pts[2] << "\n";
        return out;
    }
    bool operator==(const Triangle &t) const
    {
        std::vector<Point> vec1{pts[0], pts[1], pts[2]};
        std::vector<Point> vec2{t.pts[0], t.pts[1], t.pts[2]};
        auto func_cmp = [](const Point &a, const Point &b)
        {
            return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
        };
        std::sort(vec1.begin(), vec1.end(), func_cmp);
        std::sort(vec2.begin(), vec2.end(), func_cmp);
        return (vec1 == vec2);
    }
    bool operator<(const Triangle &t) const
    {
        std::vector<Point> vec1{pts[0], pts[1], pts[2]};
        std::vector<Point> vec2{t.pts[0], t.pts[1], t.pts[2]};
        auto func_cmp = [](const Point &a, const Point &b)
        {
            return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
        };
        std::sort(vec1.begin(), vec1.end(), func_cmp);
        std::sort(vec2.begin(), vec2.end(), func_cmp);
        return (vec1 < vec2);
    }

    /*最大最小角*/
    double get_max_min_angle() const
    {
        return 0;
    }

    /*判断点p与三角形tri的位置关系
        返回值：
        case 0:外部
        case 1 :内部
        case -1:边12上
        case -2:边23上
        case -3:边13上
    */
    static int if_point_in(const Point &p, const Triangle &tri)
    {
        int sig = 0;

        Line AP = Line{tri.pts[0], p};
        Line BP = Line{tri.pts[1], p};
        Line CP = Line{tri.pts[2], p};
        Line AB = Line{tri.pts[0], tri.pts[1]};
        Line BC = Line{tri.pts[1], tri.pts[2]};
        Line CA = Line{tri.pts[2], tri.pts[0]};

        double crs_1 = Vec2::cross(AB.vec(), AP.vec());
        double crs_2 = Vec2::cross(BC.vec(), BP.vec());
        double crs_3 = Vec2::cross(CA.vec(), CP.vec());

        if ((crs_1 > 0 && crs_2 > 0 && crs_3 > 0) || (crs_1 < 0 && crs_2 < 0 && crs_3 < 0))
        {
            sig = 1; /*点在三角形内部*/
        }
        else if (crs_1 * crs_2 * crs_3 == 0)
        { /*点不重复，所以应当不能同时出现两个叉积为0*/

            if (crs_1 == 0 && AB.if_point_in(p))
                sig = -1; /*点在AB上*/
            else if (crs_2 == 0 && BC.if_point_in(p))
                sig = -2; /*点在BC上*/
            else if (crs_3 == 0 && CA.if_point_in(p))
                sig = -3; /*点在CA上*/
        }
        else
        {
            sig = 0; /*点在三角形外，不在边上*/
        }
        return sig;
    }

    /*
    判断点p是否是三角形三顶点之一：
    如果是，返回1,2,3
    不是则返回0
    */
    int if_have_vertex(const Point &p) const
    {
        if (p == pts[0])
            return 1;
        else if (p == pts[1])
            return 2;
        else if (p == pts[2])
            return 3;
        else
            return 0;
    }

    /*返回三角形重心*/
    Point get_gravity() const { return (pts[0] + pts[1] + pts[2]) / 3.0f; } /* 计算三角形重心*/

    /*返回三角形外接圆圆心*/
    Point get_centre() const /*计算三角形外接圆圆心*/
    {
        auto p1 = pts[0], p2 = pts[1], p3 = pts[2];
        /*矩阵计算理解*/
        {
            // Eigen::Matrix3f A, B, C;
            // A << 1, p1.x * p1.x + p1.y * p1.y, p1.y,
            //     1, p2.x * p2.x + p2.y * p2.y, p2.y,
            //     1, p3.x * p3.x + p3.y * p3.y, p3.y;
            // B << 1, p1.x, p1.x * p1.x + p1.y * p1.y,
            //     1, p2.x, p2.x * p2.x + p2.y * p2.y,
            //     1, p3.x, p3.x * p3.x + p3.y * p3.y;
            // C << 1, p1.x, p1.y,
            //     1, p2.x, p2.y,
            //     1, p3.x, p3.y;
            // auto a = A.determinant();
            // auto b = B.determinant();
            // auto c = C.determinant();
            // return {0.5f * a / c, 0.5f * b / c};
        }

        /*快50%,精度溢出风险*/
        {
            // auto A1 = 2 * (p2.x - p1.x);
            // auto B1 = 2 * (p2.y - p1.y);
            // auto C1 = p2.x * p2.x + p2.y * p2.y - p1.x * p1.x - p1.y * p1.y;
            // auto A2 = 2 * (p3.x - p2.x);
            // auto B2 = 2 * (p3.y - p2.y);
            // auto C2 = p3.x * p3.x + p3.y * p3.y - p2.x * p2.x - p2.y * p2.y;
            // auto a = B2 * C1 - B1 * C2;
            // auto b = -A2 * C1 + A1 * C2;
            // auto c = B2 * A1 - B1 * A2;
            // return {a / c, b / c};
        }

        /*精度溢出风险*/
        {
            double a = (p3.y - p2.y) * (p2.x * p2.x + p2.y * p2.y - p1.x * p1.x - p1.y * p1.y) - (p2.y - p1.y) * (p3.x * p3.x + p3.y * p3.y - p2.x * p2.x - p2.y * p2.y);
            double b = -(p3.x - p2.x) * (p2.x * p2.x + p2.y * p2.y - p1.x * p1.x - p1.y * p1.y) + (p2.x - p1.x) * (p3.x * p3.x + p3.y * p3.y - p2.x * p2.x - p2.y * p2.y);
            double c = (p3.y - p2.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p3.x - p2.x);
            return {0.5f * a / c, 0.5f * b / c};
        }
    }
};

/*多边形类*/
struct Polygon
{
public:
    std::vector<Point> pts; /*顶点*/

    Polygon() {}
    Polygon(const std::vector<Point> &data) : pts(data) {}
    Polygon(const std::initializer_list<Point> &list)
    {
        for (auto &item : list)
        {
            pts.push_back(item);
        }
    }

    /*计算获得凸包多边形*/
    /*将略过凸包上180度的点*/
    Polygon get_convex() const
    {
        Polygon ret;                 /*返回值*/
        Polygon this_copy = (*this); /*this对象的拷贝*/

        /*第一次排序，找到右下角元素*/ /*TODO:O1找到？*/
        std::sort(this_copy.pts.begin(), this_copy.pts.end(), [](const Point &a, const Point &b)
                  { return (a.y != b.y) ? (a.y < b.y) : (a.x < b.x); });

        /*第二次排序：按和第一个点（右下）与x轴夹角的角度排序(逆时针)*/
        std::sort(this_copy.pts.begin() + 1, this_copy.pts.end(), [&p = this_copy.pts[0]](const Point &a, const Point &b) -> bool
                  {
                    //   Vec2 a0 = a - this_copy.pts[0];
                    //   Vec2 b0 = b - this_copy.pts[0];
                      Vec2 a0 = a - p;
                      Vec2 b0 = b - p;
                      auto d_a = a0.get_norm();
                      auto d_b = b0.get_norm();
                      auto alpha_a = std::atan2(a0.y , a0.x);
                      auto alpha_b = std::atan2(b0.y , b0.x);
                      return (alpha_a!=alpha_b)?(alpha_a<alpha_b):(d_a<d_b); });

        ret.pts.push_back(this_copy.pts[0]);
        ret.pts.push_back(this_copy.pts[1]);

        for (auto it = this_copy.pts.begin() + 2; it < this_copy.pts.end(); it++)
        {
            Vec2 c0 = *it - ret.pts.back();
            Vec2 p0 = ret.pts.at(ret.pts.size() - 1) - ret.pts.at(ret.pts.size() - 2);

            double crs = Vec2::cross(p0, c0);
            if (crs > 0.0f) /*cur在直线左边**/
            {
                ret.pts.push_back(*it);
            }
            else if (crs < 0.0f)
            {
                ret.pts.pop_back();
                it--;
            }
            else /* crs == 0 */
            {
                if (ret.pts.size() <= 2)
                {
                    ret.pts.pop_back();
                    ret.pts.push_back(*it);
                    continue;
                }
                else
                {
                    ret.pts.pop_back();
                    it--;
                }
            }
        }
        return ret;
    }

    /*判断凸包，（？必须要求逆时针吗？）*/
    bool is_convex() const
    {
        int flag_plus = 0;
        int flag_minus = 0;
        int flag_0 = 0;
        for (std::size_t i = 0; i < pts.size(); i++)
        {
            Vec2 V1{pts[(i + 1) % pts.size()] - pts[(i) % pts.size()]};
            Vec2 V2{pts[(i + 2) % pts.size()] - pts[(i + 1) % pts.size()]};
            //     if (Vec2::cross(V1, V2) < 0)
            //         return false;
            // }
            // return true;

            double crs = Vec2::cross(V1, V2);
            if (crs < 0)
                flag_minus++;
            else if (crs > 0)
                flag_plus++;
            else if (crs == 0)
                flag_0++;
            else
                continue;
        }
        if (flag_0 != 0)
            return false;
        if (flag_minus * flag_plus == 0)
            return true;
        else
            return false;
    }

    /*打印*/
    void print() const
    {
        for (auto &item : pts)
        {
            std::cout << item << std::endl;
        }
    }
};

struct Related
{
    std::map<Point, std::set<Triangle>> relations;

    void add_relation(const Vec2 &p, const Triangle &t)
    {
        relations[p].insert(t);
    }
    void remove_relation(const Point &p, const Triangle &t)
    {
        auto it_p = relations.find(p);
        if (it_p != relations.end())
        {
            auto it_t = relations[p].find(t);
            if (it_t != relations[p].end())
                relations[p].erase(it_t);
        }
    }
    void show() const
    {
        for (auto &item : relations)
        {
            for (auto &r : item.second)
            {
                std::cout << "key=\n"
                          << item.first << "\nvalue=\n"
                          << r << "\n";
            }
        }
    }
};
/*三角网类*/
struct Tin
{

public:
    Tin() {}

    /*四个边界点，顺序：    0：右上，    1：左上，    2：左下，    3：右下    */
    std::vector<Point> bound_pts;

private:
    /*计算包围矩形，*/
    std::pair<Point, Point> get_bound_pts(const std::vector<Point> &data)
    {
        auto func_x = [](const Point &a, const Point &b) -> bool
        { return a.x < b.x; };
        auto func_y = [](const Point &a, const Point &b) -> bool
        { return a.y < b.y; };

        auto xmin = (*std::min_element(data.begin(), data.end(), func_x)).x;
        auto xmax = (*std::max_element(data.begin(), data.end(), func_x)).x;
        auto ymin = (*std::min_element(data.begin(), data.end(), func_y)).y;
        auto ymax = (*std::max_element(data.begin(), data.end(), func_y)).y;

        Point p0{xmax, ymax}; /*右上*/
        // Point p1{xmin, ymax}; /*左上*/
        Point p2{xmin, ymin}; /*左下*/
        // Point p3{xmax, ymin}; /*右下*/

        return {p0, p2};
    }
    /*计算包围矩形，*/
    void set_bound_pts(double xmin, double xmax, double ymin, double ymax)
    {
        Point p0{xmax, ymax}; /*右上*/
        Point p1{xmin, ymax}; /*左上*/
        Point p2{xmin, ymin}; /*左下*/
        Point p3{xmax, ymin}; /*右下*/

        bound_pts.push_back(p0);
        bound_pts.push_back(p1);
        bound_pts.push_back(p2);
        bound_pts.push_back(p3);
    }

    /*检查边合法性，递归*/
    void legalize_edge(const Line &L)
    {
        auto tri_1_it = triangles.end();
        auto tri_1_1 = 0, tri_1_2 = 0; /*第一个三角形的*/
        for (auto it = triangles.begin(); it != triangles.end(); it++)
        {
            int this_1 = (*it).if_have_vertex(L.a);
            int this_2 = (*it).if_have_vertex(L.b);
            if (!(this_1 && this_2))
                continue;
            else
            {
                if (!(tri_1_1 && tri_1_2))
                {
                    /*推迟到找到有公共边的另一个三角形*/
                    tri_1_1 = this_1;
                    tri_1_2 = this_2; /*第一个三角形的*/
                    tri_1_it = it;
                    continue;
                }
                else
                {
                    // std::cout << "找到有公共边的另一个三角形\n";
                    int idx_1 = 0 + 1 + 2 - (tri_1_1 - 1) - (tri_1_2 - 1);
                    int idx_2 = 0 + 1 + 2 - (this_1 - 1) - (this_2 - 1);

                    auto p1 = (*tri_1_it).pts[idx_1]; /*第一个三角形的*/
                    auto p2 = (*it).pts[idx_2];       /*第二个三角形的*/

                    /*
                    方案1：构造正向（点按逆时针顺序）多边形（四边形），判断凹凸
                    // Polygon pg{L.b, p1, L.a, p2};
                    // Polygon pg{(*it).pts[0], (*it).pts[1], (*it).pts[2], p1};

                    // std::cout << (pg.is_convex() ? "是" : "不是") << std::endl;
                    // pg.print();

                    // std::cout << "is_convex: " << pg.is_convex() << "\n";
                    // if (pg.is_convex()){}
                    */
                    /*
                     替代方案：
                     计算点p2是否在另一个三角形内,即可确定构成的四边形凹凸情况
                    */
                    if (Triangle::if_point_in(p1, (*it)) != 1)
                    {
                        auto c1 = (*tri_1_it).get_centre(); /*c1圆心*/
                        auto c2 = (*it).get_centre();       /*c2圆心*/

                        Vec2 p2_to_c1(p2 - c1);
                        Vec2 p1_to_c1(p1 - c1);

                        Vec2 p1_to_c2(p1 - c2);
                        Vec2 p2_to_c2(p2 - c2);

                        auto b1 = (p2_to_c1.get_norm() < p1_to_c1.get_norm());
                        auto b2 = (p1_to_c2.get_norm() < p2_to_c2.get_norm());

                        // if (!b1)
                        // {
                        //     // std::cout << "p2不在圆c1内\n";
                        //     // return;
                        // }
                        // if (!b2)
                        // {
                        //     // std::cout << "p1不在圆c2内\n";
                        // }
                        if (b1 || b2)
                        {
                            /*p2在圆c1内*/
                            // std::cout << p2 << "p2在圆c1内\n";

                            // auto item_1 = *it;
                            // auto item_2 = *tri_1_it;

                            triangles.erase(it);
                            triangles.erase(tri_1_it);

                            triangles.push_back(Triangle{p1, L.a, p2});
                            triangles.push_back(Triangle{p2, L.b, p1});

                            // legalize_edge( Line{item_1.pts[(idx_1 + 1) % 3], item_1.pts[(idx_1 + 2) % 3]});
                            // legalize_edge( Line{item_2.pts[(idx_2 + 1) % 3], item_2.pts[(idx_2 + 2) % 3]});
                            legalize_edge(Line{p1, L.a});
                            legalize_edge(Line{p1, L.b});
                            legalize_edge(Line{p2, L.a});
                            legalize_edge(Line{p2, L.b});
                        }
                    }
                    return;
                }
            }
        }
        return;
    }

    /*遍历每一个三角形，去除含有四个边界点的三角形*/
    void clear_bound()
    {
        if (bound_pts.size() == 0)
            return;
        std::list<decltype(triangles.begin())> ready_to_remove;

        for (auto &p : bound_pts)
        {
            for (auto it = triangles.begin(); it != triangles.end(); it++)
            {
                if ((*it).if_have_vertex(p))
                    ready_to_remove.emplace_back(it);
                else
                    continue;
            }
        }

        for (auto it = ready_to_remove.begin(); it != ready_to_remove.end(); it++)
        {
            auto rt = std::find(ready_to_remove.begin(), it, (*it));

            if (rt == it)
                continue;
            else
            {
                ready_to_remove.erase(rt);
            }
        }

        for (auto &it : ready_to_remove)
        {
            triangles.erase(it);
            // auto p0 = (*it).pts[0];
            // auto p1 = (*it).pts[1];
            // auto p2 = (*it).pts[2];
            // legalize_edge( Line{p0, p1});
            // legalize_edge( Line{p1, p2});
            // legalize_edge( Line{p2, p0});
        }
    }

    /*逐点增长，构建三角网*/
    void grow(const Point &p)
    {
        auto flag_it = triangles.end(); /*若找到有p的三角形，记录其迭代器*/
        int flag_sig = 0;               /*若算出p在三角形边上，记录其对应边号*/

        for (auto it = triangles.begin(); it != triangles.end(); it++)
        {
            /*记录点p：
            0：所有三角形外
            1：一个三角形内
            -1、-2、-3：三角形边上，记录： 是被找到三角形内，哪条边所对的顶点*/
            int sig = Triangle::if_point_in(p, *it);

            // std::cout << "sig = " << sig << std::endl;

            switch (sig)
            {
            case 0: /*点在三角形外*/
                continue;
                break;
            case 1: /*点在三角形内*/
            {
                auto item = *it;
                triangles.erase(it);

                triangles.push_back(Triangle{p, item.pts[0], item.pts[1]});
                triangles.push_back(Triangle{p, item.pts[1], item.pts[2]});
                triangles.push_back(Triangle{p, item.pts[2], item.pts[0]});

                legalize_edge(Line{item.pts[0], item.pts[1]});
                legalize_edge(Line{item.pts[1], item.pts[2]});
                legalize_edge(Line{item.pts[2], item.pts[0]});

                return;
            }
            break;
            default: /*点在边上*/ /*-1,-2,-3*/
            {
                if (0 == flag_sig)
                {
                    /*推迟到找到有公共边的另一个三角形再计算*/
                    flag_sig = sig; /*赋值为-1、-2、-3*/
                    flag_it = it;
                    continue;
                }
                else
                {
                    /*找到有公共边的另一个三角形*/
                    auto item = *it;
                    auto item_flag = *flag_it;
                    triangles.erase(flag_it);
                    triangles.erase(it);

                    triangles.push_back(Triangle{p, item.pts[(1 - sig) % 3], item.pts[(1 + 1 - sig) % 3]});
                    triangles.push_back(Triangle{p, item.pts[(1 - sig) % 3], item.pts[(2 + 1 - sig) % 3]});
                    triangles.push_back(Triangle{p, item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(1 + 1 - flag_sig) % 3]});
                    triangles.push_back(Triangle{p, item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(2 + 1 - flag_sig) % 3]});

                    legalize_edge(Line{item.pts[(1 - sig) % 3], item.pts[(1 + 1 - sig) % 3]});
                    legalize_edge(Line{item.pts[(1 - sig) % 3], item.pts[(2 + 1 - sig) % 3]});
                    legalize_edge(Line{item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(1 + 1 - flag_sig) % 3]});
                    legalize_edge(Line{item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(2 + 1 - flag_sig) % 3]});

                    return;
                }
            }
            break;
            }
            // return;
            /*找不到有公共边的另一个三角形 - > 点在super三角形上 - > 不可能发生，也不应当发生（足够大的包围三角形）*/
        }

        /*当前点所在边只属于一个三角形，即当前点在外凸包上，是凸包的平顶点（凸包180度内角）*/
        /*将此三角形分两份即可*/
        if (triangles.end() != flag_it)
        {
            // std::cout << p << "没找到匹配的，当前此点只属于一个三角形\n";

            auto item_flag = *flag_it;
            triangles.erase(flag_it);

            triangles.push_back(Triangle{p, item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(1 + 1 - flag_sig) % 3]});
            triangles.push_back(Triangle{p, item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(2 + 1 - flag_sig) % 3]});

            legalize_edge(Line{item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(1 + 1 - flag_sig) % 3]});
            legalize_edge(Line{item_flag.pts[(1 - flag_sig) % 3], item_flag.pts[(2 + 1 - flag_sig) % 3]});
        }
        return;
    }

public:
    /*打印*/
    void print() const
    {
        auto it = triangles.begin();
        std::cout << "当前有 " << triangles.size() << " 个三角形\n";
        for (std::size_t i = 0; i < triangles.size(); i++, it++)
        {
            std::cout << "第" << i + 1 << "个三角形：\n"
                      << (*it);
        }
        std::cout << "\n";
    }

    /*三角形链表*/
    std::list<Triangle> triangles;
    // [[deprecated]] Triangle super_triangle;

    /*生长算法*/
    [[TODO]] static std::unique_ptr<Tin> build_from_baseline(std::vector<Point> &data)
    {
        struct Baseline
        {
            Point a, b, p;
            bool operator==(const Baseline &bl) const
            {
                return (a == bl.a && b == bl.b) ||
                       (b == bl.a && a == bl.b);
            }
        };

        std::list<Point> un_used;
        std::for_each(data.begin(), data.end(), [&un_used](Point &p)
                      { un_used.push_front(p); });
        // std::vector<Point> used;
        std::list<Baseline> BaseStack;
        std::list<Baseline> LineList;
        std::list<Triangle> TriangleList;

        // Related R;

        // used.push_back(un_used.back());
        Point first = un_used.back();
        un_used.pop_back();

        auto m_it = un_used.begin();
        double m_distance = (first - (*m_it)).get_norm();

        for (auto it = un_used.begin(); it != un_used.end(); it++)
        {
            double c_dis = (first - (*it)).get_norm();
            if (c_dis < m_distance)
            {
                m_it = it;
                m_distance = c_dis;
            }
        }

        Baseline line{first, *m_it};

        BaseStack.push_back(line);

        auto get_angle = [](Point &a, Point &b, Point &p) -> double
        {
            auto PA = a - p;
            auto PB = b - p;
            return Vec2::dot(PA / PA.get_norm(), PB / PB.get_norm());
        };

        auto is_on_right = [](Point &a, Point &b, Point &p) -> bool
        {
            auto AP = p - a;
            auto AB = b - a;
            return (Vec2::cross(AP, AB) > 0);
        };

        while (!BaseStack.empty())
        {
            Baseline bl = BaseStack.back();
            BaseStack.pop_back();

            /*0到pi 区间内，角度增大，cos下降*/
            auto m_it = un_used.begin();
            double m_cos = 1.1f; /*0 度*/

            for (auto it = un_used.begin(); it != un_used.end(); it++)
            {
                if (is_on_right(bl.a, bl.b, *it))
                {
                    auto t_cos = get_angle(bl.a, bl.b, *it);
                    if (t_cos < m_cos)
                    {
                        m_it = it;
                        m_cos = t_cos;
                    }
                }
            }

            if (m_cos < 1.1f)
            {
                bl.p = *m_it;
                un_used.erase(m_it);

                auto bl1 = Baseline{bl.a, bl.p};
                auto bl2 = Baseline{bl.p, bl.b};

                // if (BaseStack.end() == std::find(BaseStack.begin(), BaseStack.end(), bl1))
                //     BaseStack.push_back(bl1);
                // if (BaseStack.end() == std::find(BaseStack.begin(), BaseStack.end(), bl2))
                //     BaseStack.push_back(bl2);

                if (LineList.end() == std::find(LineList.begin(), LineList.end(), bl1))
                    BaseStack.push_back(bl1);
                if (LineList.end() == std::find(LineList.begin(), LineList.end(), bl2))
                    BaseStack.push_back(bl2);

                if (LineList.end() == std::find(LineList.begin(), LineList.end(), bl))
                    LineList.push_back(bl);

                auto tri = Triangle{bl.a, bl.b, bl.p};
                if (TriangleList.end() == std::find(TriangleList.begin(), TriangleList.end(), tri))
                    TriangleList.push_back(tri);
            }
        }

        // std::cout << TriangleList.size() << std::endl;
        // for (auto it = TriangleList.begin(); it != TriangleList.end(); it++)
        // std::cout << *it << std::endl;

        auto tin_ptr = std::unique_ptr<Tin>(new Tin());
        auto &tin = *tin_ptr;
        // (*tin).triangles.swap(TriangleList);
        std::swap(tin.triangles, TriangleList);

        auto pr = tin.get_bound_pts(data);
        auto xmax = pr.first.x, ymax = pr.first.y, xmin = pr.second.x, ymin = pr.second.y;
        tin.set_bound_pts(xmin - 1, xmax + 1, ymin - 1, ymax + 1);
        return tin_ptr;
    }
    /*凸包算法2*/
    static std::unique_ptr<Tin> build_from_convex_2(std::vector<Point> &data)
    {
        /*平移*/
        Point f1 = data[0];
        {
            std::for_each(data.begin(), data.end(), [&f1](Point &p)
                          { p -= f1; });
        }

        auto cal_start = std::chrono::system_clock::now();

        /*计算凸包*/
        Polygon convex = Polygon(data).get_convex();

        auto cal_ok = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(cal_ok - cal_start).count();
        printf("计算凸包用时%dus\n", diff);

        Polygon convex_copy = convex; /*下面求差集会重排序*/

        /*计算差集*/
        Polygon difference;

        auto func_cmp = [](const Point &a, const Point &b)
        {
            return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
        };
        /*
        set_insertection 集合操作要求容器的元素有序，所以要先排序
        */
        std::sort(data.begin(), data.end(), func_cmp);
        std::sort(convex_copy.pts.begin(), convex_copy.pts.end(), func_cmp);

        std::set_difference(
            data.begin(), data.end(),
            convex_copy.pts.begin(), convex_copy.pts.end(),
            std::back_inserter(difference.pts), func_cmp);

        /*std::back_inserter?
        因为：set_difference 最后，执行的是复制操作而不是插入操作，必须要保证容器足够大，否则溢出，
        所以使用std::back_inserter,执行插入。
        */
        // printf("%d", difference.pts.size());

        auto tin_ptr = std::unique_ptr<Tin>(new Tin());
        auto &tin = *tin_ptr;

        // tin.set_bound_pts(data);
        auto pr = tin.get_bound_pts(data);
        auto xmax = pr.first.x, ymax = pr.first.y, xmin = pr.second.x, ymin = pr.second.y;
        tin.set_bound_pts(xmin - 1, xmax + 1, ymin - 1, ymax + 1);

        for (std::size_t i = 1; i < convex.pts.size() - 1; i++)
        { /*1 -> n-2*/
            auto &p0 = convex.pts[0];
            auto &p1 = convex.pts[i];
            auto &p2 = convex.pts[i + 1];

            tin.triangles.push_back(Triangle{p0, p1, p2});
            // legalize_edge( Line{p0, p1});
            // legalize_edge( Line{p0, p2});
            // legalize_edge( Line{p1, p2});
        }

        /*只有凸包边界，没有中心点，的情况*/
        if (data.size() == convex.pts.size())
        {
            for (auto it = tin.triangles.begin(); it != tin.triangles.end(); it++)
            {
                tin.legalize_edge(Line{(*it).pts[0], (*it).pts[1]});
                tin.legalize_edge(Line{(*it).pts[0], (*it).pts[2]});
                tin.legalize_edge(Line{(*it).pts[1], (*it).pts[2]});
            }
        }

        /*时间测试区*/
        {
            auto cal_start = std::chrono::system_clock::now();

            /*被测试代码*/
            {
                for (auto &item : difference.pts)
                    tin.grow(item);
            }

            auto cal_ok = std::chrono::system_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(cal_ok - cal_start).count();
            printf("grow迭代用时%dms\n", diff);
        }
        /*平移*/

        {
            std::for_each(data.begin(), data.end(), [&f1](Point &p)
                          { p += f1; });
            std::for_each(tin.triangles.begin(), tin.triangles.end(), [&f1](Triangle &tri)
                          {tri.pts[0] += f1;tri.pts[1]+=f1;tri.pts[2]+=f1; });
            std::for_each(tin.bound_pts.begin(), tin.bound_pts.end(), [&f1](Point &p)
                          { p += f1; });
        }

        return tin_ptr;
    }

    /*从凸包构造，seed*/
    /*考虑所有点刚好构成一个凸包，则没有seed...*/
    static std::unique_ptr<Tin> build_from_convex_with_seed(std::vector<Point> &data)
    {
        Point f1 = data[0];
        /*平移*/
        {
            std::for_each(data.begin(), data.end(), [&f1](Point &p)
                          { p -= f1; });
        }

        /*计算凸包*/
        Polygon convex = Polygon(data).get_convex();

        /*下面求差集会重排序*/
        Polygon convex_copy = convex;

        /*计算差集*/
        Polygon difference;

        auto func_cmp = [](const Point &a, const Point &b)
        {
            return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
        };

        /*
        set_insertection 集合操作要求容器的元素有序，所以要先排序
        */
        std::sort(data.begin(), data.end(), func_cmp);
        std::sort(convex_copy.pts.begin(), convex_copy.pts.end(), func_cmp);

        std::set_difference(
            data.begin(), data.end(),
            convex_copy.pts.begin(), convex_copy.pts.end(),
            std::back_inserter(difference.pts), func_cmp);

        /*std::back_inserter?
        因为：set_difference 最后，执行的是复制操作而不是插入操作，必须要保证容器足够大，否则溢出，
        所以使用std::back_inserter,执行插入。
        */

        // std::cout << "凸包大小：" << convex.pts.size() << "\n";
        // std::cout << "差集大小：" << difference.pts.size() << "\n";

        Point seed;
        /*如果凸包内没有点*/
        if (!difference.pts.empty())
        {
            /*随机取凸包内部一点，作为种子*/
            seed = difference.pts.back();
            difference.pts.pop_back();
        }

        auto tin_ptr = std::unique_ptr<Tin>(new Tin());
        auto &tin = *tin_ptr;

        // tin.set_bound_pts(data);
        auto pr = tin.get_bound_pts(data);
        auto xmax = pr.first.x, ymax = pr.first.y, xmin = pr.second.x, ymin = pr.second.y;
        tin.set_bound_pts(xmin, xmax, ymin, ymax);

        for (std::size_t i = 0; i < convex.pts.size(); i++)
        {
            auto &p1 = convex.pts[i];
            auto &p2 = convex.pts[(i + 1) % convex.pts.size()];

            tin.triangles.push_back(Triangle{seed, p1, p2});
            tin.legalize_edge(Line{seed, p1});
            tin.legalize_edge(Line{seed, p2});
            tin.legalize_edge(Line{p1, p2});
        }

        /*只有凸包边界，没有中心点，的情况*/
        if (data.size() == convex.pts.size())
            std::cerr << "空凸包错误";

        /*逐点添加*/
        for (auto &item : difference.pts)
            tin.grow(item);

        /*平移数据*/
        {
            std::for_each(data.begin(), data.end(), [&f1](Point &p)
                          { p += f1; });
            std::for_each(tin.triangles.begin(), tin.triangles.end(), [&f1](Triangle &tri)
                          {tri.pts[0] += f1;tri.pts[1]+=f1;tri.pts[2]+=f1; });
            std::for_each(tin.bound_pts.begin(), tin.bound_pts.end(), [&f1](Point &p)
                          { p += f1; });
        }
        return tin_ptr;
    }

    /*从超级包围三角形构造*/
    // [[deprecated]] std::unique_ptr<Tin> build_from_triangle(std::vector<Point> &data) {}

    /*从矩形框（两个三角形）构造*/
    static std::unique_ptr<Tin> build_from_rectangle(std::vector<Point> &data)
    {
        /*平移*/
        Point f1 = data[0];
        std::for_each(data.begin(), data.end(), [&f1](Point &p)
                      { p -= f1; });

        auto tin_ptr = std::unique_ptr<Tin>(new Tin());
        auto &tin = *tin_ptr;

        // tin.set_bound_pts(data);

        auto pr = tin.get_bound_pts(data);
        auto xmax = pr.first.x, ymax = pr.first.y, xmin = pr.second.x, ymin = pr.second.y;
        tin.set_bound_pts(xmin - 1, xmax + 1, ymin - 1, ymax + 1);

        tin.triangles.push_back(Triangle{tin.bound_pts[0], tin.bound_pts[1], tin.bound_pts[2]});
        tin.triangles.push_back(Triangle{tin.bound_pts[2], tin.bound_pts[3], tin.bound_pts[0]});

        for (auto &item : data)
            tin.grow(item);

        tin.clear_bound(); /*清理边界三角形*/

        std::for_each(data.begin(), data.end(), [&f1](Point &p)
                      { p += f1; });
        std::for_each(tin.triangles.begin(), tin.triangles.end(), [&f1](Triangle &tri)
                      {tri.pts[0] += f1;tri.pts[1]+=f1;tri.pts[2]+=f1; });
        std::for_each(tin.bound_pts.begin(), tin.bound_pts.end(), [&f1](Point &p)
                      { p += f1; });
        return tin_ptr;
    }
};

struct Shader
{
public:
    /*构造shader*/
    unsigned int ID;
    Shader(const char *v, const char *f) : ID(make_shader(v, f)) {}
    void use() const
    {
        glUseProgram(ID);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

private:
    unsigned int make_shader(const char *vertex_shader_source, const char *fragment_shader_source) const
    {
        // vertex shader
        int success;
        char infoLog[512];

        GLuint vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertex_Shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_Shader);

        glShaderSource(fragment_Shader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragment_Shader);

        glGetShaderiv(vertex_Shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_Shader, 512, NULL, infoLog);
            printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
        }
        glGetShaderiv(fragment_Shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment_Shader, 512, NULL, infoLog);
            printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
        }

        GLuint shader_Program = glCreateProgram();
        glAttachShader(shader_Program, vertex_Shader);
        glAttachShader(shader_Program, fragment_Shader);

        glLinkProgram(shader_Program);

        glGetProgramiv(shader_Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_Shader, 512, NULL, infoLog);
            printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        }

        glDeleteShader(vertex_Shader);
        glDeleteShader(fragment_Shader);
        return shader_Program;
    }
    /*回收shader资源*/
};

struct UI
{

public:
    UI()
    {
        my_opengl_init();
        /*RAII：
    Resource Acquisition Is Initialization,
    资源获取即初始化，使用局部对象（储存在栈中的对象）来管理资源的技术。
    资源使用三个步骤：获取资源、使用资源、销毁资源。
    借助析构函数销毁资源*/
    }
    void run(const std::vector<Point> &data, const Polygon &convex)
    {
        auto shader = Shader(vertex_shader_source, fragment_shader_source);

        std::vector<float> vert;

        for (auto &item : convex.pts)
        {
            vert.push_back(item.x);
            vert.push_back(item.y);
            vert.push_back(1.0f);
        }

        auto func_x = [&](const Point &a, const Point &b) -> bool
        { return a.x < b.x; };
        auto func_y = [&](const Point &a, const Point &b) -> bool
        { return a.y < b.y; };

        auto xmin = (*std::min_element(data.begin(), data.end(), func_x)).x;
        auto xmax = (*std::max_element(data.begin(), data.end(), func_x)).x;
        auto ymin = (*std::min_element(data.begin(), data.end(), func_y)).y;
        auto ymax = (*std::max_element(data.begin(), data.end(), func_y)).y;

        auto dx = xmax - xmin;
        auto dy = ymax - ymin;

        GLuint VBO, VAO[VAOS::numVAO];

        glGenVertexArrays(VAOS::numVAO, VAO);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO[VAOS::Points]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), vert.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO[VAOS::Triangle]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), vert.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        while (!glfwWindowShouldClose(window))
        {
            // My code;
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 状态函数：设置状态清屏颜色
            glClear(GL_COLOR_BUFFER_BIT);         // 动作函数：执行清屏动作
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            auto color = glm::vec3(1, 0, 0);

            glm::mat4 mv = glm::ortho(xmin - 2, xmax + 2, ymin - 2, ymax + 2);
            if (dx >= dy)
            {
                mv = glm::ortho(xmin - 2, xmax + 2, ymin - 2, ymax + 2 + (dx - dy));
            }
            else
            {
                mv = glm::ortho(xmin - 2, xmax + 2 + (dy - dx), ymin - 2, ymax + 2);
            }

            mv = glm::scale(mv, glm::vec3(gain_scale, gain_scale, 1.0f));
            mv = glm::translate(mv, glm::vec3(gain_x, gain_y, 0.0f));

            shader.use();
            glPointSize(4);
            glLineWidth(1);
            shader.setMat4("mv", mv);
            shader.setVec3("m_Color", color);

            glBindVertexArray(VAO[VAOS::Points]);
            glDrawArrays(GL_POINTS, 0, vert.size() / 3);

            glBindVertexArray(VAO[VAOS::Triangle]);
            glDrawArrays(GL_POLYGON, 0, vert.size() / 3);

            process_input(window);
            glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
            glfwSwapBuffers(window);

            glfwPollEvents();
            // glfwSetWindowShouldClose(window, GL_TRUE);
        }

        write_screen();
        glfwTerminate();
    }
    void run(const std::vector<Point> &data, const std::shared_ptr<Tin> &tin)
    {
        auto shader = Shader(vertex_shader_source, fragment_shader_source);

        std::vector<float> vert;

        for (auto &item : (*tin).triangles)
        {
            vert.push_back(item.pts[0].x);
            vert.push_back(item.pts[0].y);
            vert.push_back(1.0f);
            vert.push_back(item.pts[1].x);
            vert.push_back(item.pts[1].y);
            vert.push_back(1.0f);
            vert.push_back(item.pts[2].x);
            vert.push_back(item.pts[2].y);
            vert.push_back(1.0f);
        }

        auto xmax = (*tin).bound_pts[0].x; /*右上*/
        auto ymax = (*tin).bound_pts[0].y;
        auto xmin = (*tin).bound_pts[2].x; /*左下*/
        auto ymin = (*tin).bound_pts[2].y;

        auto dx = xmax - xmin;
        auto dy = ymax - ymin;

        GLuint VBO, VAO[VAOS::numVAO];

        glGenVertexArrays(VAOS::numVAO, VAO);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO[VAOS::Points]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), vert.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO[VAOS::Triangle]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), vert.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        while (!glfwWindowShouldClose(window))
        {
            // My code;
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 状态函数：设置状态清屏颜色
            glClear(GL_COLOR_BUFFER_BIT);         // 动作函数：执行清屏动作
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            auto color = glm::vec3(1, 0, 0);

            glm::mat4 mv = glm::ortho(xmin - 2, xmax + 2, ymin - 2, ymax + 2);
            if (dx >= dy)
            {
                mv = glm::ortho(xmin - 2, xmax + 2, ymin - 2, ymax + 2 + (dx - dy));
            }
            else
            {
                mv = glm::ortho(xmin - 2, xmax + 2 + (dy - dx), ymin - 2, ymax + 2);
            }

            mv = glm::scale(mv, glm::vec3(gain_scale, gain_scale, 1.0f));
            mv = glm::translate(mv, glm::vec3(gain_x, gain_y, 0.0f));

            // glUseProgram(shader.ID);
            shader.use();
            glPointSize(4);
            glLineWidth(1);
            shader.setMat4("mv", mv);
            shader.setVec3("m_Color", color);

            glBindVertexArray(VAO[VAOS::Points]);
            glDrawArrays(GL_POINTS, 0, vert.size() / 3);

            glBindVertexArray(VAO[VAOS::Triangle]);
            glDrawArrays(GL_TRIANGLES, 0, vert.size());

            process_input(window);
            glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
            glfwSwapBuffers(window);

            glfwPollEvents();
            // glfwSetWindowShouldClose(window, GL_TRUE);
        }

        write_screen();
        glfwTerminate();
    }
    GLFWwindow *window = NULL; /*GLFW窗体*/

private:
    int SCR_WIDTH = 800;  /*设备显示屏宽度*/
    int SCR_HEIGHT = 800; /*设备显示屏高度*/
    float gain_x = 0.0f, gain_y = 0.0f, gain_scale = 1.0f;

    /*vertex着色器*/
    const char *vertex_shader_source = R"glsl(
        #version 430 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 mv;
        void main()
        {
        gl_Position = mv * vec4(aPos, 1.0f);
        }
    )glsl";
    /*fragment着色器*/
    const char *fragment_shader_source = R"glsl(
        #version 430 core
        out vec4 FragColor;
        uniform vec3 m_Color;
        void main()
        {
        FragColor = vec4(m_Color, 1.0f);
        }
    )glsl";

    enum VAOS
    {
        Points,
        Triangle,
        numVAO
    };

    /*处理GLFW窗口键盘输入事件*/
    void process_input(GLFWwindow *window)
    {
        /*成员函数(非static)不能做回调函数*/

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            gain_x += 10.0f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            gain_x -= 10.0f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            gain_y -= 5.0f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            gain_y += 5.0f;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            gain_scale -= 0.04f;
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            gain_scale += 0.04f;
    }

    /*OpenGL，GLFW初始化工作*/
    void my_opengl_init()
    {
        if (!glfwInit())
            exit(-1);
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "show", NULL, NULL);

        if (!window)
        {
            printf("Failed to create window!!!\n");
            glfwTerminate();
            exit(-1);
        }

        glfwMakeContextCurrent(window);

        // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // glfwSetWindowSizeLimits(window, SCR_WIDTH, SCR_HEIGHT, SCR_WIDTH, SCR_HEIGHT);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            printf("Failed to initialize GLAD\n");
            exit(-1);
        }

        // OpenGL state
        // ------------
        // glEnable(GL_CULL_FACE);
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /*保存屏幕显示结果*/
    void write_screen() const
    {
        int width = SCR_WIDTH;
        int height = SCR_HEIGHT;
        /**获取屏幕显示的像素，以生成图像*/
        unsigned char *buffer = (unsigned char *)calloc(4, width * height);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        stbi_write_png("offscreen.png",
                       width, height, 4,
                       buffer + (width * 4 * (height)), -width * 4);
        free(buffer);
    }
};

/*读取数据点文件*/
void read_file(const std::string file, std::vector<Point> &data)
{
    {
        /*
        int id;
        double x, y, h;
        FILE *fp = freopen(file.data(), "r", stdin);
        while (scanf("%id,%f,%f,%f", &id, &x, &y, &h) != EOF)
        {
            data.push_back({x, y});
        }
        fclose(fp);
        */
    } {
        std::ifstream fcin(file);
        if (fcin.fail())
        {
            std::cout << "read data failed\n";
            return;
        }
        std::string line;
        std::stringstream ss;
        while (1)
        {
            ss.clear();
            line.clear();
            std::getline(fcin, line);
            if (line.size() == 0)
                break;
            int id = line.find_first_of(',');
            std::string str_id = line.substr(0, id);
            ss << line.substr(id);
            char comma;
            double x, y, z;
            ss >> comma >> x >> comma >> y >> comma >> z;
            // data.push_back({str_id, {x, y, z}});
            data.push_back({x, y, z, str_id});
        }
    }
    /*
    值得注意的是：
    float只能保证6到7位有效数字
    double则能保证15到16位

    float x = 1234567890.12345678901234567890f;
    double y = 1234567890.12345678901234567890;
    std::cout << std::fixed << std::setprecision(20) << x << std::endl;
    std::cout << std::fixed << std::setprecision(20) << y << std::endl;
    输出结果
    1234567936.00000000000000000000
    1234567890.12345671653747558594
    */
}
/*程序主入口*/
