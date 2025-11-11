import sys
import re
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--gold", action="store_true")
args = parser.parse_args()

ptrs = {}
normalized = ""
while (l := sys.stdin.readline()):
    ptrs = ptrs | dict.fromkeys(re.findall(r"0x[\w\d]+", l))
    normalized += l

alpha = False if len(ptrs) > 26 else True 
id = 'A' if alpha else 0

subs = {}

for p in ptrs:
    subs[p] = id
    normalized = re.sub(p, f"< {id} >", normalized)
    id = chr(ord(id) + 1) if alpha else id + 1


print(normalized)

if not args.gold:
    for k, v in sorted(subs.items()):
        print(f"{v}\t{k}")