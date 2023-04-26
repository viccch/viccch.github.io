# TIFF
TIFF（Tagged Image File Format）格式是一种常用的图像文件格式，它支持无损压缩，可以保存高质量的图像数据，后缀名为".tif"。它最初由Aldus公司与微软公司一起为PostScript打印开发。TIFF与JPEG和PNG一起成为流行的高位彩色图像格式。TIF格式的图像处理技术通常包括以下:

- 解码（Decoding）解码是将经过编码的数字信号进行解析还原成原始数据的过程。在TIF格式中，解码通常是指将已经压缩过的图像数据进行解压缩，还原成原始像素数据的过程。TIF格式支持多种压缩算法，包括无压缩、LZW压缩和JPEG压缩等。不同的压缩算法对解码的要求也不同，需要根据具体的压缩算法来进行解码操作。

- 编码（Encoding）编码是将原始的数字信号（如图像、音频等）转换为压缩格式的过程。在TIF格式中，编码通常是指将原始像素数据进行压缩和编码，生成TIF格式的文件。TIF格式支持多种压缩算法，需要根据具体的需求选择合适的压缩算法和参数来进行编码操作。

- 压缩（Compression）压缩是指将原始的数字信号进行压缩，以减小数据量的过程。在TIF格式中，压缩通常是指对图像数据进行压缩，以减小TIF格式文件的大小。TIF格式支持多种压缩算法，包括无压缩、LZW压缩和JPEG压缩等。不同的压缩算法对图像质量、文件大小和解码速度都有影响，需要根据具体需求进行选择。


TIF格式：
- 优点：TIF格式支持多种压缩算法，包括无损压缩和有损压缩，可以满足不同的需求。TIF格式支持多种颜色空间和深度，包括RGB、CMYK、灰度等，适合不同的应用场景。TIF格式支持多帧图像，可以保存多页文档或动画图像等。
- 缺点：TIF格式文件通常比较大，不适合在网络上传输或储存大量的图片。

TIFF的TAG

| 十六进制 | TAG 名称 | 描述 |
| -------- | -------- | ---- |
| 00FE     | NewSubfile Type | 新的子文件类型标识 LONG 长度为1,用比特来标识图像的类型,Bit0如果是1代表缩略图,Bit1如果是1代表多页图像中的某一页,Bit2如果是1代表它是透明度掩码图像,其余的位数暂时没有定义。与SubfileType的是，此Tag用比特位来区分文件类型而不是用值来区分 |
| 00FF     | Subfile Type | 子文件类型标识 SHORT 长度为1,1 全分辨率图像,2 缩小分辨率的图像,3 多页图像的某一页,过时的Tag，已不再使用 |
| 0100     | ImageWidth | 图像宽度 |
| 0101     | ImageLength | 图像高度                                                     |
| 0102     | BitsPerSample | 每个分量的bit数 |
| 0103     | Compression | 压缩类型 SHORT 长度为1,随着TIFF的不断扩张，目前支持多达几十种的压缩方式,Compression＝1： 没有压缩,Compression＝7：JPEG压缩。 如果是RGB图像并且SamplesPerPixel＝3，则是标准的有损JPEG压缩。如果是CFA图像，则是Lossless JPEG |
| 0106     | PhotometricInterpretation | 颜色空间，SHORT，0：WhitelsZero，0对应最亮灰度；1：BlacklsZero,0对应最暗灰度；2：RGB图像，索引顺序RGB；3：Palette索引图像，ColorMap必须定义。SamplesPerPixel必须为１；32803＝CFA（Color Filter Array） |
| 0107     | Threshholding | 定义了转换成二值图像的阈值 |
| 0108     | CellWidth |      |
| 0109     | CellLength |      |
| 010A     | FillOrder | 在同一字节的逻辑顺序 |
| 010E     | ImageDescription | 字符串，对图像的描述 |
| 010F     | Make | 字符串，生产厂商 |
| 0110     | Model | 字符串 |
| 0111     | StripOffsets |      |
| 0112     | Orientation |      |
| 0115     | SamplesPerPixel | 每个像素的通道数4byte |
| 0116     | RowsPerStrip | 每个strip有多少行4byte |
| 0117     | StripByteCounts | 每个strip的长度 |
| 0118     | MinSampleValue |      |
| 0119     | XResolution |      |
| 011A     | YResolution |      |
| 011B     | PlanarConfiguration |      |
| 011C     | FreeOffsets |      |
| 0120     | FreeByteCounts |      |
| 0121     | GrayResponseUint |      |
| 0122     | GrayResponseCurve |      |
| 0123     | ResolutionUint |      |
| 0128     | Software |      |
| 0131     | DataTime | Data and time of image creation |
| 0132     | Artist | Person who created the image |
| 013B     | HostComputer |  |
| 0140     | ColorMap | 调色板 |
| 0152     | Extrasamples |      |
| 8298     |   CopyRight       |      |



