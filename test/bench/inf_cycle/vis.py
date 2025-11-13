import polars as pl

import seaborn as sns
import matplotlib.pyplot as plt

schema = {
    "us" : pl.Int32,
    "blocks" : pl.Int32,
    "bytes" : pl.Int32
}
df = pl.read_csv("reports/251112-213749.csv", schema=schema).tail(100)

sns.lineplot(data=df, x="us", y="bytes")

plt.savefig("out.png")