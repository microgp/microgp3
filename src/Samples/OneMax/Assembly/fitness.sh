#!/bin/bash
#########################################################################
#                                                                       #
# This file is part of MicroGP v3 (ugp3)                                #
# http://ugp3.sourceforge.net/                                          #
#                                                                       #
#########################################################################
#                                                                       #
# This program is free software; you can redistribute it and/or modify  #
# it under the terms of the GNU General Public License as published by  #
# the Free Software Foundation, either version 3 of the License, or (at #
# your option) any later version.                                       #
#                                                                       #
# This program is distributed in the hope that it will be useful, but   #
# WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      #
# General Public License for more details                               #
#                                                                       #
#########################################################################
# $Revision: 643 $
# $Date: 2015-02-23 14:49:36 +0100 (Mon, 23 Feb 2015) $
#########################################################################

# Starting from v3.1.2_1142 fitness scripts can use (again) environment variables:
# $UGP3_FITNESS_FILE : the file created by the evaluator
# $UGP3_OFFSPRING    : the individuals to be evaluated (space separated list)
# $UGP3_GENERATION   : generation number
# $UGP3_VERSION      : current ugp3 version. eg. 3.1.2_1142
# $UGP3_TAGLINE      : full ugp3 tagline. eg. ugp3 (MicroGP++) v3.1.2_1142 "Bluebell"
[[ ! "$UGP3_FITNESS_FILE" ]] && UGP3_FITNESS_FILE=fitness.out

# Check if main.c was already compiled
[[ -f main.o ]] || gcc -O99 -c main.c

# Clean up directory
rm -f tprog error.log $UGP3_FITNESS_FILE

# Compile & execute
gcc -o tprog main.o $1 -lm 2>error.log
if [[ ! -f tprog ]]; then
    echo " "
    echo " "
    echo "PANIK! Can't compile the test program (tprog)"
    echo " "
    cat error.log
    exit
fi

# Get tprog output & number of lines
tprog=( $(./tprog) )
len=$(wc -l $1 | awk '{ print $1 }')
a=${tprog[0]}
(( b = 10000 - $len )); [[ $b < 0 ]] && b=0 # (paranoia)
note="0x${tprog[1]}/$len"

echo "$a $b $note" >$UGP3_FITNESS_FILE
