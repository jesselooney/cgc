import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("csv")
args = parser.parse_args()

schema = {
    "ns" : pl.Int32,
    "bytes" : pl.Int32,
    "blocks" : pl.Int32,
    "pools" : pl.Int32,
    "ptrassigns" : pl.Int32,
}
df = pl.read_csv(args.csv, schema=schema)\
    .with_columns(
        pl.col("ns").diff().alias("latency")
    )\
    .tail(10000)\

fig, axs = plt.subplots(nrows=3, sharex=True, figsize=(6, 12))

sns.lineplot(data=df, x="ns", y="bytes", ax=axs[0])
sns.lineplot(data=df, x="ns", y="latency", ax=axs[1])
sns.lineplot(data=df, x="ns", y="ptrassigns", ax=axs[2])

fig.tight_layout()

plt.savefig("out.png")