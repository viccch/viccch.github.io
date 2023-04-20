# 高德瓦片地图获取与处理脚本

## Usage

使用方法：

1. 使用坐标拾取器获取研究区域的(gcj火星坐标系)坐标，左上（NW，西北）和右下角（SE，东南）两点。
2. 修改[get_tile.py](./scripts/get_tile.py)脚本尾部的点坐标，并指定要获取的瓦片等级，指定储存目录。建议先根据注释，计算瓦片数量。
3. 合成单幅高清大图，使用[merge_pic.py](./scripts/merge_pic.py)脚本的build_pic方法。

## 数学原理

$$
\begin{matrix}
&x=\left[\frac{lon+180}{360}{\cdot}2^z\right]
\\
&y=\left[ (1-\frac{\ln(\tan(lat{\cdot}\frac{\pi}{180})+\frac{1}{\cos(lat{\cdot}\frac{\pi}{180})})}{\pi}){\cdot}2^{z-1} \right]
\end{matrix}
$$

$$
\begin{matrix}
lon = \frac{x}{2^z}{\cdot}360-180
\\
lat = \arctan(\sinh(\pi-\frac{y}{2^z}{\cdot}2\pi))\cdot\frac{180}{\pi}
\end{matrix}
$$

## 相关文章

[通过经纬度计算不同模型图瓦片坐标](./theory.html)