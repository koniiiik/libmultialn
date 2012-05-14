#!/usr/bin/env python3
"""
This script generates plots of the size of individual BitSequence
implementations depending on the length of the sequence. It uses the
bitseq_assess binary to get this information.
"""

from random import random
from subprocess import Popen, PIPE

def get_bitseq_size(input_bytes, bitseq_type):
    process = Popen(["examples/bitseq_assess", "/dev/stdin", bitseq_type],
            stdin=PIPE, stdout=PIPE)
    process.stdin.write(input_bytes)
    process.stdin.close()
    output = process.stdout.readline()
    process.wait()
    return int(output.decode('ascii'))


density = 0.9
bitseq_types = ('RG2', 'RG3', 'RG4', 'RG20', 'RRR', 'SDArray')
results = {t: [] for t in bitseq_types}

for t in bitseq_types:
    for length in range(10, 10001, 5):
        seq = "".join('-' if random() > density else 'a'
                      for x in range(length))
        seq = bytes(seq, 'ascii')
        results[t].append((length, get_bitseq_size(seq, t)))
    print("%s finished." % (t,))

gnuplot_template = """
set autoscale
set xlabel "Sequence size"
set ylabel "Sequence length"
set key top left
set term svg
set output "bitseq_assess.svg"
plot %s
""" % (", ".join('"-" title "%s" with lines' % (t,) for t in bitseq_types),)

gnuplot = Popen("gnuplot", bufsize=-1, stdin=PIPE)
gnuplot.stdin.write(bytes(gnuplot_template, 'ascii'))

for t in bitseq_types:
    for length, size in results[t]:
        gnuplot.stdin.write(bytes("%d %d\n" % (length, size), 'ascii'))
    gnuplot.stdin.write(b'e\n')

gnuplot.stdin.close()
gnuplot.wait()
