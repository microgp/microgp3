
import math
import random
from collections import defaultdict
from collections import deque
import numpy as np

MAX_DURATION = 20000

class Policy(object):
    def set_duration(self, duration):
        self.rewards = [None] * duration
        self.selected = [None] * duration
        self.events = defaultdict(list)
        
    def before_select(self):
        pass
    
    def before_reward(self):
        pass
    
    def event(self, step, event):
        self.events[event].append(step)
        
    def reward(self, step, op, reward):
        self.selected[step] = op
        self.rewards[step] = reward

class Reference(Policy):
    def __init__(self, ops):
        self.name = 'reference'
        self.ops = ops
    
    def select(self, time):
        # Select the best reward
        return max(self.ops, key=lambda op: op.reward(time))
    
class Random(Policy):
    def __init__(self, ops):
        self.name = 'random'
        self.ops = ops
    
    def select(self, time):
        # Select a random operator
        return random.choice(self.ops)
    

class DMAB(Policy):
    def __init__(self, ops):
        # PH reset trigger level (generally from 0.5 to 20)
        # Original value: 0.8
        self.PHlambda = 0.8;
        # Enforces PH robustness when dealing with slow changing environments
        self.PHdelta = 0.15;
        # Exploration level
        self.C = 1.4;
        self.name = 'DMAB'
        self.totExecutions = 0
        self.data = {op.name: {'op': op} for op in ops}
        self.reset()
    
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        
    def select(self, time):
        selected = None;
        if any(d['nExecutions'] == 0 for d in self.data.itervalues()):
            selected = min(self.data.keys(), key=lambda name: self.data[name]['nExecutions'] + self.data[name]['nPendingExecutions'])
        else:
            scores = {name: d['meanReward'] + self.C * math.sqrt(math.log(self.totExecutions) / d['nExecutions'])
                      for name, d in self.data.iteritems()}
            selected = max(self.data.keys(), key=lambda name: scores[name])
        self.data[selected]['nPendingExecutions'] += 1
        return self.data[selected]['op']
    
    def reward(self, step, op, reward):
        Policy.reward(self, step, op, reward)
        self.totExecutions += 1
        should_reset = self.actual_reward(step, op, reward)
        if should_reset:
            self.event(step, "reset")
            self.reset()
        
    def actual_reward(self, step, op, reward):
        d = self.data[op.name]
        d['meanReward'] = (d['meanReward'] * d['nExecutions'] + reward) / (d['nExecutions'] + 1);
        d['nExecutions'] += 1
        d['meanDeviation'] = d['meanDeviation'] + (d['meanReward'] - reward + self.PHdelta);
        d['maxDeviation'] = max(d['maxDeviation'], d['meanDeviation']);
        return d['maxDeviation'] - d['meanDeviation'] > self.PHlambda
    
    def reset(self):
        self.totExecutions = 0
        for d in self.data.itervalues():
            self.reset_one(d)
    
    def reset_one(self, d):
        d['nExecutions'] = 0
        d['meanReward'] = 0
        d['meanDeviation'] = 0
        d['maxDeviation'] = 0
        
class DMABLR(DMAB):
    def __init__(self, ops):
        DMAB.__init__(self, ops)
        self.name = "DMAB-LR"
        
    def reward(self, step, op, reward):
        Policy.reward(self, step, op, reward)
        self.totExecutions += 1
        should_reset = self.actual_reward(step, op, reward)
        if should_reset:
            # Reset only this operator
            self.event(step, "reset " + op.name)
            d = self.data[op.name]
            self.totExecutions -= d['nExecutions']
            self.reset_one(d)
                
class PDMAB(DMAB):
    def __init__(self, ops):
        DMAB.__init__(self, ops)
        self.name = 'PDMAB'
    
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        self.totPendingExecutions = 0;
    
    def select(self, time):
        selected = None;
        if any(d['nExecutions'] == 0 for d in self.data.itervalues()):
            selected = min(self.data.keys(), key=lambda name: self.data[name]['nExecutions'] + self.data[name]['nPendingExecutions'])
        else:
            scores = {name: d['meanReward'] + self.C * math.sqrt(math.log(self.totExecutions + self.totPendingExecutions) / (d['nExecutions'] + d['nPendingExecutions']))
                      for name, d in self.data.iteritems()}
            selected = max(self.data.keys(), key=lambda name: scores[name])
        self.data[selected]['nPendingExecutions'] += 1
        self.totPendingExecutions += 1
        return self.data[selected]['op']
            
class PDMABLR(PDMAB):
    def __init__(self, ops):
        PDMAB.__init__(self, ops)
        self.name = "PDMAB-LR"
        
    def reward(self, step, op, reward):
        Policy.reward(self, step, op, reward)
        self.totExecutions += 1
        should_reset = self.actual_reward(step, op, reward)
        if should_reset:
            self.event(step, "reset " + op.name)
            d = self.data[op.name]
            self.totExecutions -= d['nExecutions']
            self.reset_one(d)
            
