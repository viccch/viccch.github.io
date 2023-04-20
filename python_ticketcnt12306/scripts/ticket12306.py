import requests
import json
import re
import datetime

"""

服务名称：
12306同车换乘查询

需求分析：
长距离出行，购买火车票时，往往难以买到全程车票，
小A发现可以通过将乘车区间成多个小段，分别购买这些小段上的车票，通过同内换乘，以实现坐满整个区间的目的
编写脚本程序，查询给定日期、出发地、目的地、车次，能否同车换乘，若能，返回中间需要换乘的站点。

输入：
日期，出发地，目的地，车次

输出：
查询指定日期指定车次余票信息，
以及以沿途各个站点为中转的通车换乘余票数量

"""


class StationName:

    station = dict()

    def __init__(self):

        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36',
            'Cookie': 'JSESSIONID=85ABACBDDEC5EF0D3F4390E49C235DCD; BIGipServerotn=569377290.24610.0000; BIGipServerpool_passport=183304714.50215.0000; RAIL_EXPIRATION=1584746738988; RAIL_DEVICEID=c_HPEh5qqB0-onW7FlqB5a2T-w9tiHZ95ePILEBaXLQ3Nj84j7a4PV1ezmRs7O57oEVHFp3JcbAEi_s3qJb_bqey5sGYiQ-RmKrzrZ0wzbndDLKGidKjF1l5UZ4FjwqSTdhbaSx8ds-5RgV-KxQrm0mINenavAb3; route=c5c62a339e7744272a54643b3be5bf64; _jc_save_fromStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2020-03-17; _jc_save_toDate=2020-03-17; _jc_save_wfdc_flag=dc; _jc_save_toStation=%u5929%u6D25%2CTJP'
        }

        url = 'https://kyfw.12306.cn/otn/resources/js/framework/station_name.js?station_version=1.9028'

        response = requests.get(url=url, headers=headers)

        station = re.findall(r'([\u4e00-\u9fa5]+)\|([A-Z]+)', response.text)
        station = dict(station)
        self.station = station
        pass
    pass


class TrainTimeTable:

    def get(train: str, dt: datetime.date) -> json:

        date_without_dash = dt.strftime("%Y%m%d")
        date_with_dash = dt.strftime("%Y-%m-%d")

        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36',
            'Cookie': 'JSESSIONID=85ABACBDDEC5EF0D3F4390E49C235DCD; BIGipServerotn=569377290.24610.0000; BIGipServerpool_passport=183304714.50215.0000; RAIL_EXPIRATION=1584746738988; RAIL_DEVICEID=c_HPEh5qqB0-onW7FlqB5a2T-w9tiHZ95ePILEBaXLQ3Nj84j7a4PV1ezmRs7O57oEVHFp3JcbAEi_s3qJb_bqey5sGYiQ-RmKrzrZ0wzbndDLKGidKjF1l5UZ4FjwqSTdhbaSx8ds-5RgV-KxQrm0mINenavAb3; route=c5c62a339e7744272a54643b3be5bf64; _jc_save_fromStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2020-03-17; _jc_save_toDate=2020-03-17; _jc_save_wfdc_flag=dc; _jc_save_toStation=%u5929%u6D25%2CTJP'
        }

        url_no = "https://search.12306.cn/search/v1/train/search?keyword="+train+"&date="+date_without_dash

        train_js = requests.get(url=url_no, headers=headers).json()

        train_no = train_js['data'][0]['train_no']

        url = 'https://kyfw.12306.cn/otn/queryTrainInfo/query'

        params = {
            'leftTicketDTO.train_no': train_no,
            'leftTicketDTO.train_date': date_with_dash,
            'rand_code': ''
        }

        js = requests.get(url=url, params=params, headers=headers).json()

        return js

        pass

    pass


