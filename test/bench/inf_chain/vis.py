import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

import argparse



def vis_heapstate(lines: list[list[str]]):
    schema = {
        "ns" : pl.Int32,
        "blocks" : pl.Int32,
        "bytes" : pl.Int32,
        "pools" : pl.Int32,
        "ptrassigns" : pl.Int32,
    }

    df = pl.from_records(lines, schema=schema)
    fig, axs = plt.subplots(nrows=2, sharex=True, figsize=(6, 10))

    sns.lineplot(data=df, x="ns", y="bytes", ax=axs[0])
    sns.lineplot(data=df, x="ns", y="latency", ax=axs[1])
    #sns.lineplot(data=df, x="ns", y="ptrassigns", ax=axs[2])

    latcol = df.get_column("latency")
    fig.text(.2,.4, f"50%={latcol.quantile(.5)}\n90%={latcol.quantile(.9)}\n99%={latcol.quantile(.99)}\n99.9%={latcol.quantile(.999)}")

    fig.tight_layout()

    plt.savefig("out.png")

    print(df.get_column("latency").quantile(.5))
    print(df.get_column("latency").quantile(.9))
    print(df.get_column("latency").quantile(.99))
    print(df.get_column("latency").quantile(.999))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("csv")
    args = parser.parse_args()
    inpath = args.csv

    cats: dict[str, list[str]] = {}
    with open(inpath, "r") as f:
        for line in f:
            row = line.strip().split(",")
            if row[0] not in cats:
                cats[row[0]] = []
            cats[row[0]].append(row[1:])
    
    vis_heapstate(cats["heapstate"])

main()