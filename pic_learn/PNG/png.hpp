#include <bits/stdc++.h>

namespace Endian
{
    /*判断本地字节序*/
    bool is_big_endian()
    {
        uint32_t x = 0xff;
        // uint8_t *p = (uint8_t *)(&x);
        uint8_t *p = reinterpret_cast<uint8_t *>(&x);
        return 0 == (*p);
    }

    template <typename T>
    T hton(T v)
    {
        /**
         * 本地字节序（unknown）转网络字节序（大端）：
         * 判断本地字节序，若本地大端，不做处理，直接返回；
         * 若本地为小端，进行翻转处理成大端。
         */
        if (!is_big_endian())
        {
            T r = 0;
            for (int i = 0; i < sizeof(T); i++)
                r |= (v << (8 * i) >> (8 * (sizeof(T) - 1)) << (8 * i));
            // std::cout << std::hex << r << std::endl;
            v = r;
        }
        return v;
    }

    /*{//测试
        uint16_t h = 0xff00;
        uint16_t n = Endian::hton(h);

        uint32_t a = 0x1122'3344;
        uint32_t b = Endian::hton(a);

        uint64_t c = 0x1122'3344'5566'7788;
        uint64_t d = Endian::hton(c);
    }*/
}

struct PNG
{
#pragma pack(push)
#pragma pack(1)
    /*文件头格式*/
    /*89 50 4E 47 0D 0A 1A 0A*/

    struct Basic_Chunk;

    /*关键数据块 Critical Chunk*/

    // struct IHDR; /*header chunk，包含图像基本信息，第一个出现，只出现一次*/
    struct PLTE; /*palette chunk，调色板数据块，必须在图像数据库之前*/
    struct IDAT; /*image data chunk，存储实际的图像　数据，PNG　数据包允许包含多个连续的图像数据块*/
    struct IEND; /*image trailer chunk，图像结束数据表示PNG流数据结束*/

    /*辅助数据块 Ancillary Chunk*/
    struct bKGD; /*背景颜色数据块*/
    struct cHRM; /*基色和白色度数据块*/
    struct gAMA; /*图像γ数据块*/
    struct hIST; /*图像直方图数据块*/
    struct pHYs; /*物理像素尺寸　数据块*/
    struct sBIT; /*样本有效位数数据块*/
    struct tEXt; /*文本信息数据块*/
    struct tIME; /*图像最后修改时间　*/
    struct tRNS; /*图像透明数据块*/
    struct zTXt; /*压缩文本数据块*/

    struct Basic_Chunk
    {
        uint32_t length;     /**/
        uint32_t chunk_type; /*数据块类型码，ASCII码字母块组成*/
        uint32_t CRC;        /*循环冗余检测*/
    };

    struct IHDR : public Basic_Chunk
    {                   /*IHDR*/
        uint32_t width; /*像素为单位*/
        uint32_t height;
        uint8_t bit_depth;          /*图像深度，索引灰度图像：１，２，４，８；灰度图像：１，２，４，８，１６；真彩色图像；８或１６*/
        uint8_t color_type;         /*颜色类型：0：灰度（1，2，4，8，16）；2：真彩色（8，16），3：索引彩色（1，2，4，8），4：带α通道数据的灰度图像（8，16），6：带α通道的真彩色图像（8，16）*/
        uint8_t compression_method; /*压缩方法（LZ77派生算法）*/
        uint8_t filter_mothod;      /*滤波器方法*/
        uint8_t interlace_method;   /*隔行扫描方法　：0：非隔行扫描；1：Adam7（7遍隔行扫描方法）*/
    };

    struct PLTE : public Basic_Chunk
    { /*PLTE*/

        struct RGB
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } rgb[256];
        uint32_t size;
    };

    struct IDAT : public Basic_Chunk
    {
        /*IDAT*/
        uint8_t data[8192];

        // IDAT() : data(nullptr) {}
        // // IDAT(uint32_t n) : data(new uint8_t[n]) {}
        // ~IDAT()
        // {
        //     if (data != nullptr)
        //         delete data;
        // }
    };
    struct IEND : public Basic_Chunk
    {
        /* data */
    };

