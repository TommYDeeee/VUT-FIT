import matplotlib.pyplot as plt
import numpy as np
from download import DataDownloader


def plot_stat(data_source, fig_location=None, show_figure=False):
    regions, index, count = np.unique(
        [region for region in data_source[1][-1]], return_index=True, return_counts=True)
    dict_regions = {}
    for i, region in enumerate(regions):
        dict_regions[region] = [index[i], count[i]]
    test = {}
    for key in dict_regions.keys():
        from_i = dict_regions.get(key)[0]
        to_i = dict_regions.get(key)[0] + dict_regions.get(key)[1]
        years, counts = np.unique([year.split(
            "-")[0] for year in data_source[1][3][from_i:to_i]], return_counts=True)
        test[key] = dict(zip(years, counts))

    new = {}
    for k1, v1 in test.items():
        for k2, v2 in v1.items():
            if k2 not in new:
                new[k2] = {}
            new[k2][k1] = v2

    sorted_regions = {region: 0 for region in new.keys()}
    for item, values in new.items():
        sorted_regions[item] = {
            values[key]: rank for rank,
            key in enumerate(
                sorted(
                    values,
                    key=values.get,
                    reverse=True),
                1)}

    fig, axs = plt.subplots(len(new), figsize=(10, 15))

    for i, (key, value) in enumerate(new.items()):
        current_year = sorted_regions.get(key)
        axs[i].bar(list(value.keys()), list(value.values()))
        axs[i].set_title(key)
        test = axs[i].patches
        for t in test:
            axs[i].annotate(
                current_year.get(
                    t.get_height()),
                (t.get_x() +
                 t.get_width() /
                 2,
                 t.get_height()),
                ha='center',
                va='bottom')
    fig.tight_layout(pad=4)
    plt.show()


data_source = DataDownloader().get_list()
plot_stat(data_source)
