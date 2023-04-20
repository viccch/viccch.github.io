#!/usr/bin/env python3


import cv2
import numpy
import os
import re
import sys


def build_pic(path_in: str, level: int, path_out: str = './'):

    def cmp_filename(file_name: str):
        pat = r'[z,x,y]'
        rst = re.split(pat, file_name.split(".")[0])
        z, x, y = (eval(rst[1]), eval(rst[2]), eval(rst[3]))
        return z, y, x  # 注意顺序

    # size = 2**level
    dir_list = os.listdir(path_in)
    dir_list.sort(key=cmp_filename)
    dz1, dy1, dx1 = cmp_filename(dir_list[0])
    dz2, dy2, dx2 = cmp_filename(dir_list[-1])
    # print(dz1, dy1, dx1)
    # print(dz2, dy2, dx2)
    # print(dy2-dy1, dx2-dx1)
    size_y, size_x = dy2-dy1+1, dx2-dx1+1
    # return
    mat_hs = []
    mat_vs = []

    t_cnt = 0
    for i in dir_list:
        # print(os.path.join(path, dir_list[i]))
        mat = cv2.imread(os.path.join(path_in, i))
        mat_hs.append(mat)

        if ((t_cnt+1) % size_x == 0):
            mat_vs.append(numpy.hstack(mat_hs))
            mat_hs.clear()

        t_cnt += 1
        pass

    photo = numpy.vstack(mat_vs)

    # cv2.imshow(str(level), photo)
    # cv2.waitKey(0)
    photo_name = str(level)+".jpeg"
    photo_name = os.path.join(path_out, photo_name)
    cv2.imwrite(photo_name, photo)
    print(photo_name+"保存成功")

    pass


if __name__ == '__main__':
    args = sys.argv[1:]

    print(args)

    while (len(args) < 3):
        args.append(str(0))

    if args[2] == '0':
        args[2] = './'

    build_pic(args[0], (int)(args[1]), args[2])