#pragma pack(pop)
    /*数据*/
    IHDR idhr;
    PLTE plte;
    IDAT idat;
    std::vector<IDAT> idats;
    IEND iend;

    /*方法*/
    void png_read(const char *fname)
    {
        auto fp = std::fstream();
        fp.open(fname, std::ios::in | std::ios::binary);

        if (fp.fail())
        {
            std::cerr << "png open failed!\n";
            return;
        }

        uint8_t hd[8];
        const uint8_t hd_c[8]{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

        fp.read((char *)&(hd), 8);

        for (int i = 0; i < 8; i++)
        {
            if (hd[i] - hd_c[i])
            {
                std::cerr << "not png file\n";
                return;
            }
        }

        auto str_2_hex = [](const char str[4])
        {
            return (uint32_t)(str[0] << 24 | str[1] << 16 | str[2] << 8 | str[3]);
        };

        auto hex_2_str = [](uint32_t num) -> std::string
        {
            char a = num << 24 >> 24;
            char b = num << 16 >> 24;
            char c = num << 8 >> 24;
            char d = num << 0 >> 24;
            return {d, c, b, a};
        };

        // 0x49484452;

        uint32_t lenth = 0, type = 0;
        while (fp.read((char *)&(lenth), 4).read((char *)&(type), 4))
        {
            lenth = Endian::hton(lenth);
            type = Endian::hton(type);

            // printf("length = %d type = %d type_str = %s\n", lenth, type, hex_2_str(type).data());

            switch (type)
            {
            case str_2_hex("IHDR"):
            {
                IHDR *chunk = &(this->idhr);

                this->idhr.length = lenth;
                this->idat.chunk_type = type;

                fp.read((char *)((uint8_t *)chunk + sizeof(Basic_Chunk)), chunk->length);
                fp.read((char *)&(this->idhr.CRC), 4);
                this->idhr.CRC = Endian::hton(this->idhr.CRC);
                this->idhr.width = Endian::hton(this->idhr.width);
                this->idhr.height = Endian::hton(this->idhr.height);
                break;
            }
            case str_2_hex("PLTE"):
            {
                PLTE *chunk = &(this->plte);

                chunk->length = lenth;
                chunk->chunk_type = type;

                chunk->size = lenth / 3;

                fp.read((char *)((uint8_t *)chunk + sizeof(Basic_Chunk)), chunk->length);
                fp.read((char *)&(this->idhr.CRC), 4);
                this->idhr.CRC = Endian::hton(this->idhr.CRC);

                break;
            }
            case str_2_hex("IDAT"):
            {
                IDAT *chunk = &(this->idat);

                chunk->length = lenth;
                chunk->chunk_type = type;

                fp.read((char *)((uint8_t *)chunk + sizeof(Basic_Chunk)), chunk->length);
                fp.read((char *)&(this->idhr.CRC), 4);
                this->idhr.CRC = Endian::hton(this->idhr.CRC);

                this->idats.push_back(*chunk);

                break;
            }
            case str_2_hex("IEND"):
            {
                fp.read((char *)&(this->idhr.CRC), 4);
                this->idhr.CRC = Endian::hton(this->idhr.CRC);

                // std::cout << std::hex << fp.tellg() << std::endl;
                break;
            }
            default:
            {
                // printf("length = %d type = %d\n", lenth, type);
                fp.seekg(lenth, fp.cur);
                break;
            }
            }
        }
        fp.close();

        // printf("%X\n", &this->idhr);
        // printf("%X\n", &this->idhr.length);
        // printf("%X\n", &this->idhr.chunk_type);
        // printf("%X\n", &this->idhr.CRC);

        // printf("%X\n", &this->idhr.width);
        // printf("%X\n", &this->idhr.height);
        // printf("%X\n", &this->idhr.interlace_method);
    }
};