

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

struct Matrix
{
public:
    uint8_t *data = nullptr;
    int width;
    int height;

    uint8_t &operator()(int x, int y) { return this->data[x + width * y]; }
    uint8_t &operator[](int idx) { return this->data[idx]; }

    Matrix() : Matrix(0, 0, nullptr) {}
    Matrix(int w, int h) : Matrix(w, h, (new uint8_t[w * h]())) {} /*new时加括号，申请的空间自动置零*/
private:
    /*禁止外部使用这个构造函数！！，否则很可能导致多次对data指向的内存进行delete！！！*/
    Matrix(int w, int h, uint8_t *_data) : width(w), height(h), data(_data) {}

public:
    /**
     * 移动拷贝构造
     */
    Matrix(Matrix &&mat) : width(mat.width), height(mat.height), data(mat.data) { mat.data = nullptr; }

    /**
     * 析构
     */
    ~Matrix()
    {
        if (nullptr != this->data)
        {
            delete this->data;
            this->data = nullptr;
        }
    }

    void print()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 5; j++)
                std::cout << (int)((*this)(i, j)) << " ";
            std::cout << "\n";
        }
    }
};

struct JPEG
{
#pragma pack(push)
#pragma pack(1)

    /*
    ---------------------------------------
    名称  标记码     说明
    ---------------------------------------
    SOI     D8          文件头
    EOI     D9          文件尾
    SOF0    C0          帧开始（标准 JPEG）
    SOF1    C1          同上
    DHT     C4          定义 Huffman 表（霍夫曼表）
    SOS DA          扫描行开始
    DQT DB          定义量化表
    DRI     DD          定义重新开始间隔
    APP0    E0          定义交换格式和图像识别信息
    COM FE          注释
    -----------------------------------------------------------
    */
    /*JPEG大端字节序*/

    struct SOI /*文件头*/
    {
        uint8_t id; /*FF，D8*/
        uint8_t type;
    };
    struct APP0 /*图像识别信息*/
    {
        // uint8_t id;            /*段标识，FF*/
        // uint8_t type;          /*段类型，E0，E1*/
        uint16_t size;           /*段长度，从这里开始计算，3n+16*/
        uint8_t format[5] = {0}; /*交换格式,JFIF'0'*/
        uint8_t version_major;   /*主版本号*/
        uint8_t version_minor;   /*次版本号*/
        uint8_t density_unit;    /*密度单位（0=无单位；1=点数/英寸；2=点数/厘米）*/
        uint16_t density_x;      /*x像素密度（水平方向）*/
        uint16_t density_y;      /*y像素密度*/
        uint8_t thumbnail_x;     /*缩略图x（水平）像素数量*/
        uint8_t thumbnail_y;     /*缩略图y（垂直）像素数量*/
        uint8_t *thumb = nullptr;
    };
    struct DQT /*定义量化表*/
    {
        // uint8_t id;    /*段标识，FF*/
        // uint8_t type;  /*段类型，D8*/
        uint16_t size; /*段长度，2+（1+n）*2*/

        struct QT
        {
            uint8_t qt_info; /*QT信息*/
            /*0-3位（右）QT号，4-7位（左）QT精度（0:8bit,否则16bit）*/
            uint8_t qt[64] = {0}; /*QT*/
        };

        QT qt_y;
        QT qt_c;

        /*JPEG一般2个DQT段，Y（亮度），C（色度）各一个*/
    };
    struct SOF0 /*图像基本信息*/
    {
        // uint8_t id;         /*段标识，FF*/
        // uint8_t type;       /*段类型，C0*/
        uint16_t size;      /*段长度，8+3*组件数量*/
        uint8_t presicison; /*样本精度，每个样本位数，一般是8*/
        uint16_t height;    /*图片高度*/
        uint16_t width;     /*图片宽度*/
        uint8_t unit_cnt;   /*组件数量*/

        struct Unit
        {
            uint8_t id;            /*标志： 1＝Y, 2＝Cb, 3＝Cr, 4＝I, 5＝Q*/
            uint8_t sample_factor; /*采样系数:0－3位：垂直采样系数,4－7位：水平采样系数*/
            uint8_t table_id;      /*量化表号*/
        };

