
import random
import math
from collections import deque
import numpy as np

# The reward must only depend on the given time step

MAX_DURATION = 30000

class Operator(object):
    def __init__(self, name):
        self.name = name
        
    def plotable_reward(self, time):
        return self.reward(time)
    
class ConstOp(Operator):
    def __init__(self, name, value):
        self.name = name
        self.value = value
        
    def reward(self, time):
        return self.value
    
class StepOp(Operator):
    def __init__(self, name, step_time, before, after):
        self.name = name
        self.step_time = step_time
        self.before = before
        self.after = after
        
    def reward(self, time):
        if time < self.step_time:
            return self.before
        else:
            return self.after
        
class DoorOp(Operator):
    def __init__(self, name, door_begin, door_end, outside, inside):
        self.name = name
        self.door_begin = door_begin
        self.door_end = door_end
        self.outside = outside
        self.inside = inside
        
    def reward(self, time):
        if self.door_begin < time < self.door_end:
            return self.inside
        else:
            return self.outside
        
class SlowStepOp(StepOp):
    def __init__(self, name, step_time, before, after, length):
        """
        Takes 2 times "length" to go from before to after
        """
        StepOp.__init__(self, name, step_time, before, after)
        k = 0.01
        self._lambda = math.log(1/k - 1)/length
        
    def reward(self, time):
        exp_arg = -self._lambda * (time - self.step_time)
        if exp_arg < -100:
            return self.after
        elif exp_arg < 100:
            return (self.after - self.before) * 1/(1 + math.exp(exp_arg)) + self.before
        else:
            return self.before
        
class SlowDoorOp(DoorOp):
    def __init__(self, name, door_begin, door_end, outside, inside, length):
        DoorOp.__init__(self, name, door_begin, door_end, outside, inside)
        max_length = (door_end - door_begin) / 2
        if length > max_length:
            print "SlowDoorOp: using length=%i instead of given %i" % (max_length, length)
            length = max_length
        self.step1 = SlowStepOp(name, door_begin, outside, inside, length)
        self.step2 = SlowStepOp(name, door_end, inside, outside, length)
        
        
    def reward(self, time):
        if time < (self.door_begin + self.door_end) / 2:
            return self.step1.reward(time)
        else:
            return self.step2.reward(time)

# TODO random must depend on time to compute regret
class BofBofWowOp(Operator):
    def __init__(self, name, low, high, pHigh):
        self.name = name
        self.values = [high if random.uniform(0, 1) < pHigh else low
                       for x in range(MAX_DURATION)]
        
    def reward(self, time):
        return self.values[time]
    
def clip(a, low, high):
    return max(min(a, high), low)

# Adds white noise to the results of another operator
class Noise(object):
    def __init__(self, op, sigma):
        self.name = op.name
        self.op = op
        self.normal = [random.normalvariate(0, sigma) for x in range(MAX_DURATION)]
        
    def reward(self, time):
        r = self.op.reward(time)
        return clip(r + self.normal[time], 0, 1)
    
    def plotable_reward(self, time):
        return self.op.reward(time)
    
# Moving mean over an interval
class Smooth(object):
    def __init__(self, op, window):
        self.name = op.name
        self.op = op
        self.values = []
        sample = deque([])
        for t in range(MAX_DURATION):
            sample.append(op.reward(t))
            if len(sample) > window:
                sample.popleft()
            self.values.append(np.mean(sample))
    
    def reward(self, time):
        return self.values[time]
    
# Quantize the reward to {0,1} using the reward of another operator
# as the probability of getting 1
class Bernoulli(object):
    def __init__(self, op):
        self.name = op.name
        self.op = op
        self.values = [random.uniform(0, 1) for x in range(MAX_DURATION)]
        
    def reward(self, time):
        r = self.op.reward(time)
        return 1 if self.values[time] < r else 0