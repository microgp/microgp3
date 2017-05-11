# 	CmakeLists for RobotObstacles
# 	Written by Alessio Moscatello
# 
#	This file is part of MicroGP v3 (ugp3) 
# 	https://github.com/squillero/microgp3        



 
# To generate Makefiles use the command: cmake .
# Cmake creates Makefile or project files depending on the OS in wich it's executed, see help of Cmake

# Tip:
# If you want to build ugp3 using Eclipse you can use the generator: -G"MinGW Makefiles" 
# that generates Makefiles suitable for MinGW; then you can import the project into Eclipse using the
# import command "Existing projects into Workspace"





#name that identifies the project
project(Evaluator)

#directories in wich compiler searches for header files

include_directories (${Evaluator_SOURCE_DIR})





#C++ flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable (Evaluator Evaluator.cpp)











