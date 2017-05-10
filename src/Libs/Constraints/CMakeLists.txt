#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList for libConstraints                                          #
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

ADD_LIBRARY(Constraints
  BitArrayParameter.cc
  BitArrayParameter.xml.cc
  CombinatorialParameter.cc
  CombinatorialParameter.xml.cc
  ConstantParameter.cc
  ConstantParameter.xml.cc
  ConstrainingElement.cc
  ConstrainingElement.xml.cc
  Constraints.cc
  Constraints.xml.cc
  DataParameter.cc
  EnvironmentParameter.cc
  EnvironmentParameter.xml.cc
  Epilogue.cc
  Expression.cc
  ExpressionElement.cc
  Expression.xml.cc
  FloatParameter.cc
  FloatParameter.xml.cc
  Format.cc
  Format.xml.cc
  GenericMacro.cc
  GenericMacro.xml.cc
  GenericSection.cc
  GenericSection.xml.cc
  InnerLabelParameter.cc
  InnerLabelParameter.xml.cc
  IntegerParameter.cc
  IntegerParameter.xml.cc
  Macro.cc
  Macro.xml.cc
### Experimental stuff
#  MacroParameter.cc
#  MacroParameter.xml.cc
### Experimental stuff
  OuterLabelParameter.cc
  OuterLabelParameter.xml.cc
  Parameter.cc
  Parameter.xml.cc
  Prologue.cc
  Section.cc
  Section.xml.cc
  SelfRefParameter.cc
  SelfRefParameter.xml.cc
  StructuralParameter.cc
  SubSection.cc
  SubSection.xml.cc
  UniqueTagParameter.cc
  UniqueTagParameter.xml.cc)

#INSTALL(TARGETS Constraints
#  LIBRARY DESTINATION lib
#)
