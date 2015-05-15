#!/usr/bin/python
# -*- coding: utf-8 -*- 

# Draw the superposed graphs of the statistics folders from MicroGP given on the
# command line.
#
# Command line arguments: folder names containing statistic files named
# <number>.csv, and a test description named test_data.yaml

import csv
import os
import re
import shutil
import sys
import tempfile
import itertools
import yaml
import pandas as pd
import statsmodels.api as sm
import numpy as np
from matplotlib import cm
import matplotlib.pyplot as plt
import IPython

#pd.options.display.mpl_style = 'default'

def setAxLinesBW(ax):
    """
    Take each Line2D in the axes, ax, and convert the line style to be 
    suitable for black and white viewing.
    """
    MARKERSIZE = 8

    COLORMAP = {
        'b': {'color': '#3498db', 'marker': 'd', 'dash': [5,5]},
        'g': {'color': '#9b59b6', 'marker': 'o', 'dash': (None,None)},
        'r': {'color': '#e74c3c', 'marker': 'H', 'dash': [5,3,1,3]},
        'c': {'color': '#e67e22', 'marker': 's', 'dash': [1,3]},
        'm': {'color': '#f1c40f', 'marker': 'D', 'dash': [5,2,5,2,5,10]},
        'y': {'color': '#39d179', 'marker': '^', 'dash': [5,3,1,2,1,10]},
        'k': {'color': '#1abc9c', 'marker': 'x', 'dash': (None,None)} #[1,2,1,10]}
        }

    for line in ax.get_lines() + ax.get_legend().get_lines():
        origColor = line.get_color()
        line.set_color(COLORMAP[origColor]['color'])
        #line.set_dashes(COLORMAP[origColor]['dash'])
        line.set_marker(COLORMAP[origColor]['marker'])
        line.set_markersize(MARKERSIZE)
        # TODO make this configurable
        #line.set_markevery(50)

def setFigLinesBW(fig):
    """
    Take each axes in the figure, and for each line in the axes, make the
    line viewable in black and white.
    """
    for ax in fig.get_axes():
        setAxLinesBW(ax)

def read_stats_from_file(filename):
    with open(filename, "rb") as f:
        data = pd.DataFrame.from_csv(f)
        # Determine population names
        populations = []
        for h in data.columns:
            if h.endswith("_Generation"):
                populations.append(h[:-len("_Generation")])
        assert populations
        population = populations[0]
        if len(populations) > 1:
            print "WARNING: these statistics contain several populations, this is not handled very well now."
            print "We will only consider the population named %s" % population
            
    return {
        'file_name': filename,
        'headers': data.columns,
        'data': data,
        'population': population,
    }

def reduce_stats(stats, ugp_name, test_name):
    """
    Merge the stats from several runs
    """
    print "Stats: reducing stats of %d runs... " % len(stats)
   
    # Get the max generation and the max number of evaluations that all stats have in common
    min_gen = min(s['data'][s['population'] + '_Generation'].iloc[-1] for s in stats)
    min_eval = min(s['data'][s['population'] + '_EvalCount'].iloc[-1] for s in stats)
    # panel will contain for each field a dataframe with values from each file
    panel = pd.Panel.from_dict({s['file_name']: s['data'] for s in stats}, orient='minor')
    for h in panel.items:
        panel[h].fillna(method='ffill', inplace=True)
        
    # we also compute data depending on the number of evaluations
    eval_dict = {}
    eval_col = stats[0]['population'] + '_EvalCount'
    for s in stats:
        df = s['data'].drop_duplicates(cols=eval_col, take_last=True)
        df.set_index(eval_col, inplace=True, drop=False)
        # Normalize indices
        df = df.groupby(lambda x: int(x/100) * 100).max()
        eval_dict[s['file_name']] = df
    eval_panel = pd.Panel.from_dict(eval_dict, orient='minor')
    for h in eval_panel.items:
        # Fill the gaps (misaligned numbers of evaluations as indices)
        eval_panel[h].fillna(method='ffill', inplace=True)
            
    return {
        'ugp_name': ugp_name,
        'test_name': test_name,
        'file_name': stats[0]['file_name'],
        'population': stats[0]['population'],
        'nice_header': nice_header,
        'headers': stats[0]['headers'],
        'panel': panel,
        'eval_panel': eval_panel,
        'runs': len(stats),
        'min_gen': min_gen,
        'min_eval': min_eval,
    }

