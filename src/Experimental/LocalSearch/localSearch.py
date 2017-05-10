# Python script that replaces a card with Leeroy Jenkins
import os
import random
import sys

fileToBeRead = "individualsLocalSearch.txt"
outputFileName = "localSearch_offspring_0"
replacement = "    \"minion_leeroy_jenkins\",\n"

# read file
lines = []
with open(sys.argv[1], "r") as fp : lines = fp.readlines()

# replace random card
replaced = False
while not replaced :
	randomNumber = random.randrange(0, len(lines))
	#print "Now looking at line \"" + lines[randomNumber] + "\""
	if lines[randomNumber].find("minion_") > -1 or lines[randomNumber].find("spell_") > -1 :
		lines[randomNumber] = replacement
		replaced = True
		
# save file
with open(outputFileName, "w") as fp :
	for line in lines :
		fp.write(line)

# also write the name of the individual to the special file
with open(fileToBeRead, "w") as fp :
	fp.write(outputFileName + "\n")
