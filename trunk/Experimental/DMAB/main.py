#!/usr/bin/env python
# -*- coding:utf-8 -*-


from ops import *
from policies import *

from collections import defaultdict
import IPython
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

#pd.options.display.mpl_style = 'default'

def make_policies(duration, ops):
    pol = [
    Reference(ops),
    #Random(ops),
    DMAB(ops),
    #DMABLR(ops),
    PDMAB(ops),
    KRDMAB(ops, 20),
    KRDMAB(ops, 10),
    KRDMAB(ops, 5),
    KRDMAB(ops, 0),
    #PDMABLR(ops),
    #RDMAB(ops),
    #KRDMAB(ops, 0),
    #KRDMAB(ops, 1),
    #KRDMAB(ops, 2),
    #KRDMAB(ops, 5),
    #KRDMAB(ops, 10),
    #KRDMABLR(ops, 10),
    #SMAB(ops),
    #RSMAB(ops, 1),
    #RSMAB(ops, 5),
    #RSMAB(ops, 10),
    ]
    for p in pol:
        p.set_duration(duration)
    return pol

environments = {
    # Bit of everything
    'everything': lambda: [
        ConstOp("bad0", 0),
        ConstOp("bad1", 0.01),
        ConstOp("bad2", 0.02),
        Noise(ConstOp("noisyBad", 0), 0.05),
        StepOp("goodThenBad", 10000, 0, 0.5),
        StepOp("badThenGood", 10000, 0.5, 0),
        SlowStepOp("slowGoodThenBad", 10000, 0, 0.6, 5000),
        SlowStepOp("slowBadThenGood", 10000, 0.6, 0, 5000),
        BofBofWowOp("bofBofWow", 0.45, 1, 0.1),
        Noise(ConstOp("noisyGood", 0.5), 0.1),
        ],
    'one good': lambda: [
        ConstOp("bad0", 0),
        ConstOp("bad1", 0.1),
        ConstOp("bad2", 0.2),
        ConstOp("good", 0.9),
        ],
    'one bad': lambda: [
        ConstOp("bad", 0.1),
        ConstOp("good1", 0.91),
        ConstOp("good2", 0.92),
        ConstOp("good3", 0.93),
        ],
    'noisy ladder': lambda: [
        Noise(ConstOp("step0", 0), 0.08),
        Noise(ConstOp("step1", 0.1), 0.08),
        Noise(ConstOp("step2", 0.2), 0.08),
        Noise(ConstOp("step3", 0.3), 0.08),
        Noise(ConstOp("step4", 0.4), 0.08),
        Noise(ConstOp("step5", 0.5), 0.08),
        Noise(ConstOp("step6", 0.6), 0.08),
        Noise(ConstOp("step7", 0.7), 0.08),
        Noise(ConstOp("step8", 0.8), 0.08),
        Noise(ConstOp("step9", 0.9), 0.08),
        Noise(ConstOp("step10", 1), 0.08),
        ],
    'simple changing': lambda: [
        DoorOp('a',     0,  1000, 0.1, 0.9),
        DoorOp('b',  1000,  2000, 0.1, 0.9),
        DoorOp('c',  2000,  3000, 0.1, 0.9),
        DoorOp('d',  3000,  4000, 0.1, 0.9),
        DoorOp('e',  4000,  5000, 0.1, 0.9),
        DoorOp('f',  5000,  6000, 0.1, 0.9),
        DoorOp('g',  6000,  7000, 0.1, 0.9),
        DoorOp('h',  7000,  8000, 0.1, 0.9),
        DoorOp('i',  8000,  9000, 0.1, 0.9),
        DoorOp('j',  9000, 10000, 0.1, 0.9),
        DoorOp('k', 10000, 11000, 0.1, 0.9),
        DoorOp('l', 11000, 12000, 0.1, 0.9),
        DoorOp('m', 12000, 13000, 0.1, 0.9),
        DoorOp('n', 13000, 14000, 0.1, 0.9),
        DoorOp('o', 14000, 15000, 0.1, 0.9),
        DoorOp('p', 15000, 16000, 0.1, 0.9),
        DoorOp('q', 16000, 17000, 0.1, 0.9),
        DoorOp('r', 17000, 18000, 0.1, 0.9),
        DoorOp('s', 18000, 19000, 0.1, 0.9),
        DoorOp('t', 19000, 20000, 0.1, 0.9),
        ],
    'noisy simple changing': lambda: [
        Noise(DoorOp('a',     0,  1000, 0.1, 0.9), 0.2),
        Noise(DoorOp('b',  1000,  2000, 0.1, 0.9), 0.2),
        Noise(DoorOp('c',  2000,  3000, 0.1, 0.9), 0.2),
        Noise(DoorOp('d',  3000,  4000, 0.1, 0.9), 0.2),
        Noise(DoorOp('e',  4000,  5000, 0.1, 0.9), 0.2),
        Noise(DoorOp('f',  5000,  6000, 0.1, 0.9), 0.2),
        Noise(DoorOp('g',  6000,  7000, 0.1, 0.9), 0.2),
        Noise(DoorOp('h',  7000,  8000, 0.1, 0.9), 0.2),
        Noise(DoorOp('i',  8000,  9000, 0.1, 0.9), 0.2),
        Noise(DoorOp('j',  9000, 10000, 0.1, 0.9), 0.2),
        Noise(DoorOp('k', 10000, 11000, 0.1, 0.9), 0.2),
        Noise(DoorOp('l', 11000, 12000, 0.1, 0.9), 0.2),
        Noise(DoorOp('m', 12000, 13000, 0.1, 0.9), 0.2),
        Noise(DoorOp('n', 13000, 14000, 0.1, 0.9), 0.2),
        Noise(DoorOp('o', 14000, 15000, 0.1, 0.9), 0.2),
        Noise(DoorOp('p', 15000, 16000, 0.1, 0.9), 0.2),
        Noise(DoorOp('q', 16000, 17000, 0.1, 0.9), 0.2),
        Noise(DoorOp('r', 17000, 18000, 0.1, 0.9), 0.2),
        Noise(DoorOp('s', 18000, 19000, 0.1, 0.9), 0.2),
        Noise(DoorOp('t', 19000, 20000, 0.1, 0.9), 0.2),
        ],
    'noisy slow changing': lambda: [
        Noise(SlowDoorOp('a',     0,  2000, 0.1, 0.9, 500), 0.15),
        Noise(SlowDoorOp('b',  2000,  5000, 0.1, 0.9, 600), 0.15),
        Noise(SlowDoorOp('c',  5000,  7000, 0.1, 0.9, 400), 0.15),
        Noise(SlowDoorOp('d',  7000,  10000, 0.1, 0.9, 1000), 0.15),
        Noise(SlowDoorOp('e',  10000, 15000, 0.1, 0.9, 2500), 0.15),
        Noise(SlowDoorOp('f',  15000, 17000, 0.1, 0.9, 500), 0.15),
        Noise(SlowDoorOp('g',  17000, 250000, 0.1, 0.9, 1000), 0.15),
        ],
    'slow changing': lambda: [
        SlowDoorOp('a',     0,  2000, 0.1, 0.9, 500),
        SlowDoorOp('b',  2000,  5000, 0.1, 0.9, 600),
        SlowDoorOp('c',  5000,  7000, 0.1, 0.9, 400),
        SlowDoorOp('d',  7000,  10000, 0.1, 0.9, 1000),
        SlowDoorOp('e',  10000, 15000, 0.1, 0.9, 2500),
        SlowDoorOp('f',  15000, 17000, 0.1, 0.9, 500),
        SlowDoorOp('g',  17000, 250000, 0.1, 0.9, 1000),
        ],
#     'smooth changing': [
#         Smooth(Noise(DoorOp('a',     0,  1000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('b',  1000,  2000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('c',  2000,  3000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('d',  3000,  4000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('e',  4000,  5000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('f',  5000,  6000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('g',  6000,  7000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('h',  7000,  8000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('i',  8000,  9000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('j',  9000, 10000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('k', 10000, 11000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('l', 11000, 12000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('m', 12000, 13000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('n', 13000, 14000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('o', 14000, 15000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('p', 15000, 16000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('q', 16000, 17000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('r', 17000, 18000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('s', 18000, 19000, 0.1, 0.9), 0.2), 300),
#         Smooth(Noise(DoorOp('t', 19000, 20000, 0.1, 0.9), 0.2), 300),
#         ],
    'door': lambda: [
        ConstOp("bad", 0.2),
        ConstOp("bof", 0.5),
        DoorOp("door", 3000, 12000, 0.1, 0.8),
        ],
    'noisy door': lambda: [
        Noise(ConstOp("bad", 0.2), 0.08),
        Noise(ConstOp("bof", 0.5), 0.08),
        Noise(DoorOp("door", 3000, 12000, 0.1, 0.8), 0.08),
        ],
    'slow replacement': lambda: [
        ConstOp("bad0", 0),
        SlowStepOp("slowGoodThenBad", 5000, 0, 1, 5000),
        SlowStepOp("slowBadThenGood", 5000, 1, 0, 5000),
        ],
    'noisy slow replacement': lambda: [
        Noise(ConstOp("bad0", 0), 0.1),
        Noise(ConstOp("bad1", 0.1), 0.1),
        Noise(ConstOp("bad2", 0.2), 0.1),
        Noise(SlowStepOp("slowGoodThenBad", 5000, 0, 1, 5000), 0.15),
        Noise(SlowStepOp("slowBadThenGood", 5000, 1, 0, 5000), 0.15),
        ],
    # Forget having only {0,1} rewards, it generates too many resets
    #    ('bernoulliesque one good', lambda seed: [
    #        Bernoulli(ConstOp("bad1", 0.1)),
    #        Bernoulli(ConstOp("bad2", 0.2)),
    #        Bernoulli(ConstOp("good", 0.9)),
    #        ]),
    #    ('bernoulliesque door', lambda seed: [
    #        Bernoulli(ConstOp("bad", 0.2)),
    #        Bernoulli(ConstOp("bof", 0.5)),
    #        Bernoulli(DoorOp("door", 7000, 12000, 0.1, 0.8)),
    #        ]),
    }
    
