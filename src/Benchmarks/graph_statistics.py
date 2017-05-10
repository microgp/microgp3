#!/usr/bin/python
# -*- coding: utf-8 -*- 

# Draw the graphs of the statistic file on the command line.
#
# Command line arguments: statistic file produced by MicroGP

import compare_statistics as cs
import sys
import IPython

filename = sys.argv[1]
if not filename:
    print "Please give one file name from which to read statistics."
    sys.exit(1)

stats = [cs.read_stats_from_file(filename)]
reduced_stats = [cs.reduce_stats(stats, '<anon ugp>', '<anon test>')]

cs.begin_interactive(reduced_stats, stats[0]['headers'])


