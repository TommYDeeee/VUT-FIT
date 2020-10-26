import requests
import os
import re
import numpy as np
import zipfile
import csv

from io import TextIOWrapper
from bs4 import BeautifulSoup


class DataDownloader:
    string_list = [
        "p1",
        "p36",
        "p37",
        "p2a",
        "weekday(p2a)",
        "p2b",
        "p6",
        "p7",
        "p8",
        "p9",
        "p10",
        "p11",
        "p12",
        "p13a",
        "p13b",
        "p13c",
        "p14",
        "p15",
        "p16",
        "p17",
        "p18",
        "p19",
        "p20",
        "p21",
        "p22",
        "p23",
        "p24",
        "p27",
        "p28",
        "p34",
        "p35",
        "p39",
        "p44",
        "p45a",
        "p47",
        "p48a",
        "p49",
        "p50a",
        "p50b",
        "p51",
        "p52",
        "p53",
        "p55a",
        "p57",
        "p58",
        "a",
        "b",
        "d",
        "e",
        "f",
        "g",
        "h",
        "i",
        "j",
        "k",
        "l",
        "n",
        "o",
        "p",
        "q",
        "r",
        "s",
        "t",
        "p2a"]
    all_regions_list = [
        "PHA",
        "STC",
        "JHC",
        "PLK",
        "KVK",
        "ULK",
        "LBK",
        "HKK",
        "PAK",
        "OLK",
        "MSK",
        "JHM",
        "ZLK",
        "VYS"]

    def __init__(
            self,
            url="https://ehw.fit.vutbr.cz/izv/",
            folder="data",
            cache_filename="data_{}.pkl.gz"):
        self.url = url
        self.folder = folder

    def download_data(self):
        headers = {
            'user-agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/ \
                537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36'}

        r = requests.get(self.url, headers=headers)
        soup = BeautifulSoup(r.text, 'html.parser')
        all_hrefs = soup.find_all('a')
        all_links = [link.get('href') for link in all_hrefs]
        zip_files = [dl for dl in all_links if dl and '.zip' in dl]
        if not os.path.exists(self.folder):
            os.makedirs(self.folder)

        for zip_file in zip_files:
            if os.path.isfile(self.folder + "/" + zip_file.split("/")[1]):
                pass
            else:
                r = requests.get(self.url + zip_file, headers=headers)
                zip_filename = os.path.basename(zip_file)
                dl_path = os.path.join(self.folder, zip_filename)

                with open(dl_path, 'wb') as z_file:
                    z_file.write(r.content)

    def parse_region_data(self, region):
        self.download_data()
        regions_ID = {
            'PHA': '00.csv',
            'STC': '01.csv',
            'JHC': '02.csv',
            'PLK': '03.csv',
            'KVK': '19.csv',
            'ULK': '04.csv',
            'LBK': '18.csv',
            'HKK': '05.csv',
            'PAK': '17.csv',
            'OLK': '14.csv',
            'MSK': '07.csv',
            'JHM': '06.csv',
            'ZLK': '15.csv',
            'VYS': '16.csv'}
        region_file = regions_ID.get(region)
        latest_month_for_year = {}
        data_types = [
            int,
            int,
            int,
            'U100',
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            int,
            float,
            float,
            float,
            float,
            'U25',
            'U25',
            'U25',
            'U25',
            'U25',
            int,
            'U25',
            'U25',
            'U25',
            int,
            int,
            'U25',
            int]
        data_list = [[] for _ in range(len(self.string_list))]
        strings = [string for string in self.string_list]
        region_data = (strings, data_list)
        for file in os.listdir(self.folder):
            file_regex = re.search(r".*?(\d{2})?-?(\d{4})\.zip", file)
            month = file_regex.group(1)
            year = file_regex.group(2)
            if month is None:
                month = '12'
            if year in latest_month_for_year:
                if latest_month_for_year[year][0] < int(month):
                    latest_month_for_year[year] = [int(month), file]
            else:
                latest_month_for_year[year] = [int(month), file]

        for zip_file in latest_month_for_year.values():
            with zipfile.ZipFile(self.folder + '/' + zip_file[1]) as current_zip:
                with current_zip.open(region_file, 'r') as opened_csv:
                    reader = csv.reader(
                        TextIOWrapper(
                            opened_csv,
                            'iso8859-2'),
                        delimiter=';',
                        quotechar='"')
                    all_rows = list(reader)
                    for row in all_rows:
                        for i, value in enumerate(row):
                            if data_types[i] == float:
                                try:
                                    region_data[1][i].append(
                                        float(value.replace(',', '.')))
                                except ValueError:
                                    region_data[1][i].append(np.nan)
                            elif data_types[i] == int:
                                try:
                                    region_data[1][i].append(int(value))
                                except ValueError:
                                    region_data[1][i].append(-999999)
                            else:
                                region_data[1][i].append(value)

        for i, region_list in enumerate(region_data[1]):
            region_data[1][i] = np.array(region_list, dtype=data_types[i])

        region_ID = np.full(region_data[1][0].shape, region)
        region_data[1].append(region_ID)
        region_data[0].append("region")
        return region_data

    def get_list(self, regions=None):
        data_list = [[] for _ in range(len(self.string_list))]
        strings = [string for string in self.string_list]
        region_list_data = (strings, data_list)
        region_list_data[1].append(np.array([]))
        region_list_data[0].append("region")
        array = []
        if regions is None:
            for region in self.all_regions_list:
                array.append(self.parse_region_data(region))

        else:
            for region in regions:
                array.append(self.parse_region_data(region))

        for i in range(len(region_list_data[0])):
            region_list_data[1][i] = np.concatenate(
                ([test[1][i] for test in array]))
            print(region_list_data[1][i])
        print(type(region_list_data[1][0][0]))
        return region_list_data


d = DataDownloader()
d.get_list(["PLK"])