        Unit units[6] = {0}; /*组件，每个组件占3个字符*/

        /*
        JPEG大都采用yCrCb色彩模型（y表示亮度，Cr红色分量，Cb表示蓝色分量），所以组件数量一般＝3,
        样本就是单个像素的颜色分量，也可理解为一个样本就是一个组件
        采样系数是实际采样方式与最高采样系数之比，而最高采样系数一般＝0.5（分数表示为1/2）。比如说，垂直采样系数＝2，那么2×0.5＝1，表示实际采样方式是每个点采一个样，也就是逐点采样；如果垂直采样系数＝1，那么：1×0.5＝0.5（分数表示为1/2），表示每２个点采一个样
        */
    };
    struct DHT /*定义Huffman表*/
    {
        // uint8_t id;    /*段标识，FF*/
        // uint8_t type;  /*段类型，C4*/
        uint16_t size; /*段长度，n+19*/

        struct HT
        {
            uint8_t ht_info;              /*HT信息，0-3位：HT号；4位：HT类型，0=DC表，1＝AC表；5-7位：必须为0*/
            uint8_t ht_table[16] = {0};   /*16个数和<=256*/
            uint8_t ht_values[162] = {0}; /*HT值表，ｎ字节，ｎ＝表头16个数的和*/
        };

        HT ht_y_dc;
        HT ht_y_ac;
        HT ht_c_dc;
        HT ht_c_ac;

        /*
        ①JPEG文件里有２类Haffman 表：一类用于DC（直流量），一类用于AC（交流量）。一般有４个表：亮度的DC和AC，色度的DC和AC。最多可有６个。

        ②一个DHT 段可以包含多个HT表, 每个都有自己的信息字节

        ③HT表是一个按递增次序代码长度排列的符号表。
        */
    };

    struct COM /*注释段*/
    {
        // uint8_t id;                  /*段标识，FF*/
        // uint8_t type;                /*段类型，FE*/
        uint16_t size;               /*注释字节数+2*/
        uint8_t *comments = nullptr; /*注释*/
        /*段内容，注释字符*/
    };
    struct DRI /*重新定义开始间隔*/
    {
        // uint8_t id;        /*段标识，FF*/
        // uint8_t type;      /*段类型，DD*/
        uint16_t size;     /*4*/
        uint16_t interval; /*复位标记的间隔距离*/

        /*
        ①开始间隔表示在压缩数据流中，每隔ｎ个MCU 块就有一个RST标记，RST标记将Huffman 的解码数据流复位，DC也重新从０开始，因此，RST标记是一种复位标记
        ②RST 标记是一种特殊的段，它只具有段标识和段类型（长度＝２字节），但它不是独立的段，只能穿插在数据流中（文件头和文件尾段也只有段标识和段类型，却都是独立的段）。
        ③RST标记共有８个（RST0－RST7），从RST0起开始使用，然后是RST1….直至RST7，再从RST0重复。
        ④RST标记的标识码是 FFD0－FFD7，对应 RST0－RST7
        说明：这个不是必须段，很多JPEG都没有。
        */
    };
    struct SOS /*SOS扫描行开始*/
    {
        // uint8_t id;    /*段标识，FF*/
        // uint8_t type;  /*段类型，DA*/
        uint16_t size; /*段长度，6+2x扫描行内组件数量 */

        uint8_t unit_cnt; /*扫描行内组件数量，必须>=1，<=4，通常=3*/
                          /*每个组件占2字节*/

        struct Unit
        {
            uint8_t unit_id;          /*组件ID, 1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q*/
            uint8_t huffman_table_id; /*Huffman表号，0-3位：AC表(值=0...3)；4-7位：DC表号（值＝0...3）*/
        };

        Unit units[6] = {0};

        uint8_t unkown[3] = {0}; /*用途不明*/

