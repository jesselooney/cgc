import sys
import re

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

for k, v in sorted(subs.items()):
    print(f"{v}\t{k}")