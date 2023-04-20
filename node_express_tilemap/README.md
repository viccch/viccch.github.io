# 使用node.js和Express框架的WMTS瓦片地图后台服务

# Introduction

使用Express作为服务框架，处理地图瓦片GET请求。

使用TypeScript脚本语言，具有强类型的特性，利于发现逻辑问题。

使用预处理切分的高清卫星影像图生成地图瓦片

自适应处理不同等级的地图瓦片

测试使用的瓦片请数据自行获取

## Usage

1. 将瓦片按照Zoom等级放置在[tilemap](/tilemap/)内的相应文件夹内
2. 确保已安装[node和npm](https://nodejs.org/)，执行`npm install`命令，安装所需依赖
3. 执行`npm run build`命令，编译ts生成js
4. 执行`npm run dev`命令，程序将运行在3000端口上

