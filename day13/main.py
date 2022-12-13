import sys
from functools import cmp_to_key

packets = [eval(l) for l in open(sys.argv[1]).readlines() if l.strip()]

def compare(l, r):
    if isinstance(l, int) and isinstance(r, int):
        return 1 if l < r else -1 if l > r else 0 
    else:
        if isinstance(r, int):
            r = [r]
        
        if isinstance(l, int):
            l = [l]

        for c in (compare(le, re) for le, re in zip(l, r)):
            if c != 0:
                return c

        return 1 if len(l) < len(r) else -1 if len(l) > len(r) else 0

pairs = zip(packets[::2], packets[1::2])
print("1:", sum(idx + 1 for idx, (l, r) in enumerate(pairs) if compare(l, r) >= 0))
