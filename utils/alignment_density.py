#!/usr/bin/env python3
"""
This simple utility reads a MAF file and calculates the density of the
sequences contained within (i. e. the number of dashes divided by the sum
of sequence lengths).
"""

from sys import argv, exit, stderr

if len(argv) != 2:
    print("Usage: %s <file.maf>" % (argv[0],))
    exit(1)

total, dashes = 0, 0

with open(argv[1]) as f:
    for line in f:
        if not line.startswith('s '):
            continue
        seq = line.split()[-1].strip()
        total += len(seq)
        dashes += seq.count('-')


print("%s/%s; %f" % (dashes, total, dashes/total))
