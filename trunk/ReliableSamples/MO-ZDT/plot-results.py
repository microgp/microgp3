# Simple Python script to plot the results of MO-ZDT2
# by Alberto Tonda, 2015 <alberto.tonda@gmail.com>

import numpy as np
import matplotlib.pyplot as plot
import os
import sys

# some hard-coded values
functionName = "zdt2"
fitnessFileName = "fitness.output"


# TODO: check if ugp3-extractor is installed and/or in the "expected" relative folder

# gather all the individuals
files = [ x for x in os.listdir(os.getcwd()) if x.startswith("PF_") ] # filter for appropriate file type

# if there are no individuals, it's a problem!
print "Gathering individuals on the PF..."
if len(files) == 0 :
	sys.stderr("Error: no individual files found in current folder.\n")
	sys.exit(0)

# get the points and MOST IMPORTANTLY their fitness values
print "Evaluating individuals..."
ugp3points = []
for individual in files :
	point = []
	with open(individual, "r") as fp :
		data = fp.read().replace('\n', ' ') # now all numbers are on the same line
		point = [ float(x) for x in data.split(' ') if len(x) > 0 ]
	
	# run the function and get the REAL fitness value
	os.system("./" + functionName + " --minimize --individual " + individual)

	with open(fitnessFileName, "r") as fp :
		data = fp.read().replace('\n', ' ') # now all numbers are on the same line
		point += [ float(x) for x in data.split(' ') if len(x) > 0 ]

	# store fitness values 
	#print "Point \"" + individual + "\" is:", point
	ugp3points.append( [point[-2], point[-1]] )

ugp3fitness0 = [ x[0] for x in ugp3points ]
ugp3fitness1 = [ x[1] for x in ugp3points ]

# create a figure with
# a - the theoretical Pareto front (green line)
# b - the points found by ugp3 (red crosses)
print "Plotting graph..."
x1 = np.arange(0,1,0.0001)
x2 = [ (1 - x**2) for x in x1 ]

fig = plot.figure()
ax = fig.add_subplot(111)

plot.plot(x1, x2, 'g-', label='Theoretical Pareto front')
plot.plot(ugp3fitness0, ugp3fitness1, 'rx', label='Points found by ugp3')
ax.legend(loc='best')

plot.savefig("results.pdf")
plot.close(fig)
