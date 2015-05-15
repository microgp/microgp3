# Simple Python script for running comparative multi-objective experiments
# by Alberto Tonda, 2014 <alberto.tonda@gmail.com>

# TODOs
# - read the name of the population from ugp3.settings.xml

import numpy as np
import matplotlib
import matplotlib.pyplot as Plt
import os
import sympy
import sys
import time
import xml.etree.ElementTree as ElementTree

from textwrap import wrap
from sympy import sympify
from sympy import Symbol

# some variables that will be used during the experiments
numberOfExperiments = 5 
directoryName = "../zdt2"

nsga2 = "nsga-ii"
nsga2directory = "../../NSGA-II/"
nsga2csvfile = "nsga2-points.csv"
nsga2outputfile = "nsga2-output.csv"

ugp3 = "ugp3"
ugp3directory = "../../../Frontends/ugp3/"
ugp3csvfile = "ugp3-points.csv"

minimizeOption = "--minimize"
maximizeOption = "--maximize"

# disclaimer
print "Python script to run experiments, by Alberto Tonda, 2014 <alberto.tonda@gmail.com>"
print "WARNING: in order for this script to work, you have to prepare a directory called <testFunction>, with a C++ file inside called <testFunction>.cpp ; your test function MUST incorporate the following command-line options: " + minimizeOption + " and " + maximizeOption + ", to return the appropriate fitness values for a min/max problem. Other files in the directory shall include a ugp3.settings.xml file, an appropriate constraints file for your problem, and a population file called moea-population.xml"

# so, first of all get the name of the current directory
os.chdir(directoryName)
fullDirectoryName = os.getcwd()
functionName = fullDirectoryName.split('/')[-1] # take the last token after splitting the full file name with '/'

# debug
print "Now about to run an experiment in directory \"" + functionName + "\"..."

# compile the function file
functionFileName = functionName + ".cpp"
sys.stdout.write("Compiling the function file...")
os.system("g++ " + functionFileName + " -o " + functionName)
sys.stdout.write("done.\n")

# find the Pareto front text file
symbolicParetoFront = None
sys.stdout.write("Looking for a Pareto front formula file...")
paretoFrontFile = functionName + ".txt"
if os.path.isfile(paretoFrontFile) :
	# read the file and get the formula
	with open(paretoFrontFile, "r") as fp : symbolicParetoFront = sympify( fp.readlines()[0] )
	sys.stdout.write("found: y = " + str(symbolicParetoFront) + "\n")
else :
	sys.stdout.write("not found.\n")

# find ugp3
sys.stdout.write("Locating ugp3...")
if os.path.isfile(ugp3directory + ugp3) :
	sys.stdout.write("found.\n")
else :
	sys.stdout.write("not found. Aborting...\n")
	sys.exit(0)
ugp3fulldirectory = os.path.abspath(ugp3directory)
	
# find nsga2
sys.stdout.write("Locating nsga2...")
if os.path.isfile(nsga2directory + nsga2) :
	sys.stdout.write("found.\n")
else :
	if os.path.isfile(nsga2directory) :
		# try to compile!
		sys.stdout.write("compiling...")
		os.chdir(nsga2directory)
		os.system("qmake")
		os.system("make")
		
		if os.path.isfile(nsga2) :
			sys.stdout.write("found.\n")
			os.chdir(fullDirectoryName)
		else :
			sys.stdout.write("compiling failed. Aborting...\n")
			sys.exit(0)
	else :
		sys.stdout.write("not found. Aborting...\n")
		sys.exit(0)
nsga2fulldirectory = os.path.abspath(nsga2directory)

# this call to the time function will be used to name the directories
tempDirBase = time.strftime("%Y-%m-%d-%H-%M-%S")

# run a certain number of experiments with ugp3 and collect the best points
# create temporary directory
ugp3tempdir = tempDirBase + "-" + ugp3
os.system("mkdir " + ugp3tempdir)
os.chdir(ugp3tempdir)

# copy all the necessary files
os.system("cp " + fullDirectoryName + "/*xml .")
os.system("cp " + fullDirectoryName + "/" + functionName + " .")
os.system("cp " + ugp3fulldirectory + "/" + ugp3 + " .")

# read the XML population file
tree = ElementTree.parse("moea-population.xml")
root = tree.getroot()

# read the number of dimensions and the name of the output file
fitnessDimension = int( root.find("fitnessParameters").get("value") )
fitnessFileName = root.find("evaluation/evaluatorOutputPathName").get("value")

print "We have " + str(fitnessDimension) + " dimensions, and the output fitness file name is \"" + fitnessFileName + "\"."

# find the "evaluatorPathName" node and check that the problem is a MAXIMIZATION problem! 
evaluatorNode = root.find("evaluation/evaluatorPathName")
evaluatorPath = evaluatorNode.get("value")

if minimizeOption in evaluatorPath :
	evaluatorPath = evaluatorPath.replace(minimizeOption, maximizeOption)
	evaluatorNode.set("value", evaluatorPath)
	tree.write("moea-population.xml")

# also, keep track of some other variables that might be useful later
popSize = root.find("mu").get("value")
offspringSize = root.find("lambda").get("value")
maxEvaluations = root.find("maximumEvaluations").get("value")
inertia = root.find("inertia").get("value")
sigma = root.find("sigma").get("value")

# start the ugp3 experiments!
ugp3points = []
for experiment in range(0, numberOfExperiments) :
	print "Running experiment " + str(experiment) + "..."
	os.system("./ugp3")
	
	# now, collect all the points 
	print "Collecting points..."
	files = [ x for x in os.listdir(os.getcwd()) if x.startswith("PF_") ] # filter for appropriate file type
	
	print "Points collected:", files

	# for every file, store its values and run the optimization to get its fitness
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

		# store individual in points
		print "Point \"" + individual + "\" is:", point
		ugp3points.append(point)
	
	# remove all temporary files
	files.append("status.xml")
	files.append("statistics.csv")
	for item in files : os.system("rm " + item)

