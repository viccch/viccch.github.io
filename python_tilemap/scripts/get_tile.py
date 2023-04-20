import math
import os
import sys
import cv2
import requests
from fetch_tiles_with_multi_thread import get_specific_tile
# from fetch_tiles import get_specific_tile
# from scripts.fetch_tiles import get_specific_tile
from merge_pic import build_pic

"""
已知低zoom、高zoom和低zoom对应的x，
求高zoom对应的x
"""


def low_to_high(low_z: int, high_z: int, low_x: int) -> int:
    return low_x*(2**(high_z-low_z))
    pass


def high_to_low(low_z: int, high_z: int, high_x: int) -> int:
    return (int)(high_x/(2**(high_z-low_z)))
    pass


def lnglat_to_tile(longitude: float, latitude: float, level_z: int):
    """
    经度：Longitude，东经+，西经-；\n
    纬度：Latitude， 北纬+，南纬-；\n
    x从左到右对应 -180 -> 180 (从西到东)；\n
    y从左到右对应   90 -> -90 (从北到南)；\n
    """
    x = (longitude-(- 180.0))/(360)*(2 ** level_z)

    # y = (180-(latitude+90))/(180)*(2 ** level_z)

    y = (1-math.log(math.tan(latitude*math.pi/180)+1/math.cos(latitude*math.pi/180))/math.pi)*(2 ** (level_z-1))

    return level_z, (int)(x), (int)(y)
    pass


class Point:
    def __init__(self, lng: float = 0.0, lat: float = 0.0) -> None:
        self.longitude = lng
        self.latitude = lat
        pass
    pass


def get_tile(save_path: str, point_NW: Point, point_SE: Point, zoom: int):
    point_SW = Point(point_NW.longitude, point_SE.latitude)
    point_NE = Point(point_SE.longitude, point_NW.latitude)

    z, from_x, to_y = lnglat_to_tile(point_SW.longitude, point_SW.latitude, zoom)
    z, to_x, from_y = lnglat_to_tile(point_NE.longitude, point_NE.latitude, zoom)

    if (to_x == 0 and to_y == 0):
        to_x = from_x+1
        to_y = from_y+1
    else:
        if (to_x == from_x):
            to_x = from_x + 1
        elif (to_y == from_y):
            to_y = from_y+1

    print('预计瓦片数量约：'+(str)((to_x-from_x)*(to_y-from_y)))
    # return
    get_specific_tile(zoom, save_path,   from_x, from_y, to_x, to_y)
    pass


# 紫禁城
p_NW = Point(116.390682, 39.923343)
p_SE = Point(116.403303, 39.912535)

# 北京二环
# p_NW = Point(116.335159,39.952988)
# p_SE = Point(116.45867,39.866017)

z = 14
get_tile('./z14', p_NW, p_SE, z)
build_pic('./z14', z)
