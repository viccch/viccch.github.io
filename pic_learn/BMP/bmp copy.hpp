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

    ~BMP()
    {
        this->bmp_free();
    }

    void bmp_read(const char *fname)
    {
        BMP::bmp_read(fname, this);
        return;
    }
    void bmp_free()
    {
        BMP::bmp_free(this);
        return;
    }

    static void bmp_free(BMP *bmp)
    {
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
    static void bmp_read(const char *fname, BMP *bmp)
    {
        /*建立fp，读取文件*/
        auto fp = std::fstream();
        fp.open(fname, std::ios::binary | std::ios::in);

        if (fp.fail())
        {
            std::cerr << "BMP Open FAILED!\n";
            return;
        }

        fp.read(reinterpret_cast<char *>(&bmp->bf_header), sizeof(BMP::Header));
        fp.read(reinterpret_cast<char *>(&bmp->bf_info), sizeof(BMP::Info));

        /*BMP 4d42*/
        if (0x4d42 != bmp->bf_header.bfType)
        {
            std::cerr << "file appears NOT to be a BMP";
            return;
        }

        /*调色板大小（字节）*/
        int size_color_palette = bmp->bf_header.bfOffBits - (bmp->bf_info.biSize + sizeof(BMP::Header));

        /*如果有调色板，申请内存，读取颜色索引*/
        if (0 < size_color_palette)
        {
            /*RAII处理*/
            bmp->bf_palette = (BMP::Color_Palette *)malloc(size_color_palette);
            fp.read(reinterpret_cast<char *>(bmp->bf_palette), size_color_palette);
        }

        uint32_t bmp_size = bmp->bf_info.biWidth * bmp->bf_info.biHeight; /*data大小*/
        if (8 < bmp->bf_info.biBitCount)
            bmp_size *= bmp->bf_info.biBitCount / 8;

        bmp->bf_data = (uint8_t *)malloc(bmp_size); /*申请内存，存放像素*/
        memset(bmp->bf_data, 0, bmp_size);          /*给所有像素初始值0*/

        // uint32_t buf = 0;                                 /*由于每行最少4个字节，使用4个字节作为一个buffer*/
        uint32_t rows = bmp->bf_info.biHeight;            /*每行像素数量，列数*/
        uint32_t columns = bmp->bf_info.biWidth;          /*每行像素数量，列数*/
        uint32_t bits_per_time = bmp->bf_info.biBitCount; /*每次loop处理像素数量*/
        uint32_t offset = (8 - bmp->bf_info.biBitCount);  /**/
        uint32_t cmp = [&bmp]() -> int
        {int cmp=0;for (int i = 0; i < bmp->bf_info.biBitCount; i++){cmp <<= 1;cmp |= 1;}return cmp; }();

        if (32 == bmp->bf_info.biBitCount)
        /*32位位图*/
        {
            fp.read((char *)bmp->bf_data, bmp_size);
        }
        else if (24 == bmp->bf_info.biBitCount)
        /*24位位图*/
        {
            for (int i = 0; i < rows; i++)
            {
                fp.read((char *)&bmp->bf_data[i * columns * 3], columns * 3);
                // fp.seekg(i * (columns + sk));
                int sk = 4 - ((columns * 3) % 4);
                fp.seekg(sk, std::ios::cur);
            }
        }
        else
        /*使用调色板（1、2、4、8位）的位图*/
        {
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    uint8_t buf_char;
                    fp.read((char *)&buf_char, sizeof(buf_char));
                    while (buf_char)
                    {
                        bmp->bf_data[i * columns + j] = (uint8_t)((buf_char >> offset) & cmp);
                        buf_char <<= bits_per_time;
                        j++;
                        if (j >= columns)
                            break;
                    }
                }
            }
        }

        /*释放资源*/
        fp.close();
        return;
    }

    static void
    bmp_dump_24bit(const char *fname, BMP *bmp)
    {
        auto fp = std::fstream();
        fp.open(fname, std::ios::out | std::ios::binary);

        fp.write((char *)&(bmp->bf_header), sizeof(BMP::Header));
        fp.write((char *)&(bmp->bf_info), sizeof(BMP::Info));
        if (!(bmp->bf_header.bfOffBits == bmp->bf_info.biSize + sizeof(BMP::Header) &
              nullptr != bmp->bf_palette))
        {
            int size_color_palette = bmp->bf_header.bfOffBits - (bmp->bf_info.biSize + sizeof(BMP::Header));

            fp.write((char *)(bmp->bf_palette), size_color_palette);
        }

        int sk = 4 - (bmp->bf_info.biWidth * 3) % (4);

        if (0 == sk)
        {
            fp.write((char *)bmp->bf_data, (bmp->bf_info.biHeight) * (bmp->bf_info.biWidth) * 3);
        }
        else
        {
            for (int i = 0; i < bmp->bf_info.biHeight; i++)
            {
                fp.write((char *)bmp->bf_data, (bmp->bf_info.biWidth) * 3);
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

    friend std::ostream &operator<<(std::ostream &fp, const BMP &bmp)
    {
        if (24 == bmp.bf_info.biBitCount)
        {
            for (int i = 0; i < bmp.bf_info.biHeight; i++)
            {
                for (int j = 0; j < bmp.bf_info.biWidth; j++)
                {
                    fp << "(";
                    fp << (uint32_t)bmp.bf_data[(i * bmp.bf_info.biWidth + j) * 3] << ",";
                    fp << (uint32_t)bmp.bf_data[(i * bmp.bf_info.biWidth + j) * 3 + 1] << ",";
                    fp << (uint32_t)bmp.bf_data[(i * bmp.bf_info.biWidth + j) * 3 + 2] << ")\t";
                }
                fp << "\n";
            }
        }
        else
        {
            for (int i = 0; i < bmp.bf_info.biHeight; i++)
            {
                for (int j = 0; j < bmp.bf_info.biWidth; j++)
                {
                    fp << (uint32_t)bmp.bf_data[i * bmp.bf_info.biWidth + j] << " ";
                }
                fp << "\n";
            }
        }
        return fp;
    }
};