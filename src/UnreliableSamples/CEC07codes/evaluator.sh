#!/bin/bash


# choose one of the following functions
# format: ./evaluator <function_name> <input_dimensions> <output_dimensions> <files_to_evaluate>

# NOTE: the PARAMS and M values are the ones specified 
#       in the CEC 2007 experimental settings

# PARAMS is the number of input parameters (the size of the x input vector)
# M      is the number of objectives of the problem


./evaluator OKA2     3       2   $*

# PARAMS = 30
#./evaluator SYMPART  $PARAMS 2  $*

# PARAMS = 30
#./evaluator S_ZDT1   $PARAMS 2  $*

# PARAMS = 30
#./evaluator S_ZDT2   $PARAMS 2  $*

# PARAMS = 30
#./evaluator S_ZDT4   $PARAMS 2  $*

# PARAMS = 10
#./evaluator R_ZDT4   $PARAMS 2  $*

# PARAMS = 30
#./evaluator S_ZDT6   $PARAMS 2  $*

# PARAMS = 30
# M = 3 or 5
#./evaluator S_DTLZ2  $PARAMS $M $*

# PARAMS = 30
# M = 3 or 5
#./evaluator R_DTLZ2  $PARAMS $M $*

# PARAMS = 30
# M = 3 or 5
#./evaluator S_DTLZ3  $PARAMS $M $*

# PARAMS = 24 or 28
# M = 3 or 5
#./evaluator WFG1     $PARAMS $M $*

# PARAMS = 24 or 28
# M = 3 or 5
#./evaluator WFG8     $PARAMS $M $*

# PARAMS = 24 or 28
# M = 3 or 5
#./evaluator WFG9     $PARAMS $M $*