def nice_header(header, the=False):
    """
    This function computes user-friendly
    names for cryptic headers of internal MicroGP stats.
    """
    # Strip population name (until first underscore)
    h = header.split('_')[1:]
    # Fitness values
    fitness = None
    if h[0] == 'Best':
        fitness = "best ind. fitness"
    elif h[0] == 'Worst':
        fitness = "worst ind. fitness"
    elif h[0] == 'Avg':
        fitness = "average ind. fitness"
    elif h[0] == 'GrBest':
        fitness = "best group fitness"
    elif h[0] == 'GrWorst':
        fitness = "worst group fitness"
    elif h[0] == 'GrAvg':
        fitness = "average group fitness"
    if fitness is not None:
        # Determine component number
        m = re.search('f([0-9]+)', h[1])
        try:
            i = int(m.group(1)) + 1
            # https://codegolf.stackexchange.com/questions/4707/outputting-ordinal-numbers-1st-2nd-3rd#answer-4712
            k = i % 10
            fitness += " (%d%s comp.)" % (i, "tsnrhtdd"[(i/10%10!=1)*(k<4)*k::4])
        except ValueError:
            print 'nice_header: unable to determine fitness component'
        return ("the " if the else "") + fitness
    # TODO other internal parameters susceptible of being plotted
    return '_'.join(h)
    

def common_fields(stats):
    """
    Determine common fields (different versions of ugp → different fields)
    """
    intersection = set(stats[0]['headers'])
    for s in stats[1:]:
        intersection = intersection & set(s['headers'])
    return intersection

def comp_criteria(stats):
    """
    Return a tuple (common, diff) of two functions that given respectively
    the reduced stats object or a specific stat object will produce a textual
    description of respectively what these reduced stats have in common and
    how this specific stat panel is different from the others.
    """
    # Determine what we are comparing
    maxLen = stats[0]['panel'].shape[1]
    nb_ugp = len(set([s['ugp_name'] for s in stats]))
    nb_test = len(set([s['test_name'] for s in stats]))
    common = None
    diff = None
    if nb_ugp == 1:
        if nb_test == 1:
            if len(stats) > 1:
                print "WARNING: comparing the same ugp version %s on the same test %s (are you sure?)" % (stats[0]['ugp_name'], stats[0]['test_name'])
            common = lambda stats: "%s on %s" % (stats[0]['ugp_name'], stats[0]['test_name'])
            diff = lambda s: s['file_name']
        else:
            prefix = os.path.commonprefix([s['test_name'] for s in stats])
            if len(prefix) >= 2 and prefix.find('_'):
                prefix = prefix[:prefix.rindex('_')]
                print "Stats: comparing test %s on %d parameter sets, maxLen = %d" % (prefix, nb_test, maxLen)
                common = lambda stats: prefix
                diff = lambda s: s['test_name'][len(prefix) + 1:]
            else:
                print "Stats: comparing ugp version %s on %d tests, maxLen = %d" % (stats[0]['ugp_name'], nb_test, maxLen)
                common = lambda stats: stats[0]['ugp_name']
                diff = lambda s: s['test_name']
    else:
        if nb_test == 1:
            print "Stats: comparing %d ugp versions on the %s test, maxLen = %d" % (nb_ugp, stats[0]['test_name'], maxLen)
            common = lambda stats: stats[0]['test_name']
            diff = lambda s: s['ugp_name']
        else:
            print "WARNING: comparing %d ugp versions on %d tests (are you sure?), maxLen = %d" % (nb_ugp, nb_test, maxLen)
            common = lambda stats: "Tests"
            diff = lambda s: s['file_name']
    return common, diff

