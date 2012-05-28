#!/usr/bin/env python3
"""
This script generates plots of lengths of sequences and blocks contained
within a given MAF file.
"""

from sys import argv, exit, stderr
import subprocess

if not 3 <= len(argv) <= 4:
    print("Usage: %s <file.maf> <output> [max_length]" % (argv[0],), file=stderr)
    exit(1)

supported_outputs = {
    "jpg": "jpeg",
    "jpeg": "jpeg",
    "pdf": "pdf driver",
    "png": "png",
    "svg": "svg",
    "mp": "mp",
    "ps": "postscript",
    "gif": "gif",
    "tex": "tikz",
    "raw": None,
}

output_extension = argv[2].split('.')[-1]
if output_extension not in supported_outputs:
    print("Supported output types: %s" % (" ".join(sorted(supported_outputs)),),
          file=stderr)
    exit(1)

try:
    max_length = int(argv[3])
except IndexError:
    max_length = -1

blocks, seqs = dict(), dict()

with open(argv[1]) as f:
    new_block = True
    for line in f:
        if line.startswith('a '):
            new_block = True
        elif line.startswith('s '):
            seq_len = len(line.split()[-1].strip())
            seqs.setdefault(seq_len, 0)
            seqs[seq_len] += 1
            if new_block:
                blocks.setdefault(seq_len, 0)
                blocks[seq_len] += 1
                new_block = False

if output_extension == "raw":
    output_file = open(argv[2], 'wb')
else:
    gnuplot_template = """
set autoscale
set logscale x
set xlabel "Sequence size"
set ylabel "Count"
set key top right
set term %s
set output "%s"
plot "-" title "blocks" with lines, \\
     "-" title "sequences" with lines
""" % (supported_outputs[output_extension], argv[2])

    gnuplot = subprocess.Popen("gnuplot", bufsize=-1, stdin=subprocess.PIPE)
    output_file = gnuplot.stdin
    output_file.write(bytes(gnuplot_template, 'utf-8'))

for d in (blocks, seqs):
    elem = ((l, c) for l, c in sorted(d.items())
            if max_length < 0 or l <= max_length)
    for length, count in elem:
        output_file.write(bytes("%d %d\n" % (length, count), 'utf-8'))
    output_file.write(b"e\n")

output_file.close()
if not output_extension == "raw":
    gnuplot.wait()
