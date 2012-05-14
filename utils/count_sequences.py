#!/usr/bin/env python3
"""
This script runs through a MAF file and counts the different sequences
present.
"""

import sys

assert len(sys.argv) == 2

seqs = set()

with open(sys.argv[1]) as f:
    for line in f:
        if line.startswith('s '):
            seqs.add(line.split(None, 2)[1])

print(len(seqs))

print(sorted(seqs))
