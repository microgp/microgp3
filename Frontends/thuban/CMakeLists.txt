#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList                                                             #
#                                                                       #
# This file is part of MicroGP v3 (ugp3)                                #
# http://ugp3.sourceforge.net/                                          #
#                                                                       #
# Copyright (c) 2002-2015 Giovanni Squillero                            #
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

project(qtgui)

# this command finds Qt4 libraries and sets all required variables
find_package(Qt4 QUIET)

if(NOT QT4_FOUND)
  message(WARNING "Missing Qt4. Graphical frontends won't be compiled.")
else(NOT QT4_FOUND)
  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by find_package( Qt4 ) that contains a path to CMake script)
  include(${QT_USE_FILE})

  # add definitions needed for Qt4
  add_definitions(${QT_DEFINITIONS})

  set(SOURCES 
    main.cpp
    CodeEditor.cpp
    MicroGPMainWindow.Constraints.cpp
    MicroGPMainWindow.Evolution.cpp
    MicroGPMainWindow.Population.cpp
    MicroGPMainWindow.cpp
    MicroGPSelectOperators.cpp
    XmlHighlighter.cpp)

  set(HEADERS 
    CodeEditor.h 
    MicroGPApplication.h 
    MicroGPMainWindow.h 
    MicroGPSelectOperators.h 
    XmlHighlighter.h)

  set(FORMS 
    ugp3-selectOperators.ui 
    ugp3.ui)

  QT4_WRAP_CPP(HEADERS_MOC 
    ${HEADERS})

  QT4_WRAP_UI(HEADERS_FORMS 
    ${FORMS})

  QT4_WRAP_CPP(MOC_SRCS 
    XmlHighlighter.h
    CodeEditor.h
    MicroGPSelectOperators.h
    MicroGPMainWindow.h)

  add_executable(qtgui 
    ${SOURCES} 
    ${HEADERS_FORMS}
    ${MOC_SRCS}
  )

  target_link_libraries(qtgui 
    ${QT_LIBRARIES}
    EvolutionaryCore Constraints Graph Log Shared XmlParser)
endif(NOT QT4_FOUND)