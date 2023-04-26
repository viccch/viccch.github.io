[PNG图像文件格式解析](https://blog.csdn.net/weixin_45715387/article/details/115181935)
[深入解析png图片解码技术](https://blog.csdn.net/whl0071/article/details/128331529?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-2-128331529-blog-115181935.235%5Ev32%5Epc_relevant_default_base3&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-2-128331529-blog-115181935.235%5Ev32%5Epc_relevant_default_base3&utm_relevant_index=3)

[zlib压缩算法](https://blog.csdn.net/rong_toa/article/details/108986430)

[libpng与zlib的配置与使用](https://blog.csdn.net/yshen_dublin/article/details/4942871)

# PNG

PNG是一种比较新的图片格式，是一种位图文件存储格式。PNG格式是无损数据压缩的，PNG格式有8位、24位、32位三种形式，其中8位PNG支持两种不同的透明形式（索引透明和alpha透明），24位PNG不支持透明，32位PNG在24位基础上增加了8位透明通道（32-24=8），因此可展现256级透明程度。

PNG-8是非常好的GIF格式替代者，在可能的情况下，应该尽可能的使用PNG-8而不是GIF，因为在相同的图片效果下，PNG-8具有更小的文件体积。PNG-8本身也是支持动画的，只是浏览器支持得不好，不像GIF那样受到广泛的支持。

