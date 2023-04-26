#include <bits/stdc++.h>

struct BMP
{
    /*内存对齐*/
#pragma pack(push)
#pragma pack(1)
    struct Header
    {
        uint16_t bfType;      /*文件类型标志*/
        uint32_t bfSize;      /*记录文件大小，单位字节*/
        uint16_t bfReserved1; /*保留字*/
        uint16_t bfReserved2; /*保留字*/
        uint32_t bfOffBits;   /*从头到位图数据的偏移量*/
    };

    struct Info
    {
        uint32_t biSize;          /*信息头的大小*/
        uint32_t biWidth;         /*图像宽度*/
        uint32_t biHeight;        /*图像高度，如果为正，说明位图倒立（左下角到右上角），为负说明正向*/
        uint16_t biPlanes;        /*颜色平面数，总被设为1*/
        uint16_t biBitCount;      /*说明比特数/像素数，值有1、2、4、8、16、24、32*/
        uint32_t biCompression;   /*说明图像压缩类型，常用0（BI_RGB），表示不压缩*/
        uint32_t biSizeImage;     /*说明位图数据的大小，当用BI_RGB格式，可以设为0*/
        uint32_t biXPelsPerMeter; /*水平分辨率，单位是像素/米，有符号整数*/
        uint32_t biYPelsPerMeter; /*垂直分辨率，单位是像素/米，有符号整数*/
        uint32_t biClrUsed;       /*说明位图使用的调色板中的颜色索引 数，为0说明使用所有*/
        uint32_t biClrImportant;  /*说明对图像显示有重要影响的颜色索引数，为0说明都重要*/
    };

    struct Color_Palette
    {
        uint8_t B;
        uint8_t G;
        uint8_t R;
        uint8_t Alpha;
    };

#pragma pack(pop)

    BMP::Header bf_header;
    BMP::Info bf_info;
    BMP::Color_Palette *bf_palette;
    uint8_t *bf_data;

    BMP() : bf_palette(nullptr), bf_data(nullptr) {}
    ~BMP() {}

    virtual void bmp_read(const char *) = 0;
    virtual void bmp_dump(const char *) = 0;
    virtual void bmp_free() = 0;
    virtual void print(std::ostream &) = 0;
    virtual void set_pixel(int, int, int _color) = 0;
};

struct BMP_bitmap : public BMP
{

    virtual void print(std::ostream &fp)
    {
        auto bmp = this;

        for (int i = 0; i < bmp->bf_info.biHeight; i++)
        {
            for (int j = 0; j < bmp->bf_info.biWidth; j++)
            {
                fp << (uint32_t)bmp->bf_data[i * bmp->bf_info.biWidth + j] << " ";
            }
            fp << "\n";
        }
    }
};

struct BMP_24bit : public BMP
{

    BMP_24bit() {}
    ~BMP_24bit() {}

    virtual void bmp_read(const char *fname)
    {
        auto bmp = this;
        auto fp = std::fstream();
        fp.open(fname, std::ios::binary | std::ios::in);

        if (fp.fail())
        {
            std::cerr << "BMP Open FAILED!\n";
            return;
        }

        fp.read((char *)(&bmp->bf_header), sizeof(BMP::Header));
        fp.read((char *)(&bmp->bf_info), sizeof(BMP::Info));

        /*File CHECK*/
        {
            if (0x4d42 != bmp->bf_header.bfType)
            {
                /*BMP 4d42*/
                std::cerr << "file appears NOT to be a BMP!\n";
                return;
            }
            if (24 != bmp->bf_info.biBitCount)
            {
                std::cerr << "not 24_bit BMP!\n";
                return;
            }
            if (bmp->bf_header.bfOffBits != sizeof(BMP::Header) + bmp->bf_info.biSize)
            {
                std::cerr << "BMP size error!\n";
                return;
            }
        }

        uint32_t rows = bmp->bf_info.biHeight;   /*每行像素数量，列数*/
        uint32_t columns = bmp->bf_info.biWidth; /*每行像素数量，列数*/

        uint32_t bmp_size = rows * columns; /*data大小*/

        if (8 < bmp->bf_info.biBitCount)
            bmp_size *= bmp->bf_info.biBitCount / 8;

        bmp->bf_data = (uint8_t *)malloc(bmp_size); /*申请内存，存放像素*/
        memset(bmp->bf_data, 0, bmp_size);          /*给所有像素初始值0*/

        for (int i = 0; i < rows; i++)
        {
            fp.read((char *)&bmp->bf_data[i * columns * 3], columns * 3);
            int sk = 4 - ((columns * 3) % 4);
            fp.seekg(sk, std::ios::cur);
        }

        fp.close(); /*释放资源*/
        return;
    }

