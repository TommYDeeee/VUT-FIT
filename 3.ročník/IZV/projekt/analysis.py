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
    if not os.path.exists(filename):
        print("Invalid filename")
        exit(1)
    df = pd.read_pickle(filename)
    old_size = round(sum(df.memory_usage(deep=True).values)/1048576,1)
    dates = [date for date in df['p2a']]
    as_categorical = [
        'p36', 'p37', 'weekday(p2a)', 'p6', 'p7', 'p8', 'p9', 'p10',
        'p11', 'p12', 'p14', 'p15', 'p16',
        'p17', 'p18', 'p19', 'p20', 'p21', 'p22', 'p23', 'p24', 'p27',
        'p28', 'p34', 'p35', 'p39', 'p44', 'p45a', 'p47', 'p48a', 'p49',
        'p50a', 'p50b', 'p51', 'p52', 'p55a', 'p57', 'p58', 'h',
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
    df_deaths_by_region = df.groupby(['region']).agg({"p13a": "sum"}).reset_index().sort_values('p13a', ascending=False)
    df_heavy_injured_by_region = df.groupby(['region']).agg({"p13b": "sum"}).reset_index().sort_values('p13b', ascending=False)
    df_light_injured_by_region = df.groupby(['region']).agg({"p13c": "sum"}).reset_index().sort_values('p13c', ascending=False)
    df_accidents_count_by_region = df.groupby(['region']).agg({"p1": "count"}).reset_index().sort_values('p1', ascending=False)
    fig, ax = plt.subplots(4, figsize=(8.27, 11.69))
    fig.patch.set_facecolor('tab:gray')
    sns.barplot(ax=ax[0], data=df_deaths_by_region, x="region", y="p13a", palette='mako', zorder=3)
    ax[0].title.set_text('Úmrtia')
    sns.barplot(ax=ax[1], data=df_heavy_injured_by_region, x="region", y="p13b", palette='mako', zorder=3)
    ax[1].title.set_text('Ťažko zranení')
    sns.barplot(ax=ax[2], data=df_light_injured_by_region, x="region", y="p13c", palette='mako', zorder=3)
    ax[2].title.set_text('Ľahko zranení')
    sns.barplot(ax=ax[3], data=df_accidents_count_by_region, x="region", y="p1", palette='mako', zorder=3)
    ax[3].title.set_text('Celkový počet nehôd')
    for ax in ax:
        ax.set(xlabel="Región", ylabel="Počet")
        ax.set_facecolor('darkgray')
        ax.yaxis.grid(True, zorder=0, color='dimgrey')
    fig.subplots_adjust(hspace=0.5, top=0.95, bottom=0.05)

    if fig_location is not None:
        os.makedirs(
            os.path.dirname(
                os.path.abspath(fig_location)),
            exist_ok=True)
        plt.savefig(fig_location, dpi=600)
    
    if show_figure:
        plt.show()

# Ukol3: příčina nehody a škoda
def plot_damage(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):

    df.drop(df.columns.difference(['p1', 'region','p12', 'p53']), 1, inplace=True)
    print(df)
    df.drop(df[(df['region'] != "JHM") & (df['region'] != "HKK") & (df['region'] != "PLK") & (df['region'] != "PHA")].index, inplace=True)
    df['p53'] = df['p53']/10
    df["pricina"] = pd.cut(df["p12"], [np.NINF, 200,300,400,500,600,700], labels=["nezaviněná řidičem", 
                                                                                "nepřiměřená rychlost jízdy", 
                                                                                "nesprávné předjíždění", 
                                                                                "nedání přednosti v jízdě", 
                                                                                "nesprávný způsob jízdy", 
                                                                                "technická závada vozidla"])

    df["skoda"] = pd.cut(df["p53"], [np.NINF, 50, 200, 500, 1000, np.inf])
    df_skoda_JHM = df.loc[df["region"] == "JHM"].groupby(['skoda', 'pricina']).size().reset_index(name='pocet')
    df_skoda_HKK = df.loc[df["region"] == "HKK"].groupby(['skoda', 'pricina']).size().reset_index(name='pocet') 
    df_skoda_PLK = df.loc[df["region"] == "PLK"].groupby(['skoda', 'pricina']).size().reset_index(name='pocet') 
    df_skoda_PHA = df.loc[df["region"] == "PHA"].groupby(['skoda', 'pricina']).size().reset_index(name='pocet') 

    fig, ax = plt.subplots(nrows=2, ncols=2, figsize=(8.27, 11.69))
    fig.patch.set_facecolor('darkgray')
    sns.set_style("darkgrid")
    sns.barplot(ax=ax[0][0], data=df_skoda_JHM, x="skoda", y="pocet", hue="pricina", zorder=3)
    ax[0][0].title.set_text("JHM")
    handles, labels = ax[0][0].get_legend_handles_labels()
    sns.barplot(ax=ax[0][1], data=df_skoda_HKK, x="skoda", y="pocet", hue="pricina", zorder=3)
    ax[0][1].title.set_text("HKK")
    sns.barplot(ax=ax[1][0], data=df_skoda_PLK, x="skoda", y="pocet", hue="pricina", zorder=3)
    ax[1][0].title.set_text("PLK")
    sns.barplot(ax=ax[1][1], data=df_skoda_PHA, x="skoda", y="pocet", hue="pricina", zorder=3)
    ax[1][1].title.set_text("PHA")

    for row_ax in ax:
        for ax in row_ax:
            ax.set_yscale("log")
            ax.legend().remove()
            ax.set(xlabel="Škoda [tisíc Kč]", ylabel="Počet")
            ax.set_xticklabels(["< 50", "50 - 200", "200 - 500", "500 - 1000", "> 1000"])
            plt.setp(ax.get_xticklabels(), rotation=30)
            ax.set_facecolor('#eaeaf2')
            ax.yaxis.grid(True, zorder=0, color='dimgrey')

    fig.legend(handles, labels, loc='center right', bbox_to_anchor=(1.01, 0.39))
    fig.subplots_adjust(hspace=0.35, wspace=0.25, top=0.95, bottom=0.1, right=0.95, left= 0.08)
    
    if fig_location is not None:
        os.makedirs(
            os.path.dirname(
                os.path.abspath(fig_location)),
            exist_ok=True)
        plt.savefig(fig_location, dpi=600)
    
    if show_figure:
        plt.show()

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
    #plot_conseq(df, fig_location="01_nasledky.png", show_figure=True)
    plot_damage(df, "02_priciny.png", True)
    plot_surface(df, "03_stav.png", True)