# write points into .csv file
with open(ugp3csvfile, "w") as fp :
	for i in range(0, len(ugp3points[0]) - fitnessDimension) :
		fp.write("dimension" + str(i) + ",")
	for i in range(0, fitnessDimension) :
		fp.write("fitness" + str(i) )
		if i == fitnessDimension - 1 : fp.write("\n")
		else : fp.write(",")
	
	for point in ugp3points :
		for i in range(0, len(point)) :
			fp.write( str(point[i]) )
			if i != len(point) - 1 : fp.write(",")
			else : fp.write("\n")

# remove ugp3 file
os.system("rm ugp3")

# run a certain number of experiments with nsga2 and collect the best points
# create the directory and copy all the necessary files
os.chdir("..")
nsga2tempdir = tempDirBase + "-" + nsga2 
os.system("mkdir " + nsga2tempdir)
os.chdir(nsga2tempdir)

os.system("cp " + fullDirectoryName + "/*xml .")
os.system("cp " + fullDirectoryName + "/" + functionName + " .")
print "About to execute \"cp " + nsga2fulldirectory + "/" + nsga2 + " .\""
os.system("cp " + nsga2fulldirectory + "/" + nsga2 + " .")

# read the XML population file
tree = ElementTree.parse("moea-population.xml")
root = tree.getroot()

# verify that the problem is correctly set as a MINIMIZATION problem
evaluatorNode = root.find("evaluation/evaluatorPathName")
evaluatorPath = evaluatorNode.get("value")

if minimizeOption not in evaluatorPath :
	evaluatorPath = evaluatorPath.replace(maximizeOption, minimizeOption)
	evaluatorNode.set("value", evaluatorPath)
	tree.write("moea-population.xml")

# TODO check whether it's possible to also have the coordinates of the nsga2 points, and not just the fitness values
# start the nsga2 experiments!
nsga2points = []

for experiment in range(0, numberOfExperiments) :
	print "Running experiment " + str(experiment) + "..."
	os.system("./" + nsga2)
	
	# now, collect all the points 
	print "Collecting points..."
	with open(nsga2outputfile, "r") as fp : lines = fp.readlines()

	# debug
	print "There are a total of " + str(len(lines)) + " points found by NSGA-II"
	# the file should be just space-separated numbers, with the N fitnesses
	for line in lines :
		fitnesses = [x for x in line.split() if len(x) > 0]
		nsga2points.append(fitnesses)

	# remove temporary files
	os.system("rm " + nsga2outputfile)

# remove other temporary file
os.system("rm " + nsga2)	

# write the nsga2 points to file
with open(nsga2csvfile, "w") as fp :
	
	for i in range(0, fitnessDimension) :
		fp.write( "fitness" + str(i) )
		if i == fitnessDimension-1 : fp.write("\n")
		else : fp.write(",")
	
	for point in nsga2points :
		for i in range(0, len(point)) :
			fp.write( point[i] )
			if i == len(point)-1 : fp.write("\n")
			else: fp.write(",")

# now, plot the points!
# TODO here there is the STRONG ASSUMPTION that we only have 2 fitness values...

# for ugp3 points, take only the last "fitnessDimension" elements
ugp3fitnesspoints = []
for point in ugp3points : 
	print "ugp3point = ", point
	fitnessPoint = [ point[i] for i in [ len(point)-1, len(point)-2] ] 
	ugp3fitnesspoints.append( fitnessPoint ) 

# TODO also select the points for nsga2 (at the moment it's not necessary, only the fitness values are stored)


# this is debugging
print "ugp3fitnesspoints=", ugp3fitnesspoints

# also, select the fitnesses in-order
ugp3fitness0 = [ x[0] for x in ugp3fitnesspoints ]
ugp3fitness1 = [ x[1] for x in ugp3fitnesspoints ]
nsga2fitness0 = [ x[0] for x in nsga2points ] 
nsga2fitness1 = [ x[1] for x in nsga2points ] 

fig = Plt.figure(1)

# if the theoretical Pareto front is there, plot it!
if symbolicParetoFront != None :
	# sample one dimension
	xValues = np.arange(0, 1, 0.001)
	# evaluate the symbolic function, replacing symbol 'x' with the values
	y = [ symbolicParetoFront.subs( Symbol("x"), x ) for x in xValues ]
	# plot as a line
	Plt.plot(xValues, y, 'k-')

# plot the points
Plt.plot(ugp3fitness1, ugp3fitness0, 'gx') # for some reason, fitness0 and fitness1 are inverted?
Plt.plot(nsga2fitness0, nsga2fitness1, 'r.') 

# now, the title is quite complex
figTitle = "ugp3 (" + str(len(ugp3fitness0)) + " points) vs nsga2 (" + str(len(nsga2fitness0)) + " points), " + str(numberOfExperiments) + " runs, mu=" + popSize + ", lambda=" + offspringSize + ", eval=" + maxEvaluations + ", sigma=" + sigma + ", inertia=" + inertia
Plt.title("\n".join(wrap(figTitle,60)))

# axis labels
Plt.xlabel("fitness 0")
Plt.ylabel("fitness 1")

Plt.show()

# change back to the mother directory
os.chdir(fullDirectoryName) 
# the filename is composed by the date and the name of the function
pdfFileName = functionName + "-" + tempDirBase + ".pdf"
fig.savefig(pdfFileName)