def compare_statistics(stats, header_selector, err='std', x='gen', bw=False, group=20):
    """
    Visualize and statistically compare stats from the reduce function.
    err = ['std', 'minmax', 'box']
            'std' displays error bars
            'minmax' displays the curve of the minimum and maximum
            'box' displays functional boxplots
    x = ['gen', 'eval']
    """
    # Draw graphs

    common, diff = comp_criteria(stats)

    for header in common_fields(stats):
        if not header_selector(header):
            continue
        def graph(fig, panel, xlabel):
            ax = fig.add_subplot(111)
            grouped = panel[header].groupby(lambda x: int(x/group) * group).mean()
            xdata = grouped.index.values
            if err == 'std':
                ax.errorbar(xdata, grouped.mean(axis=1), grouped.std(axis=1),
                            label="%s, %s runs" % (diff(s), s['runs']))
            elif err == 'mean':
                ax.plot(xdata, grouped.mean(axis=1),
                        label="%s, %s runs (mean)" % (diff(s), s['runs']))
            elif err == 'median':
                ax.plot(xdata, grouped.median(axis=1),
                        label="%s, %s runs (median)" % (diff(s), s['runs']))
            elif err == 'minmax':
                pmax = ax.plot(xdata, grouped.max(axis=1),
                        label="%s, %s runs (max)" % (diff(s), s['runs']))
                pmedian = ax.plot(xdata, grouped.median(axis=1),
                        label="%s, %s runs (median)" % (diff(s), s['runs']),
                        color=pmax[0].get_color())
                pmin = ax.plot(xdata, grouped.min(axis=1),
                        label="%s, %s runs (min)" % (diff(s), s['runs']),
                        color=pmax[0].get_color())
            elif err == 'box':
                sm.graphics.fboxplot(grouped.values.T, xdata, ax=ax)
            elif err == 'all':
                color = None
                for col in grouped.columns:
                    # TODO choose colors intelligently
                    if color:
                        ax.plot(xdata, grouped[col], color=color)
                    else:
                        p = ax.plot(xdata, grouped[col], label=diff(s))
                        color = p[0].get_color()
            ax.set_xlabel(xlabel)
            ax.set_ylabel(nice_header(header))
            ax.legend(loc='best')
        if x == 'gen':
            # Graph depending on the generation
            fig = plt.figure()
            fig.suptitle('%s: comparison of %s wrt generation number' % (
                common(stats), nice_header(header, the=True)))
            for s in stats:
                graph(fig, s['panel'], 'generation')
            if err in ['mean', 'median', 'minmax']:
                setFigLinesBW(fig)
            fig.show()
        else:
            # Same graph depending on the number of evaluations
            fig = plt.figure()
            fig.suptitle('%s: comparison of %s wrt number of evaluations' % (
                common(stats), nice_header(header, the=True)))
            for s in stats:
                graph(fig, s['eval_panel'], 'evaluation')
            if err in ['mean', 'median', 'minmax']:
                setFigLinesBW(fig)
            fig.show()
            
