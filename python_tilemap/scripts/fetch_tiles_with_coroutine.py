#!/usr/bin/env python3

import requests
import cv2
import numpy
import asyncio
import aiohttp
import time
import sys


@DeprecationWarning
async def get_specific_tile(level: int, save_path: str, from_x: int = 0, from_y: int = 0, to_x: int = 0, to_y: int = 0):

    url = 'https://webst01.is.autonavi.com/appmaptile'

    params = {
        'style': 6,
        'z': 1,  # 瓦片等级
        'x': 210,
        'y': 105
    }

    async def fetch(url: str, params: dict, x: int, y: int):
        # params['z'] = z
        params['x'] = x
        params['y'] = y
        re = requests.get(url, params)
        file_name = 'z'+(str)(params['z']) + 'x'+(str)(params['x'])+'y'+(str)(params['y'])+'.jpeg'
        fp = open(file_name, 'wb')
        fp.write(re.content)
        print(fp.name+"下载成功")
        pass

    tasks = []

    """添加协程任务"""
    for i in range(0, 10):
        for j in range(0, 10):
            paras = params.copy()
            paras['x'] = i
            paras['y'] = i
            tasks.append(fetch(url, paras, i, j))

    await asyncio.gather(*tasks)


if __name__ == '__main__':
    # please make sure dir exits
    args = sys.argv[1:]
    print(args)
    while (len(args) < 6):
        args.append(str(0))

    print("参数处理后：")
    print(args)

    loop = asyncio.get_event_loop()
    loop.run_until_complete(get_specific_tile(args[0], args[1], (int)(args[2]), (int)(args[3]), (int)(args[4]), (int)(args[5])))