random.seed(42)
duration = 20000

bw = False

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
        line.set_markevery(1000)

def setFigLinesBW(fig):
    """
    Take each axes in the figure, and for each line in the axes, make the
    line viewable in black and white.
    """
    for ax in fig.get_axes():
        setAxLinesBW(ax)

def plot_operators(ops, ax):
    time = range(0, duration)
    ax.set_title('Operator rewards over time w/o noise')

    for op in ops:
        ax.plot(time, [op.plotable_reward(t) for t in time], label=op.name)

    ax.set_xlabel('time')
    ax.set_ylabel('reward')
    ax.set_ylim([0,1])
    ax.legend(loc='best')
    
def plot_regrets(experiment_name, runs, ax):
    time = range(0, duration)
    ax.set_title('Cumulated regret (%s)' % experiment_name)

    regrets = defaultdict(list)
    for policies in runs:
        best_rewards = policies[0].rewards
        for p in policies[1:]:
            regrets[p.name].append(np.cumsum(np.subtract(best_rewards, p.rewards)))
    
    for p in runs[0][1:]:
        df = pd.DataFrame(regrets[p.name])
        ax.plot(time, df.T.mean(axis=1), label=p.name)
        # for event in p.events:
        #     for step in p.events[event]:
        #         ax.annotate(event, xy=(step, regrets[step]),
        #                     horizontalalignment='right',
        #                     xytext=(-50, 30), textcoords='offset points',
        #                     arrowprops=dict(arrowstyle="->"))

    ax.set_xlabel('time')
    ax.set_ylabel('regret')
    ax.legend(loc='best')


def plot(env, lambda_ops):
    fig = plt.figure()
    
    ops = lambda_ops()
    plot_operators(ops, fig.add_subplot(131))
        
    # Parallel test (as in uGP)
    def test(lamb, ax):
        runs = []
        for i in range(20):
            policies = make_policies(duration, lambda_ops())
            selected = {}
            for p in policies:
                for step in range(0, duration):
                    if step % lamb == 0:
                        p.before_select()
                    selected[step] = p.select(step)
                    if step % lamb == lamb - 1:
                        p.before_reward()
                        for step, op in selected.iteritems():
                            p.reward(step, op, op.reward(step))
                        selected = {}
            runs.append(policies)
        plot_regrets(env + ', $\lambda$=%i' % lamb, runs, ax)
        
    #test(1)
    test(20, fig.add_subplot(132))
    test(50, fig.add_subplot(133))
    #test(100)
    
    setFigLinesBW(fig)
    fig.show()
    
def show(name):
    for env, ops in environments.items():
        if str.lower(name) in str.lower(env):
            plot(env, ops)

print "Available environments:"
for env in environments.keys():
    print env
    
print ""
print "Call show('environment name')"
print ""

IPython.embed()
#show("noisy changing")