def boxplot_(stats, header_selector, x='gen', pos=-1):
    """
    Draw one boxplot per stat of the values of the selected header at the
    generation (x='gen') or evaluation count (x='eval') given by pos
    (pos=-1 → last, pos >= 0 → the given gen/evalCount)
    """
    common, diff = comp_criteria(stats)
    
    wordfor = {'gen': 'generations', 'eval': 'evaluations'}
    panelfor = {'gen': 'panel', 'eval': 'eval_panel'}
    
    max = min(s['min_' + x] for s in stats)
    if pos == -1:
        pos = max
    elif pos > max:
        print "boxplot: WARNING, given pos=%i greater than max=%i" % (pos, max)
        print "boxplot: some of the used values will be older than required (results from fillna())"
    
    if x == 'eval':
        pos = int(pos / 100) * 100
    
    print "boxplot: showing after %i %s" % (pos, wordfor[x])
    
    for header in common_fields(stats):
        if not header_selector(header):
            continue
        fig = plt.figure()
        fig.suptitle('%s: values of %s after %i %s' % (
            common(stats), nice_header(header, the=True), pos, wordfor[x]))
        data = []
        labels = []
        for s in stats:
            columnfor = {
                'gen': s['population'] + '_Generation',
                'eval': s['population'] + '_EvalCount',
            }
            # Select rows at requested position
            p = s[panelfor[x]]
            # Selector contains 
            #      seed1    seed2    seed3
            # 0        
            # 1      true when the row matches the criterion 'P0_Generation'== pos
            # 3
            selector = p[columnfor[x]] == pos
            row = []
            for seed in p.minor_axis.values:
                row.append(p[header][seed] # Get the column for the given header, seed
                           .loc[selector[seed]] # Filter only the rows that match
                           .iloc[0]) # If several rows match, take the first one
            data.append(row)
            labels.append(diff(s))
        ax = fig.add_subplot(111)
        ax.boxplot(data, labels=labels)
        ax.set_ylabel(nice_header(header))
        fig.show()
    
def boxplot_convergence_(stats, header_selector, value, y='gen', decreasing=False):
    """
    Draw one boxplot per stat of the first number of evaluations/generations (y)
    at which the selected header reaches the threshold 'value'.
    """
    common, diff = comp_criteria(stats)
    
    wordfor = {'gen': 'generations', 'eval': 'evaluations'}
    panelfor = {'gen': 'panel', 'eval': 'eval_panel'}
    
    for header in common_fields(stats):
        if not header_selector(header):
            continue
        fig = plt.figure()
        fig.suptitle('%s: number of %s for %s to reach %s' % (
            common(stats), wordfor[y], nice_header(header, the=True), value))
        data = []
        labels = []
        for s in stats:
            columnfor = {
                'gen': s['population'] + '_Generation',
                'eval': s['population'] + '_EvalCount',
            }
            # Select rows at requested position
            p = s['panel']
            # Selector contains 
            #      seed1    seed2    seed3
            # 0        
            # 1      true when the row matches the criterion 
            # 3
            if decreasing:
                selector = p[header] <= value
            else:
                selector = p[header] >= value
            row = []
            for seed in p.minor_axis.values:
                matching = p[columnfor[y]][seed].loc[selector[seed]]
                if len(matching) > 0:
                    row.append(matching.iloc[0])
                else:
                    print "boxplot: WARNING: seed %s, header %s never reaches %f" % (
                        seed, header, value)
            data.append(row)
            labels.append("%s\n(%i runs)" % (diff(s), len(row)))
        ax = fig.add_subplot(111)
        ax.boxplot(data, labels=labels)
        ax.set_ylabel("number of %s" % wordfor[y])
        fig.show()

