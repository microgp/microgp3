#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList for libShared                                               #
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

ADD_LIBRARY(Shared 
  Base.cc 
  Convert.cc 
  drand48.cc 
  Environment.cc 
  Exception.cc 
  File.cc 
  InfinityString.cc 
  Info.cc
  IString.cc 
  LineInformation.cc 
  Option.cc 
  Option.xml.cc 
  Random.cc 
  RegexMatch.cc
  Settings.cc 
  SettingsContext.cc 
  SettingsContext.xml.cc 
  Settings.xml.cc 
  StackTrace.cc 
  Tag.cc 
  Taggable.cc 
  UniqueIdCounter.cc 
  Utility.cc 
  XMLIFace.cc)

#INSTALL(TARGETS Shared
#  LIBRARY DESTINATION lib
#)
