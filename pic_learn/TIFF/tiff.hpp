#include <bits/stdc++.h>
struct TIFF
{
#pragma pack(push)
#pragma pakc(1)
    struct IFH; /*图像文件头*/
    struct IFD; /*图像文件目录*/
    struct DE;  /*目录项*/

    struct IFH
    {
        /*8 Bytes*/
        uint16_t byte_order;          /*TIF标记，4D4D或4949*/
        uint16_t version;             /*版本号，2A00*/
        uint32_t offset_to_first_IFD; /*第一个IFD的偏移量*/
    };

    struct IFD
    {
        uint16_t directroy_entry_count; /*本IFD中DE的数量*/
        DE *des = nullptr;              /*DE，目录项*/
        uint32_t offset_to_next_IFD;    /*下一个IFD的偏移量*/
    };

    struct DE
    {
        /*12 Bytes*/
        uint16_t tag;          /*本属性的标签编号*/
        uint16_t type;         /*本属性值的类型数据*/
        uint32_t count;        /*该类型数据的数量*/
        uint32_t value_offset; /*属性值的存放偏移量*/
    };
#pragma pack(pop)
};