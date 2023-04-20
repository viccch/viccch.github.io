#!/usr/bin/env python3

import requests
import cv2
import numpy
import asyncio
import aiohttp
import time
import math
import os
import sys


def get_specific_tile(level: int, save_path: str, from_x: int = 0, from_y: int = 0, to_x: int = 0, to_y: int = 0):

    start = time.time()

    # 高德地图的瓦片服务器

    url = 'https://webst01.is.autonavi.com/appmaptile'

    params = {
        'style': 6,
        'x': 0,
        'y': 0,
        'z': 1
    }

    params['z'] = level

    xmax = ymax = 2**level

    if (to_x == 0 and to_y == 0):
        to_x = from_x+1
        to_y = from_y+1
    else:
        if (to_x == from_x):
            to_x = from_x + 1
        if (to_y == from_y):
            to_y = from_y+1

    cnt_img = 0

    for i in range(from_x, to_x):
        for j in range(from_y, to_y):
            params['x'] = i
            params['y'] = j

            re = requests.get(url, params)
            file_name = 'z'+(str)(params['z']) + 'x'+(str)(params['x'])+'y'+(str)(params['y'])+'.jpeg'
            fp = open(os.path.join(save_path, file_name), 'wb')
            fp.write(re.content)
            cnt_img += 1
            print("第"+(str)(cnt_img)+"块"+fp.name+"下载成功")

    end = time.time()
    print("共获取{}块tile，用时：{:.2f} seconds".format(cnt_img, end-start))


if __name__ == '__main__':
    # please make sure dir exits
    args = sys.argv[1:]

    print(args)

    while (len(args) < 6):
        args.append(str(0))

    print("参数处理后：")
    print(args)

    get_specific_tile(args[0], args[1], (int)(args[2]), (int)(args[3]), (int)(args[4]), (int)(args[5]))