    virtual void bmp_dump(const char *fname)
    {
        auto bmp = this;
        auto fp = std::fstream();
        fp.open(fname, std::ios::out | std::ios::binary);

        fp.write((char *)&(bmp->bf_header), sizeof(BMP::Header));
        fp.write((char *)&(bmp->bf_info), sizeof(BMP::Info));

        int rows = bmp->bf_info.biHeight;
        int columns = bmp->bf_info.biWidth;
        int sk = 4 - (columns * 3) % (4);

        if (0 == sk)
        {
            fp.write((char *)bmp->bf_data, (rows) * (columns)*3);
        }
        else
        {
            for (int i = 0; i < rows; i++)
            {
                fp.write((char *)&(bmp->bf_data[i * columns * 3]), (columns)*3);
                for (int j = 0; j < sk; j++)
                {
                    uint8_t ch = 0;
                    fp.write((char *)&ch, 1);
                }
            }
        }
        fp.close();
        return;
    }

    virtual void bmp_free()
    {
        auto bmp = this;
        if (nullptr != bmp->bf_palette)
        {
            free(bmp->bf_palette);
            bmp->bf_palette = nullptr;
        }
        if (nullptr != bmp->bf_data)
        {
            free(bmp->bf_data);
            bmp->bf_data = nullptr;
        }
        return;
    }

    virtual void print(std::ostream &fp)
    {
        auto bmp = this;
        for (int i = 0; i < bmp->bf_info.biHeight; i++)
        {
            for (int j = 0; j < bmp->bf_info.biWidth; j++)
            {
                fp << "(";
                fp << (uint32_t)bmp->bf_data[(i * bmp->bf_info.biWidth + j) * 3] << ",";
                fp << (uint32_t)bmp->bf_data[(i * bmp->bf_info.biWidth + j) * 3 + 1] << ",";
                fp << (uint32_t)bmp->bf_data[(i * bmp->bf_info.biWidth + j) * 3 + 2] << ")\t";
            }
            fp << "\n";
        }
        return;
    }

    virtual void set_pixel(int x, int y, int color)
    {
        auto bmp = this;
        int pos = 3 * (this->bf_info.biWidth * y + x); /*结构化绑定 structure binding*/
        auto [r, g, b] = std::make_tuple(color << 24 >> 24, color << 16 >> 24, color << 8 >> 24);
        this->bf_data[pos] = b;
        this->bf_data[pos + 1] = g;
        this->bf_data[pos + 2] = r;
        // this->bf_data[pos] = color << 24 >> 24;
        // this->bf_data[pos + 1] = color << 16 >> 24;
        // this->bf_data[pos + 2] = color << 8 >> 24;
        return;
    }
};

static constexpr unsigned int RGBA(int R, int G, int B, int A)
{
    // return ((((((A << 8) | B) << 8) | G) << 8) | R);
    return ((A << 24) | (B << 16) | (R << 8) | R);
}

static constexpr unsigned int RGB(int R, int G, int B)
{
    return ((B << 16) | (G << 8) | R);
}