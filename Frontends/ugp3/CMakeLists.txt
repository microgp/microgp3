#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList for MicroGP++ frontend                                      #
#                                                                       #
# This file is part of MicroGP v3 (ugp3)                                #
# http://ugp3.sourceforge.net/                                          #
#                                                                       #
# Copyright (c) 2002-2016 Giovanni Squillero                            #
#                                                                       #
#-----------------------------------------------------------------------#
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
# $Revision: 644 $
# $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
#########################################################################

IF(WIN32)
  MESSAGE(STATUS "Compiling under windoze, adding resources")
  ADD_DEFINITIONS(-DRC_VERSION_STR="3.${VERSION}.${SUBVERSION}")
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  ADD_EXECUTABLE(ugp3 
    Main.cc Program.cc Program.Run.cc Argument.cc MicroGPSettings.cc ugp3.rc)
ELSE(USE_RC)
  ADD_EXECUTABLE(ugp3 
    Main.cc Program.cc Program.Run.cc Argument.cc MicroGPSettings.cc)
ENDIF(WIN32)

ADD_LIBRARY(FrontendCommon
  Argument.cc MicroGPSettings.cc)

TARGET_LINK_LIBRARIES(ugp3
 EvolutionaryCore Constraints Graph Log Shared XmlParser)

INSTALL(PROGRAMS ugp3 DESTINATION bin)
