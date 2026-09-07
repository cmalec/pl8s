#!/usr/bin/env python3
"""Summarize a PNG dump: dark/light pixel counts per row band."""
import sys

path = sys.argv[1]
rows = open(path).read().splitlines()
print("size:", rows[0])
for i, r in enumerate(rows[1:]):
    dark = sum(1 for c in r if c == "@")
    light = sum(1 for c in r if c in "+*=:. ")
    if i % 8 == 0:
        print(f"{i:3d} dark={dark:4d} light={light:4d} sample={r[:48]}")