        /*
        说明：紧接SOS段后的是压缩的图像数据（一个个扫描行），数据存放顺序是从左到右、从上到下。
        */
    };
    // struct EOI /*文件结尾*/
    // {
    //     // uint8_t id;   /*FF*/
    //     // uint8_t type; /*D9*/
    // };
#pragma pack(pop)

    SOI soi;   /*文件头*/
    APP0 app0; /*定义交换格式和图像识别信息*/
    DQT dqt;   /*定义量化表*/
    SOF0 sof0; /*帧开始*/
    DHT dht;   /*定义Huffman表*/
    SOS sos;   /*扫描行开始*/
    DRI dri;   /*重新定义开始间隔*/
    COM com;   /*注释*/

    uint8_t *code_buffer = nullptr; /*未解码数据*/
    uint32_t code_buffer_size;      /*未解码数据大小*/

    Matrix *mat;

    JPEG(){};
    ~JPEG() { jpeg_free(); };

    void decode()
    {
        JPEG *jpeg = this;
        int width = jpeg->sof0.width;
        int height = jpeg->sof0.height;

        uint8_t *buffer = jpeg->code_buffer;
        int buffer_size = jpeg->code_buffer_size;

        Matrix *mat = new Matrix(width * 3, height); /*rgb*/

        int row_blocks = (width + 15) >> 4;     /*垂直方向分块*/
        int column_blocks = (height + 15) >> 4; /*水平方向分块*/

        uint32_t pos = 0; /*当前code_buffer的指针位置*/

        auto Next_Pos=[&pos,this](){

        };

        /*解码*/
        for (int i = 0; i < row_blocks; i++)
        {
            for (int j = 0; j < column_blocks; j++)
            {
                if (0xff == code_buffer[i])
                {
                    switch (code_buffer[0])
                    {
                    case 0:
                        /* code */
                        break;

                    default:
                        break;
                    }
                }
            }
        }

        {

            /*量化表： jpeg->dqt.qt_y.qt;*/
            /*量化表： jpeg->dqt.qt_c.qt;*/
        }
        jpeg->mat = mat;
        return;
    }

    void jpeg_read(const char *fname)
    {
        auto fp = std::fstream();
        fp.open(fname, std::ios::in | std::ios::binary);

        if (fp.fail())
        {
            std::cerr << "jpeg open failed!\n";
            return;
        }

        SOI header; /*段头信息 */
        /*************************************************************/

        /*SOI   D8    文件头*/
        auto response_D8 = [&fp, this, &header]()
        {
            if (0xFF != header.id || 0xD8 != header.type)
            {
                std::cerr << "not jpeg file!\n";
                return;
            }
        };

        /*EOI   D9  文件尾*/
        auto response_D9 = [&fp, this, &header]()
        {
            std::cout << "read finished!\n";
        };

        /*SOF0 C0 帧开始（标准JPEG）*/
        auto response_C0 = [&fp, this, &header]()
        {
            uint16_t size;
            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);
            // fp.seekg(size - 2, std::ios::cur);
            this->sof0.size = size;
            fp.read((char *)&(this->sof0.presicison), 1);
            fp.read((char *)&(this->sof0.height), 2);
            fp.read((char *)&(this->sof0.width), 2);
            this->sof0.height = Endian::hton(this->sof0.height);
            this->sof0.width = Endian::hton(this->sof0.width);
            fp.read((char *)&(this->sof0.unit_cnt), 1);
            fp.read((char *)&(this->sof0.units), (this->sof0.unit_cnt) * sizeof(SOF0::Unit));
        };

