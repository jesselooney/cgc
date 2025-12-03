from pathlib import Path
import os

import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

import argparse

###############################################################################
# Globals

SCHEMAS = {
    "metadata": {
        "gc": pl.String
    },

    "heapstate": {
        "ns": pl.Int64,
        "blocks": pl.Int64,
        "bytes": pl.Int64,
        "pools": pl.Int64,
        "ptrassigns": pl.Int64
    }
}

###############################################################################



###############################################################################
# parse_csv
#   given a csv path, return list of 
#   1. name and 
#   2. dfs from each data category found
def parse_csv(path: str) -> list[str, dict[str, pl.DataFrame]]:
    cats: dict[str, list[list[str]]] = {} # categories of data recording
    with open(path, 'r') as f:
        for line in f:
            row = line.strip().split(",")
            if not row[0]: 
                # empty row prolly
                continue
            if row[0] not in cats:
                # first encounter
                cats[row[0]] = []
            cats[row[0]].append(row[1:])

    out: dict[str, pl.DataFrame] = {}
    for cat, rows in cats.items():
        if cat not in SCHEMAS:
            # some debug output prolly
            continue
        out[cat] = pl.from_records(rows, orient="row", schema=SCHEMAS[cat])
    
    metadata = {
        "gc": out["metadata"].item(row=0, column="gc")
    }
    name = f"{metadata['gc']}_{Path(path).stem}"
    return name, out
###############################################################################



###############################################################################
# plotter
def plot_heapstate_bytes(ax, run: dict[str, pl.DataFrame]):
    sns.lineplot(data=run["heapstate"], x="ns", y="bytes", ax=ax)
###############################################################################



###############################################################################
# plotter
def plot_heapstate_latency(ax, run: dict[str, pl.DataFrame]):
    heapstate_df = run["heapstate"].with_columns(
        pl.col("ns").diff().alias("latency")
    )
    sns.lineplot(data=heapstate_df, x="ns", y="latency", ax=ax)
###############################################################################



###############################################################################
# main
def main():
    # get variadic num csvs from cmd line
        # each csv corresponds to its own col on the vis
    # for each csv, extract each data category
    # select which vis's to include
    # export the plot to a png
    parser = argparse.ArgumentParser()
    parser.add_argument("inputs", nargs="*")
    args = parser.parse_args()

    if not args.inputs:
        args.inputs = [f"./reports/{sorted(os.listdir('./reports'))[-1]}"]

    runs: dict[
        str, dict[
            str, pl.DataFrame]] = {}
    for csv in args.inputs:
        name, parsed = parse_csv(csv)
        runs[name] = parsed
    
    # debug print the data dataframes
    """ for i in runs:
        print("#########")
        for j in runs[i]:
            print(i, ": ", j)
            print(runs[i][j]) """
    # ok so now. 
    # runs = {
    #   arc_2342 : {
    #       heapstate : <dataframe>
    #       ...
    #   },
    #   ...
    # }


    # TEMP
    plotters = [
        plot_heapstate_bytes,
        plot_heapstate_latency
    ]
    num_runs = len(args.inputs)
    num_vizs = len(plotters)
    
    # create grid of axes based on the # runs passed and the # vizs requested
    fig, axes = plt.subplots(
        nrows=num_vizs,
        ncols=num_runs,
        figsize=(num_runs * 5, num_vizs * 4),
        squeeze=False
    )

    # populate each spot on the subplot grid with 
    # the appropriate vis of the appropriate run
    for row, plotter in enumerate(plotters):
        for col, run in enumerate(runs):
            ax = axes[row][col]
            plotter(ax, runs[run])
            if row == 0:
                ax.set_title(run)
            else:
                ax.set_title("")

            if col == 0:
                ax.set_ylabel(plotter.__name__)
            else:
                ax.set_ylabel("")
    
    plt.tight_layout()
    plt.savefig("out.png")
###############################################################################



main()