	This file is part of MicroGP v3 (ugp3) 
 	https://github.com/squillero/microgp3         


This example simulates a simple robot that can only rotate or move straight

> Files:
  ugp3.settings.xml:  contains basic settings for ugp
  population.settings.xml: contains settings for the population to evolve
  constraints.xml: contains the "instructions" to generate the individuals
  constrainsScripted.xslt: this is a stylesheet used to display constraints.xml in a browser
  
  Evaluator.cpp: this is the evaluator that evaluates the behavior of the robot




> How to build the evaluator:
If you have cmake on your system, perform this command:

Linux:
  *cmake .
  *make

Other OS in wich is avaiable MinGW:
  *cmake -G"MinGW Makefiles" . 
  *make




> How to execute this example:
  Simply run ugp3

Don't forget to delete whole_fitness.txt at every new simulation (every evaluated fitness is appended to it)!!



> How to simulate a single individual:
  *Evaluator simulate individual.txt

The individual coordinates will be stored in coordinates.txt






Last update 22/02/2008, Alessio Moscatello