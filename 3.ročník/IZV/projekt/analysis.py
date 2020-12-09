#!/usr/bin/env python3.8
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import os
# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

# Ukol 1: nacteni dat
def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    df = pd.read_pickle(filename)
    old_size = round(sum(df.memory_usage(deep=True).values)/1048576,1)
    dates = [date for date in df['p2a']]
    as_categorical = [
        'p36', 'p37', 'weekday(p2a)', 'p6', 'p7', 'p8', 'p9', 'p10',
        'p11', 'p12', 'p14', 'p15', 'p16',
        'p17', 'p18', 'p19', 'p20', 'p21', 'p22', 'p23', 'p24', 'p27',
        'p28', 'p34', 'p35', 'p39', 'p44', 'p45a', 'p47', 'p48a', 'p49',
        'p50a', 'p50b', 'p51', 'p52', 'p53', 'p55a', 'p57', 'p58', 'h',
        'i', 'j', 'k', 'l', 'n', 'o', 'p', 'q', 'r', 's', 't', 'p5a'
    ]
    df['date'] = pd.to_datetime(dates)
    df[as_categorical] = df[as_categorical].astype('category')
    new_size = round(sum(df.memory_usage(deep=True).values)/1048576,1)
    if verbose:
        print(f'orig_size={old_size} MB')
        print(f'new_size={new_size} MB')

    return df

# Ukol 2: následky nehod v jednotlivých regionech
def plot_conseq(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    print(df[['region', 'p13a']])
    df_deaths_by_region = df.groupby(['region']).agg({"p13a": "sum"}).reset_index().sort_values('p13a', ascending=False)
    df_heavy_injured_by_region = df.groupby(['region']).agg({"p13b": "sum"}).reset_index().sort_values('p13b', ascending=False)
    df_light_injured_by_region = df.groupby(['region']).agg({"p13c": "sum"}).reset_index().sort_values('p13c', ascending=False)
    df_accidents_count_by_region = df.groupby(['region']).agg({"p1": "count"}).reset_index().sort_values('p1', ascending=False)
    fig, ax = plt.subplots(4, figsize=(8.27, 11.69))
    sns.barplot(ax=ax[0], data=df_deaths_by_region, x="region", y="p13a")
    sns.barplot(ax=ax[1], data=df_heavy_injured_by_region, x="region", y="p13b")
    sns.barplot(ax=ax[2], data=df_light_injured_by_region, x="region", y="p13c")
    sns.barplot(ax=ax[3], data=df_accidents_count_by_region, x="region", y="p1")
    plt.show()

# Ukol3: příčina nehody a škoda
def plot_damage(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    pass

# Ukol 4: povrch vozovky
def plot_surface(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):
    pass


if __name__ == "__main__":
    pass
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni ¨
    # funkce.
    df = get_dataframe("accidents.pkl.gz", verbose=True)
    plot_conseq(df, fig_location="01_nasledky.png", show_figure=True)
    plot_damage(df, "02_priciny.png", True)
    plot_surface(df, "03_stav.png", True)
