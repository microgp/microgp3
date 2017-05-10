#!/bin/bash

# $UGP3_FITNESS_FILE : the file created by the evaluator
# $UGP3_OFFSPRING    : the individuals to be evaluated (space separated list)
# $UGP3_GENERATION   : generation number

group=$(echo $UGP3_OFFSPRING | tr " " "\n")

./evaluator --param parameters.txt --group $group --numberInFitness