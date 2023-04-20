#include "utils.hpp"

int main(int argc, char **argv)
{
    auto cal_start = std::chrono::system_clock::now();

    std::vector<Point> data;
    read_file(argv[1], data);
{
    // auto tin = std::shared_ptr<Tin>(Tin::build_from_rectangle(data));
    // auto tin = std::shared_ptr<Tin>(Tin::build_from_convex_with_seed(data));
    auto tin = std::shared_ptr<Tin>(Tin::build_from_convex_2(data));
    // auto tin = std::shared_ptr<Tin>(Tin::build_from_baseline(data));
}
    auto cal_ok = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(cal_ok - cal_start).count();
    printf("计算用时%dms\n", diff);
    printf("数据规模：%d个点,%d个三角形", data.size(), (*tin).triangles.size());
    std::endl(std::cout);

    UI ui;
    ui.run(data, tin);
    return 0;
}

/*
int nmain(int argc, char **argv)
{
    Data data;
    Tin_Builder builder;
    builder.read_file(argv[1], data);

    // auto tin = std::shared_ptr<Tin>(builder.build_from_rectangle(data));
    // auto tin = std::shared_ptr<Tin>(builder.build_from_convex_with_seed(data));
    auto tin = std::shared_ptr<Tin>(builder.build_from_convex_2(data));

    UI ui;
    ui.run(data, tin);
    // std::cout<<"size="<<(*tin).triangles.size();

    return 0;
}

int mainn(int argc, char **argv)
{

    Data data;
    Tin_Builder builder;
    builder.read_file(argv[1], data);

    Polygon pg;
    pg.pts = data;

    Polygon convex = pg.get_convex();

    UI ui;

    ui.run(data, convex);

    // // convex.print();

    std::cout << convex.pts.size() << "\n";

    // pg.print();

    return 0;
}

*/