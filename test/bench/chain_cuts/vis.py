from pathlib import Path
import os

import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

import argparse

###############################################################################
# Globals

SCHEMAS = {
    "heapstate": {
        "ns": pl.Int64,
        "blocks": pl.Int64,
        "bytes": pl.Int64,
        "pools": pl.Int64,
        "ptrassigns": pl.Int64,
    },

    "progress": {
        "ns": pl.Int64,
        "count": pl.Int64,
    },
}

###############################################################################



###############################################################################
# parse_csv
#   given a csv path, return list of 
#   1. name and 
#   2. dfs from each data category found
def parse_csv(path: str) -> dict[str, pl.DataFrame]:
    cats: dict[str, list[list[str]]] = {} # categories of data recording
    with open(path, 'r') as f:
        for line in f:
            row = line.strip().split(",")
            if len(row) <= 1:
                continue
            if row[1] not in cats:
                # first encounter
                cats[row[1]] = []
            cats[row[1]].append([row[0]] + row[2:])
    
    # debug
    print(f"In {Path(path).name}, found these categories:")
    print(f"    {cats.keys()}")

    out: dict[str, pl.DataFrame] = {}
    for cat, rows in cats.items():
        if cat not in SCHEMAS:
            # some debug output prolly
            continue
        out[cat] = pl.from_records(rows, orient="row", schema=SCHEMAS[cat])
    
    return out
###############################################################################



###############################################################################
# plotter
def plot_heapstate_bytes(ax, run: dict[str, pl.DataFrame], label=None):
    sns.lineplot(data=run["heapstate"], x="ns", y="bytes", ax=ax, label=label)
###############################################################################



###############################################################################
# plotter
def plot_heapstate_latency(ax, run: dict[str, pl.DataFrame], label=None):
    heapstate_df = run["heapstate"].with_columns(
        pl.col("ns").diff().alias("latency")
    )\
    
    """ .filter(
        pl.col("latency") < 2000
    ) """
    sns.lineplot(data=heapstate_df, x="ns", y="latency", ax=ax, label=label)
###############################################################################


###############################################################################
# plotter
def plot_progress(ax, run: dict[str, pl.DataFrame], label=None):
    df = run["progress"]
    sns.lineplot(data=df, x="ns", y="count", ax=ax, label=label)
###############################################################################




###############################################################################
# main
def main():
    # read in csvs to process
    parser = argparse.ArgumentParser()
    parser.add_argument("inputs", nargs="*")
    args = parser.parse_args()

    # default to the most recent one
    if not args.inputs:
        args.inputs = [f"./reports/{sorted(os.listdir('./reports'))[-1]}"]

    # run_name : { category : <df>, ... }
    runs: dict[
        str, dict[
            str, pl.DataFrame]] = {}
    for csv in args.inputs:
        runs[Path(csv).stem] = parse_csv(csv)

    # filtering
    for run in runs.values():
        for name, df in run.items():
            pass
            #run[name] = df.tail(200)


    # TEMP
    plotters = [
        plot_heapstate_bytes,
        plot_progress
    ]
    n = len(plotters)
    m = len(args.inputs)

    combined = True

    if combined:
        fig, axes = plt.subplots(
            nrows=n,
            ncols=1,
            figsize=(7, n * 5),
            squeeze=False,
            sharex="all"
        )
        for row, plotter in enumerate(plotters):
            ax = axes[row][0]
            for col, run in enumerate(runs):
                plotter(ax, runs[run], label=run)
            ax.set_ylabel(plotter.__name__)
            ax.legend()
                    
    else:
        # create grid of axes based on the # runs passed and the # vizs requested
        fig, axes = plt.subplots(
            nrows=n,
            ncols=m,
            figsize=(m * 5, n * 4),
            squeeze=False,
            sharex="all",
            sharey="row"
        )

        # populate each spot on the subplot grid with 
        # the appropriate vis of the appropriate run
        for row, plotter in enumerate(plotters):
            for col, run in enumerate(runs):
                ax = axes[row][col]
                plotter(ax, runs[run])

                if row == 0:
                    ax.set_title(run)
                if col == 0:
                    ax.set_ylabel(plotter.__name__)
    
    plt.tight_layout()
    plt.savefig("out.png")
###############################################################################



main()