class Query:

    def __init__(self, date_s: datetime.date, from_s: str, to_s: str, train_s: str, stn: StationName) -> None:
        self.date_s = date_s
        self.from_s = from_s
        self.to_s = to_s
        self.train_s = train_s
        self.stn = stn
        self.from_n = self.stn.station[from_s]
        self.to_n = self.stn.station[to_s]

        pass

    def G(self, js: json):

        data = js['data']['data']

        start_n = 0
        stop_n = 0

        for i in data:
            if i['station_name'] == self.from_s:
                start_n = eval((str)(i['station_no']).lstrip('0'))-1
            if i['station_name'] == self.to_s:
                stop_n = eval((str)(i['station_no']).lstrip('0'))-1

        print(self.train_s, self.from_s, "->", self.to_s, "中转查询:")

        for i in range(start_n+1, stop_n):
            # print(data[i])

            mid_station = (str)(data[i]['station_name'])

            days_diff = datetime.timedelta(days=eval((str)(data[i]['arrive_day_diff'])))

            transfer_time = (str)(data[i]['arrive_time'])

            cls1 = Query(self.date_s, self.from_s, mid_station, self.train_s, self.stn)
            cls2 = Query(self.date_s+days_diff, mid_station, self.to_s, self.train_s, self.stn)

            tkt1 = TicketCNT()
            tkt2 = TicketCNT()

            tkt1.fetch_info(cls1)
            tkt2.fetch_info(cls2)

            print("以", mid_station, "为中转站")

            print(self.date_s, self.train_s, self.from_s, "->", mid_station, "余票：\t", tkt1)
            print(self.date_s+days_diff, transfer_time, "到达", mid_station)
            print(self.date_s+days_diff, self.train_s,  mid_station, "->", self.to_s, "余票：\t", tkt1)
            print("\n")

            # tkt = TicketCNT.
        pass

    pass


class TicketCNT:

    def __init__(self):
        self.zt = 0  # 商务座、特等座
        self.z1 = 0  # 一等座
        self.z2 = 0  # 二等座
        self.rwt = 0  # 高级软卧
        self.rw = 0  # 软卧、一等卧
        self.dw = 0  # 动卧
        self.yw = 0  # 硬卧、二等卧
        self.rz = 0  # 软座
        self.yz = 0  # 硬座
        self.w = 0  # 无座
        self.o = 0  # 其他
        pass

    def set(self, list: list[str]):

        def fix_str(str: str) -> int:
            if('无' == str):
                return 0
            elif ('有' == str):
                return 999
            elif ('' == str):
                return 0
            else:
                return eval(str)
            pass

        self.zt = fix_str(list[32])
        self.z1 = fix_str(list[31])
        self.z2 = fix_str(list[30])

        self.rw = fix_str(list[23])
        self.rwt = fix_str(list[21])
        self.w = fix_str(list[26])
        self.yw = fix_str(list[28])
        self.yz = fix_str(list[29])
        # print(list)
        # print(list[29])
        pass

    def __str__(self) -> str:
        s = ""
        s += "硬座:"+(str)(self.yz)+"\t"
        s += "硬卧:"+(str)(self.yw)+"\t"
        s += "二等座:"+(str)(self.z2)+"\t"

        return s
        pass

    def fetch_info(self, cls: Query):
        date_s = cls.date_s
        from_n = cls.from_n
        to_n = cls.to_n
        train_s = cls.train_s
        url = "https://kyfw.12306.cn/otn/leftTicket/queryZ"
        params = {"leftTicketDTO.train_date":  date_s,
                  "leftTicketDTO.from_station": from_n,
                  "leftTicketDTO.to_station": to_n,
                  "purpose_codes": "ADULT"}
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36',
                   'Cookie': 'JSESSIONID=85ABACBDDEC5EF0D3F4390E49C235DCD; BIGipServerotn=569377290.24610.0000; BIGipServerpool_passport=183304714.50215.0000; RAIL_EXPIRATION=1584746738988; RAIL_DEVICEID=c_HPEh5qqB0-onW7FlqB5a2T-w9tiHZ95ePILEBaXLQ3Nj84j7a4PV1ezmRs7O57oEVHFp3JcbAEi_s3qJb_bqey5sGYiQ-RmKrzrZ0wzbndDLKGidKjF1l5UZ4FjwqSTdhbaSx8ds-5RgV-KxQrm0mINenavAb3; route=c5c62a339e7744272a54643b3be5bf64; _jc_save_fromStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2020-03-17; _jc_save_toDate=2020-03-17; _jc_save_wfdc_flag=dc; _jc_save_toStation=%u5929%u6D25%2CTJP'}
        rq = requests.get(url=url, params=params, headers=headers)

        js = rq.json()
        result = js['data']['result']
        for i in result:
            str_list = i.split('|')
            for j in str_list:
                if(train_s == j):
                    self.set(str_list)
                    return
        pass

    pass


def main(date_s: str, from_s: str, to_s: str, train_s: str):

    stn = StationName()
    cls = Query(date_s, from_s, to_s, train_s, stn)
    # tktcnt = cls.fetch_info()

    tktcnt = TicketCNT()
    tktcnt.fetch_info(cls)

    # print(tktcnt)
    print(cls.train_s, cls.date_s, cls.from_s, "->", cls.to_s, "余票：\t", tktcnt, "\n")

    js = TrainTimeTable.get(train_s, date_s)

    cls.G(js)

    pass


if __name__ == "__main__":
    date_s = datetime.date(2023, 4, 25)
    main(date_s, "上海", "西宁", "Z376")