def op_usage(stats, group=10, bw=False, filter=''):
    """
    group = size of the buckets for the graphs
    """
    common, diff = comp_criteria(stats)
    # histogram
    fig = plt.figure()
    fig.suptitle("Operator usage wrt generation number")
    for i in range(0, len(stats)):
        ax = fig.add_subplot(len(stats), 1, i + 1) 
        ax.set_xlabel('generation')
        ax.set_ylabel('#uses/gen')
        ax.set_title('Using ' + diff(stats[i]))
        bars = []
        names = []
        # /10 because we subsample the data
        bottom = None
        numOps = sum(1 for headerName in stats[i]['headers'] if '_OpUse_' in headerName)
        colors = itertools.cycle([cm.jet(0.14259401*j - np.floor(0.14259401*j)) for j in range(0, numOps)])
        for header in stats[i]['headers']:
            if "_OpUse_" in header and filter in header:
                names.append(header.split('_')[-1])
                mean = stats[i]['panel'][header].groupby(lambda x: int(x/group) * group).mean().mean(axis=1)
                if bottom is None:
                    bottom = np.zeros_like(mean)
                bars.append(ax.bar(mean.index.values, mean,
                                   bottom=bottom,
                                   linewidth=0, width=group,
                                   color=colors.next()))
                bottom = np.add(bottom, mean)
        if i == 0:
            ax.legend(tuple(reversed([b[0] for b in bars])),
                    tuple(reversed(names)), loc='upper left',
                    bbox_to_anchor=(1.02, 1))
    fig.subplots_adjust(right=0.6)
    fig.show()
    # normal plots
    fig = plt.figure()
    fig.suptitle("Operator usage wrt generation number")
    for i in range(0, len(stats)):
        ax = fig.add_subplot(len(stats), 1, i + 1) 
        ax.set_xlabel('generation')
        ax.set_ylabel('#uses/gen')
        ax.set_title('Using ' + diff(stats[i]))
        for header in stats[i]['headers']:
            if "_OpUse_" in header and filter in header:
                name = header.split('_')[-1]
                mean = stats[i]['panel'][header].groupby(lambda x: int(x/group) * group).mean().mean(axis=1)
                ax.plot(mean.index.values, mean, label=name)
        if i == 0:
            ax.legend(loc='upper left', bbox_to_anchor=(1.02, 1))
    fig.subplots_adjust(right=0.6)
    fig.show()

def begin_interactive(reduced_stats, headers):
    # Show comparison of the result from the various executables
    print """
Query results:
Examples: show('Sigma') # Type part of the name
          show('Sigma', err='mean') # No errorbars, only the mean
          show('Sigma', err='median') # No errorbars, only the median
          show('Sigma', err='minmax') # Min and max curves
          show('Sigma', err='box') # Functional boxplot
          show('Sigma', err='all') # All curves, superposed
          show('Sigma', x='eval') # Graph wrt number of evaluations
          show('Sigma', group=20) # One point per group of 20 generations

          boxplot('Best') # Box plot of the given parameter after
                          # max common number of generations
          boxplot('Best', x='eval') # same after max common number of evaluations

          boxplot_convergence('Best', 10000, y='gen')
          # Boxplot of the number of generations before the given header
          # reaches the given value (supposed monotonic)

          opuse() # Operator usage
          opuse(group=20) # One point per group of 20 generations
          opuse(filter='test') # Only operators which name contains 'test'

Available headers: """
    print headers
    
    def show(name, **kwargs):
        compare_statistics(reduced_stats, lambda h: name in h, **kwargs)
        
    def boxplot(name, **kwargs):
        boxplot_(reduced_stats, lambda h: name in h, **kwargs)
        
    def boxplot_convergence(name, value, **kwargs):
        boxplot_convergence_(reduced_stats, lambda h: name in h, value, **kwargs)
    
    def opuse(**kwargs):
        op_usage(reduced_stats, **kwargs)
    
    IPython.embed()

if __name__ == '__main__':
    directories = sys.argv[1:]
    if not directories:
        print "Please give at least one directory from which to read statistics."
        sys.exit(1)

    reduced_stats = []
    for d in directories:
        stats = []
        test_data = False
        for fn in os.listdir(d):
            if fn == "test_data.yaml":
                with open(os.path.join(d, fn)) as f:
                    test_data = yaml.safe_load(f)
            elif re.match("\d+.csv$", fn):
                stats.append(read_stats_from_file(os.path.join(d, fn)))
        if test_data:
            ugp_name = test_data.get('ugp_name', '<anon ugp>')
            test_name_from_file = re.match('.*(?=_%s)' % re.escape(ugp_name), os.path.split(d)[-1])
            test_name = test_data.get('test_name', test_name_from_file.group(0) if test_name_from_file else '<anon test>')
            reduced_stats.append(reduce_stats(stats, ugp_name, test_name))
        else:
            print "No test data found for directory `%s'." % d
    
    # First determine common fields (different versions of ugp → different fields)
    intersection = set(reduced_stats[0]['headers'])
    for s in reduced_stats[1:]:
        intersection = intersection & set(s['headers'])

    begin_interactive(reduced_stats, intersection)




