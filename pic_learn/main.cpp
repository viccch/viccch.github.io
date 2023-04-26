#include <bits/stdc++.h>
#include "TGA/tgaimage.h"
int main()
{
    TGAImage *tga = new TGAImage(100, 100, 3);

    TGAColor color_blue{0xff, 0, 0, 0};
    TGAColor color_green{0, 0xff, 0, 0};
    TGAColor color_red{0, 0, 0xff, 0};

    for (int i = 0; i < tga->width(); i++)
        tga->set(50, i, color_red);

    tga->write_tga_file("./a.tga");

    return 0;
}
