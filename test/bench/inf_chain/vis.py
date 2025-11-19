import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

schema = {
    "ns" : pl.Int32,
    "bytes" : pl.Int32,
    "blocks" : pl.Int32,
    "pools" : pl.Int32,
    "ptrassigns" : pl.Int32,
}
df = pl.read_csv("reports/251118-220805.csv", schema=schema).tail(50)

sns.lineplot(data=df, x="ns", y="bytes")

plt.savefig("out.png")