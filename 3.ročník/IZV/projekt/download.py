"""
    Author: Tomáš Ďuriš (xduris05)
"""

import requests
import os
import re
import numpy as np
import zipfile
import csv
import pickle
import gzip

from io import TextIOWrapper
from bs4 import BeautifulSoup


def get_data(obj, regions):
    """
        Get all the necessary data for providen regions if they are not already
        stored in class attribute (data_dict{}). If cache file exists, load its
        content into class attribute otherwise firstly store data into pickle
        and then load into class attribute
    """
    regions = [region for region in regions if region not in obj.data_dict.keys()]
    for region in regions:
        path = os.path.join(obj.folder, obj.cache_filename.format(region))
        if not os.path.isfile(path):
            with gzip.open(path, 'wb') as f:
                pickle.dump(obj.parse_region_data(region), f)

        with gzip.open(path, 'rb') as f:
            obj.data_dict[region] = pickle.load(f)


class DataDownloader:
    """
        Simple class that downloads data with traffic accidents from
        providen web server and save parsed and cleaned information
        into tuple.
    """

    # Every region with file name
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

    def __init__(
            self,
            url="https://ehw.fit.vutbr.cz/izv/",
            folder="data",
            cache_filename="data_{}.pkl.gz"
            ):
        """
            Initialize class  with attributes with providen information

            Args:
                url (str): Url address with web server to scrape.
                folder (str): Name of a folder to store data in.
                cache_filename (str): Name of a file to store cache in.
        """

        self.url = url
        self.folder = folder
        self.cache_filename = cache_filename

        # dictionary with data from pickle grouped together
        self.data_dict = {}

        # list of pre-defined column names in CSV
        self.string_list = [
            "p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b",
            "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13a",
            "p13b", "p13c", "p14", "p15", "p16", "p17", "p18",
            "p19", "p20", "p21", "p22", "p23", "p24", "p27",
            "p28", "p34", "p35", "p39", "p44", "p45a", "p47",
            "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53",
            "p55a", "p57", "p58", "a", "b", "d", "e", "f", "g", "h",
            "i", "j", "k", "l", "n", "o", "p", "q", "r", "s", "t", "p5a"]

        # list of appropriate data types for each column in CSV
        self.data_types = [
            int, int, int, 'U15', int, int, int, int, int, int,
            int, int, int, int, int, int, int, int, int, int, int,
            int, int, int, int, int, int, int, int, int, int, int,
            int, int, int, int, int, int, int, int, int, int, int,
            int, int, int, int, float, float, float, float,
            'U25', 'U25', 'U25', 'U25', 'U25', int,
            'U25', 'U25', 'U25', int, int, 'U25', int]

    def download_data(self):
        """
            Download every zip with accident information from given URL
            and store them into given folder
        """

        # Request header to avoid robot detection
        headers = {
            'user-agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/ \
                537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36'}

        # store all links with zips on web
        r = requests.get(self.url, headers=headers)
        soup = BeautifulSoup(r.text, 'html.parser')
        all_hrefs = soup.find_all('a')
        all_links = [link.get('href') for link in all_hrefs]
        zip_files = [dl for dl in all_links if dl and '.zip' in dl]

        if not os.path.exists(self.folder):
            os.makedirs(self.folder)

        # download each zip on web if it is not already downloaded
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
        """
            Get all necessary data and parse them according to providen
            region code into tuple with column names and cloumn values
            each in numpy array

            Args:
                region (str): Region code that we want data about

            Return:
                tuple: Tuple with cleaned and parsed information about region
        """

        self.download_data()

        # Filter unnecessary zip files, work just with latest month in year
        region_file = self.regions_ID.get(region)
        latest_month_for_year = {}
        data_list = [[] for _ in range(len(self.string_list))]
        strings = [string for string in self.string_list]
        region_data = (strings, data_list)
        for file in os.listdir(self.folder):
            if file.endswith(".zip"):
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

        # parse and clean data about region just from latest month in every
        # year and store each row in CSV file for given region into numpy list
        for zip_file in latest_month_for_year.values():
            with zipfile.ZipFile(self.folder + '/' + zip_file[1]) as current_zip:
                with current_zip.open(region_file, 'r') as opened_csv:
                    reader = csv.reader(
                        TextIOWrapper(
                            opened_csv,
                            'iso8859-2'),
                        delimiter=';',
                        quotechar='"')
                    for row in reader:
                        for i, value in enumerate(row):
                            if self.data_types[i] == float:
                                try:
                                    region_data[1][i].append(
                                        float(value.replace(',', '.')))
                                except ValueError:
                                    region_data[1][i].append(np.nan)
                            elif self.data_types[i] == int:
                                if i == 5:
                                    if(value == "2560"):
                                        region_data[1][i].append(-999999)
                                    else:
                                        try:
                                            region_data[1][i].append(int(value))
                                        except ValueError:
                                            region_data[1][i].append(-999999)
                                else:
                                    try:
                                        region_data[1][i].append(int(value))
                                    except ValueError:
                                        region_data[1][i].append(-999999)
                            else:
                                region_data[1][i].append(value)

        # change list with column information into numpy array
        for i, region_list in enumerate(region_data[1]):
            region_data[1][i] = np.array(region_list, dtype=self.data_types[i])

        # append last column with region code
        region_ID = np.full(region_data[1][0].shape, region)
        region_data[1].append(region_ID)
        region_data[0].append("region")
        return region_data

    def get_list(self, regions=None):
        """
            Get data for list of regions grouped together in one tuple.
            Data are either taken from class attribute, pickle or even parsed
            from zip files

            Args:
                regions (list): List of regions that we want data about
           
            Return:
                tuple: Tuple with concatenated information about given regions
        """

        # precreate tuple with constant information
        data_list = [[] for _ in range(len(self.string_list))]
        strings = [string for string in self.string_list]
        region_list_data = (strings, data_list)
        region_list_data[1].append(np.array([]))
        region_list_data[0].append("region")

        # If zero regions were providen, process all regions
        if regions is None:
            regions = [region for region in self.regions_ID.keys()]

        get_data(self, regions)

        # concatenate all data about given regions
        for i in range(len(region_list_data[0])):
            region_list_data[1][i] = np.concatenate(
                ([test[1][i] for test in self.data_dict.values()]))

        return region_list_data


# script was called as a main (not as a module)
# print basic information about chosen regions (JHC, PLK, PAK)
if __name__ == "__main__":
    obj = DataDownloader()
    output = obj.get_list(["JHC", "PLK", "PAK"])
    print(f'STLPCE: {output[0]}')
    print(f'POCET ZAZNAMOV: {len(output[1][0])}')
    print(f'KRAJE: {set(output[1][-1])}')
