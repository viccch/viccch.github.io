# 字节序——大端和小端：

多字节对象被存储为连续的字节序列。

字节序两个通用规则：
- **大端序**（Big Endian）
  - 将数据的低位字节放在内存的高位地址。
  - 符合人类阅读习惯。

- **小端模式**：
  - 用低地址记录低位，是小端模式，
  - 与人类阅读习惯相反，但更适合计算机读取内存的方式，因为CPU读取内存中的数据时，时从低地址向高地址方向读取的。
  - 如0x123456，小端存储为0x563412


**字节序转换方法**

```cpp

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

```