        /*SOF1 C1 帧开始（）*/
        auto response_C1 = [&fp, this, &header]()
        {
            std::cout << "ss\n";
            uint16_t size;
            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);
            fp.seekg(size - 2, std::ios::cur);
        };

        /*DHT C4 定义Huffman表*/
        auto response_C4 = [&fp, this, &header]()
        {
            uint16_t size;

            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);

            this->dht.size = size;

            size -= 2;
            while (size)
            {
                DHT::HT *ht = nullptr;
                uint8_t ht_info = 0;
                fp.read((char *)&(ht_info), 1);

                switch (ht_info)
                {
                case 0x00:
                    ht = &(this->dht.ht_y_dc);
                    break;
                case 0x10:
                    ht = &(this->dht.ht_y_ac);
                    break;
                case 0x01:
                    ht = &(this->dht.ht_c_dc);
                    break;
                case 0x11:
                    ht = &(this->dht.ht_c_ac);
                    break;
                default:
                    std::cerr << "暂不支持的DHT\n";
                    exit(0);
                    break;
                }

                fp.read((char *)&(ht->ht_table), 16);

                uint32_t n = 0;
                for (int i = 0; i < 16; i++)
                    n += ht->ht_table[i];

                // std::cout << fp.tellg() << "\n";
                // assert(n == size - 19);

                fp.read((char *)&(ht->ht_values), n);
                size -= (n + 16 + 1);
            }
        };

        /*SOS DA 扫描行开始*/
        auto response_DA = [&fp, this, &header]()
        {
            fp.read((char *)&(this->sos.size), 2);
            this->sos.size = Endian::hton(this->sos.size);

            fp.read((char *)&(this->sos.unit_cnt), 1);
            uint32_t n = this->sos.unit_cnt;

            for (int i = 0; i < n; i++)
                fp.read((char *)&(this->sos.units[i]), sizeof(SOS::Unit));

            fp.read((char *)&(this->sos.unkown), 3);

            int cur = fp.tellg();
            fp.seekg(0, fp.end);
            int end = fp.tellg();
            fp.seekg(cur, fp.beg);

            /*获取到文件尾的大小*/
            this->code_buffer_size = end - cur - 2;
            this->code_buffer = (uint8_t *)malloc(this->code_buffer_size);
            fp.read((char *)(this->code_buffer), this->code_buffer_size);
        };

        /*DQT DB 定义量化表*/
        auto response_DB = [&fp, this, &header]()
        {
            DQT *dqt = &(this->dqt);

            fp.read((char *)&dqt->size, 2);
            dqt->size = Endian::hton(dqt->size);

            /*亮度的量化表*/
            fp.read((char *)&(dqt->qt_y.qt_info), 1);
            fp.read((char *)&(dqt->qt_y.qt), 64);

            /*色度的量化表*/
            fp.read((char *)&(dqt->qt_c.qt_info), 1);
            fp.read((char *)&(dqt->qt_c.qt), 64);
        };

        /*DRI DD 定义重新开始间隔*/
        auto response_DD = [&fp, this, &header]()
        {
            fp.read((char *)&(this->dri.size), 2);
            this->dri.size = Endian::hton(this->dri.size);

            fp.read((char *)&(this->dri.interval), 2);
            this->dri.interval = Endian::hton(this->dri.interval);

            std::cout << "dir:\t" << this->dri.interval << "\n";
        };

        /*APP0 E0 定义交换格式和图像识别信息*/
        auto response_E0 = [&fp, this, &header]()
        {
            // std::cout << "E0，app0\n";
            uint16_t size;
            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);
            // fp.seekg(size - 2, std::ios::cur);

            this->app0.size = size;
            fp.read((char *)&(this->app0.format), 5);
            fp.read((char *)&(this->app0.version_major), 1);
            fp.read((char *)&(this->app0.version_minor), 1);
            fp.read((char *)&(this->app0.density_unit), 1);
            fp.read((char *)&(this->app0.density_x), 2);
            this->app0.density_x = Endian::hton(this->app0.density_x);
            fp.read((char *)&(this->app0.density_y), 2);
            this->app0.density_y = Endian::hton(this->app0.density_y);

            fp.read((char *)&(this->app0.thumbnail_x), 1);
            fp.read((char *)&(this->app0.thumbnail_y), 1);

            this->app0.thumb = nullptr;
            if (this->app0.thumbnail_x > 0 && this->app0.thumbnail_y > 0)
            {
                uint32_t n = this->app0.thumbnail_x * this->app0.thumbnail_y;
                this->app0.thumb = (uint8_t *)malloc(3 * n);
                fp.read((char *)(this->app0.thumb), 3 * n);
            }
        };

        /*COM FE 注释*/
        auto response_FE = [&fp, this, &header]()
        {
            std::cout << "comments\n";
            uint16_t size;
            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);

            this->com.size = size;
            this->com.comments = (uint8_t *)malloc(size);
            fp.read((char *)(this->com.comments), size);
        };

        /*default*/
        auto response_default = [&fp, this, &header]()
        {
            uint16_t size;
            fp.read((char *)&size, sizeof(size));
            size = Endian::hton(size);
            fp.seekg(size - 2, std::ios::cur);

            printf("%x size=%x , %d unknown segment\n", header.type, size, size);
        };

        /*************************************************************/

        while (fp.read((char *)&header, sizeof(header)))
        {
            if (0xFF != header.id)
            {
                std::cerr << "unknown error!\n";
                return;
            }
            std::cout << std::hex << (uint32_t)header.type << std::endl;

            switch (header.type)
            {
            case 0xD8: /*文件头*/
                response_D8();
                break;
            case 0xD9: /*文件尾*/
                response_D9();
                break;
            case 0xE0: /*APP0，定义交换格式和图像识别信息*/
                response_E0();
                break;
            case 0xC0: /*帧开始（标准JPEG），基线离散余弦变换*/
                response_C0();
                break;
            case 0xC4: /*定义Huffman表*/
                response_C4();
                break;
            case 0xDA: /*扫描行开始*/
                response_DA();
                break;
            case 0xDB: /*定义量化表*/
                response_DB();
                break;
            case 0xDD:         /*重新定义开始间隔*/
                response_DD(); /*FFD0-FFD7*/
                break;
            case 0xFE: /*注释*/
                response_FE();
                break;
            default: /*default*/
                response_default();
                break;
            }
        }
        fp.close();
    };

    void jpeg_free()
    {
        if (nullptr != this->com.comments)
        {
            free(this->com.comments);
            this->com.comments = nullptr;
        }
        if (nullptr != this->app0.thumb)
        {
            free(this->app0.thumb);
            this->app0.thumb = nullptr;
        }
        if (nullptr != this->code_buffer)
        {
            free(this->code_buffer);
            this->code_buffer = nullptr;
        }
        if (nullptr != this->mat)
        {
            free(this->mat);
            this->mat = nullptr;
        }
    }

    /**
     * DCT:Discrete Sosine Transform，离散余弦变换
     * 属于傅里叶变换的一种，通常用于对信号和图像进行有损数据压缩。
     *
     * 一组离散的数据，如果数据是对称的，那么傅里叶变换后的函数只有余弦项，这种变换称为离散余弦变换。
     *
     * DCT是世界上应用最广泛的数据压缩转换技术，同时也是大多数数字媒体标准（图像、视频和音频）的基础。
     *
     * DCT将图像分成由不同频率组成的小块，然后进行量化。在量化过程中，
     * 舍弃高频分量，剩下的低频分量保存下来用于后面的图像重建。
     *
     * 步骤
     * step.1 将图像分成8*8的图像块
     * step.2 将RGB转换成YUV系统
     * step.3 从左到右，从上到下，对每个图像块做DCT变换，舍弃高频，保留低频
     * step.4　对余下的图像块进行量化压缩，由压缩后的数据所组成的图像大大缩减了存储空间
     * step.5 解压缩时对每个图像块做DCT反变换，然后重建一幅完整的图像。
     *
     * DCT具有去相关性和能量集中的特性。
     * DCT将矩阵的能量压缩到第一个元素中，称为直流系数（DC）。其余的系数称为交流系数（AC）
     * 这意味着输出的二维DCT的左上角是DC系数，是DCT最重要的输出，包含了很多关于原始图像的信息，
     * 其余系数称为交流系数（AC coefficients），包含了图像的更多细节。
     *
     * DCT本身不会压缩数据，它位随后的量化等操作，提供了一个良好的基础。
     */
    void DCT_transform() {}
};