# Roulette DMAB with standard UCB1 formula
class RDMAB(DMAB):
    def __init__(self, ops):
        DMAB.__init__(self, ops)
        self.name = 'RDMAB'
        self.values = [random.uniform(0, 1) for x in range(MAX_DURATION)]
        
    def wheelcoeff(self, d):
        return d['meanReward'] + self.C * math.sqrt(math.log(self.totExecutions) / d['nExecutions'])
    
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        self.wheel = [
            (name, self.wheelcoeff(d))
            for name, d in sorted(self.data.iteritems()) if d['nExecutions'] != 0
            ]
        self.wheeltot = 0
        for name, score in self.wheel:
            self.wheeltot += score
        
    def select(self, time):
        selected = None
        if any(d['nExecutions'] == 0 for d in self.data.itervalues()):
            selected = min(self.data.keys(), key=lambda name: self.data[name]['nExecutions'] + self.data[name]['nPendingExecutions'])
        else:
            # Roulette-wheel selection
            rand = self.values[time] * self.wheeltot
            for name, score in self.wheel:
                if score >= rand:
                    selected = name
                    break
                else:
                    rand -= score
        self.data[selected]['nPendingExecutions'] += 1
        return self.data[selected]['op']

# Roulette DMAB with exponentiated scores
# Kappa = 0 -> random
# Kappa = 1 -> RDMAB
# Kappa = inf -> DMAB
class KRDMAB(RDMAB):
    def __init__(self, ops, kappa):
        RDMAB.__init__(self, ops)
        self.kappa = kappa
        self.name = "KRDMAB %i" % kappa
        
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        self.wheel = [
            (name, self.wheelcoeff(d))
            for name, d in sorted(self.data.iteritems()) if d['nExecutions'] != 0 and self.wheelcoeff(d) > 0
            ]
        self.wheeltot = 0
        if self.wheel:
            # normalize: best coeff = 1 then exponentiate
            maxcoeff = max(self.wheel, key=lambda tuple: tuple[1])[1]
            self.wheel = [
                (name, math.pow(score / maxcoeff, self.kappa))
                for name, score in self.wheel
                ]
            # sum all coeffs
            for name, score in self.wheel:
                self.wheeltot += score
                
class KRDMABLR(KRDMAB):
    def __init__(self, ops, kappa):
        KRDMAB.__init__(self, ops, kappa)
        self.name = "KRDMAB-LR %i" % kappa
        
    def reward(self, step, op, reward):
        Policy.reward(self, step, op, reward)
        self.totExecutions += 1
        should_reset = self.actual_reward(step, op, reward)
        if should_reset:
            self.event(step, "reset " + op.name)
            d = self.data[op.name]
            self.totExecutions -= d['nExecutions']
            self.reset_one(d)

# Sliding window MAB, parallel-enabled
class SMAB(Policy):
    def __init__(self, ops):
        self.name = 'SMAB'
        self.totExecutions = 0
        self.data = {op.name: {
            'op': op,
            'nExecutions': 0,
            'rewards': deque(maxlen=10),
        } for op in ops}
    
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        
    def select(self, time):
        scores = {name: 1000 - d['nPendingExecutions'] if d['nExecutions'] == 0 else 
                  np.mean(d['rewards']) + self.C * math.sqrt(math.log(self.totExecutions) / (d['nExecutions'] + d['nPendingExecutions']))
                  for name, d in self.data.iteritems()}
        selected = max(self.data.keys(), key=lambda name: scores[name])
        self.totExecutions += 1
        self.data[selected]['nPendingExecutions'] += 1
        return self.data[selected]['op']
    
    def reward(self, step, op, reward):
        Policy.reward(self, step, op, reward)
        self.actual_reward(step, op, reward)
        
    def actual_reward(self, step, op, reward):
        d = self.data[op.name]
        d['rewards'].append(reward);
        d['nExecutions'] += 1
    
# Sorted roulette SMAB with beta distribution
class RSMAB(SMAB):
    def __init__(self, ops, beta):
        SMAB.__init__(self, ops)
        self.name = "RSMAB"
        # beta=1 -> normal roulette, beta=+inf -> UCB1 MAB, example value: 5
        self.beta = beta
        self.values = [random.betavariate(1, self.beta) for x in range(MAX_DURATION)]
        
    def wheelcoeff(self, d):
        # UCB1 formula
        return np.mean(d['rewards']) + self.C * math.sqrt(math.log(self.totExecutions + 1) / d['nExecutions'])
    
    def before_select(self):
        for d in self.data.itervalues():
            d['nPendingExecutions'] = 0
        self.wheel = sorted([
            (name, self.wheelcoeff(d))
            for name, d in self.data.iteritems() if d['nExecutions'] != 0
            ], key=lambda tuple: tuple[1], reverse=True)
        self.wheeltot = 0
        for name, score in self.wheel:
            self.wheeltot += score
        
    def select(self, time):
        selected = None
        for name, d in self.data.iteritems():
            # Run once all operators
            if d['nExecutions'] + d['nPendingExecutions'] == 0:
                selected = name
                break
        if not selected:
            if self.wheel:
                # Roulette-wheel selection
                rand = self.values[time] * self.wheeltot
                for name, score in self.wheel:
                    if score >= rand:
                        selected = name
                        break
                    else:
                        rand -= score
            else:
                # Run again the operator we already ran the least
                selected_data = min(self.data.values(), key=lambda d: d['nExecutions'] + d['nPendingExecutions'])
                selected = selected_data['op'].name
        self.totExecutions += 1
        self.data[selected]['nPendingExecutions'] += 1
        return self.data[selected]['op']