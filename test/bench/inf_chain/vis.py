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
df = pl.read_csv(args.csv, schema=schema).tail(100)

sns.lineplot(data=df, x="ns", y="bytes")

plt.savefig("out.png")