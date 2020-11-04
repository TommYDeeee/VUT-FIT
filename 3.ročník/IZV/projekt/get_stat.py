"""
    Author: Tomáš Ďuriš (xduris05)
"""

import matplotlib.pyplot as plt
import numpy as np
import os
import argparse

from matplotlib import ticker
from download import DataDownloader


def plot_stat(data_source, fig_location=None, show_figure=False):
    """
        Get data from DataDownloader class and visualize number of accidents
        for providen reagions for every year

        Args:
            data_source (tuple): Tuple with information about given regions
            fig_location (str): String with path/name of output file with graph
            show_figure (bool): Tells us if we want output shown on screen
    """

    # arrange data from tuple to dictionary
    # format them so we can easily use them to make a graph
    # firstly parse them accorind to last value (region name)
    # then parse them according to 3rd column (year in date)
    regions, index, count = np.unique(
        [region for region in data_source[1][-1]], return_index=True, return_counts=True)
    dict_regions = {}
    for i, region in enumerate(regions):
        dict_regions[region] = [index[i], count[i]]
    formated_dict = {}
    for key in dict_regions.keys():
        from_i = dict_regions.get(key)[0]
        to_i = dict_regions.get(key)[0] + dict_regions.get(key)[1]
        years, counts = np.unique([year.astype("datetime64[Y]") for year in data_source[1][3][from_i:to_i]], return_counts=True)
        formated_dict[key] = dict(zip(years, counts))

    # regroup dictionary to have year as a key
    ready_to_visualize_dict = {}
    for k1, v1 in formated_dict.items():
        for k2, v2 in v1.items():
            if k2 not in ready_to_visualize_dict:
                ready_to_visualize_dict[k2] = {}
            ready_to_visualize_dict[k2][k1] = v2

    # remove  invalid year from visualization
    ready_to_visualize_dict.pop(np.datetime64('1969'), None)

    # sort regions according to number of traffic incidents (for annotations)
    sorted_regions = {region: 0 for region in ready_to_visualize_dict.keys()}
    for item, values in ready_to_visualize_dict.items():
        sorted_regions[item] = {
            values[key]: rank for rank,
            key in enumerate(
                sorted(
                    values,
                    key=values.get,
                    reverse=True),
                1)}

    # make a graph in matplotlib with all necessary informations
    # and make it easy to orientate in
    fig, axs = plt.subplots(len(ready_to_visualize_dict), figsize=(8.27, 11.69))
    fig.patch.set_facecolor('tab:gray')
    fig.suptitle('Počty nehôd v jednotlivých krajoch Českej republiky', fontsize=16)
    max_value = max([max(value.keys()) for value in sorted_regions.values()])

    # process every subplot
    for i, (key, value) in enumerate(ready_to_visualize_dict.items()):
        current_year = sorted_regions.get(key)
        values = list(value.values())
        keys = list(value.keys())
        colors = ['cornflowerblue' if (
                x < max(values)) else 'royalblue' for x in values]
        axs[i].bar(keys, values, color=colors, zorder=3)
        axs[i].set_xbound(-1.0, len(DataDownloader.regions_ID))
        axs[i].set_title(key)
        axs[i].set_facecolor('darkgray')
        axs[i].spines['top'].set_visible(False)
        axs[i].spines['right'].set_visible(False)
        axs[i].spines['left'].set_visible(False)
        axs[i].spines['bottom'].set_zorder(3)
        axs[i].set_ylabel('počet nehôd', labelpad=10, fontsize=8.5, style='italic')
        axs[i].yaxis.grid(True, zorder=0, color='dimgrey')
        bars = axs[i].patches
        yticks = ticker.MaxNLocator(5)
        axs[i].set_ylim(ymax=max_value + max_value / 6.5)
        axs[i].tick_params(axis=u'both', which=u'both', length=0)
        axs[i].yaxis.set_major_locator(yticks)

        # process every bar in single subplot
        for b in bars:
            axs[i].annotate(
                current_year.get(
                    b.get_height()),
                (b.get_x() +
                 b.get_width() /
                 2,
                 b.get_height()),
                ha='center',
                va='bottom')
            axs[i].text(
                b.get_x() +
                b.get_width() /
                2,
                b.get_height()-(max_value/10),
                b.get_height(),
                size=6,
                ha='center',
                va='bottom', color='xkcd:dark')
    fig.subplots_adjust(hspace=0.5, top=0.91, bottom=0.05)

    # save graph if location was providen
    if fig_location is not None:
        os.makedirs(
            os.path.dirname(
                os.path.abspath(fig_location)),
            exist_ok=True)
        plt.savefig(fig_location, dpi=600)

    # show figure if it is expected
    if(show_figure):
        plt.show()


# script was called as a main (not as a module)
# parse arguments, print graph with accidents information
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="data visualization")
    parser.add_argument('--fig_location')
    parser.add_argument('--show_figure', action='store_true')
    args = parser.parse_args()

    data_source = DataDownloader().get_list()
    plot_stat(
        data_source,
        fig_location=args.fig_location,
        show_figure=args.show_figure)
