import sys
import re

id = 0
ptrs = {}
normalized = ""
while (l := sys.stdin.readline()):
    matches = set(re.findall("0x[\w\d]+", l))
    for match in matches:
        if match not in ptrs:
            ptrs[match] = id
            id += 1
    out = l
    for match in matches:
        out = re.sub(match, f"{ptrs[match]}", out)
    normalized += out

print(normalized)