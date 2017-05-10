#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList                                                             #
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

project(ugp3-constraints-editor)

# this command finds Qt4 libraries and sets all required variables
find_package(Qt4 QUIET)

if(NOT QT4_FOUND)
  message(WARNING "Missing Qt. Graphical frontends won't be compiled.")
else(NOT QT4_FOUND)
  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by find_package( Qt4 ) that contains a path to CMake script)
  include(${QT_USE_FILE})

  # add definitions needed for Qt4
  add_definitions(${QT_DEFINITIONS})

  set(SOURCES 
    XmlHighlighter.cpp
    CodeEditor.cpp
    ConstraintsEditor.cpp
    main-constraints-editor.cpp)

  set(HEADERS 
   XmlHighlighter.h
   CodeEditor.h
   ConstraintsEditor.h
   MicroGPApplication.h)

  set(FORMS 
    ugp3-constraints.ui)

  QT4_WRAP_CPP(HEADERS_MOC 
    ${HEADERS})

  QT4_WRAP_UI(HEADERS_FORMS 
    ${FORMS})

  QT4_WRAP_CPP(MOC_SRCS 
   XmlHighlighter.h
   CodeEditor.h
   ConstraintsEditor.h
   MicroGPApplication.h)

  add_executable(ugp3-constraints-editor 
    ${SOURCES} 
    ${HEADERS_FORMS}
    ${MOC_SRCS}
  )

  target_link_libraries(ugp3-constraints-editor
    ${QT_LIBRARIES}
    -Wl,--start-group EvolutionaryCore Constraints Graph Log Shared XmlParser FrontendCommon -Wl,--end-group)
endif(NOT QT4_FOUND)
