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

#########################################################################
# CMAKE INTERNAL
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0005 NEW)

#########################################################################
# Name that identifies the project & version info
PROJECT(MicroGP)

#########################################################################
# VERSION_INFO
SET(VERSION_NAME "Camellia")
SET(VERSION "4")
SET(SUBVERSION "0")
# Check subversion
FIND_PACKAGE(Subversion QUIET)
IF(Subversion_FOUND)
  EXECUTE_PROCESS(COMMAND ${Subversion_SVN_EXECUTABLE} info ${ABS_FOLDER}
                  RESULT_VARIABLE INFO_RESULT
                  OUTPUT_VARIABLE IGNORED
                  ERROR_VARIABLE IGNORED)
  IF(INFO_RESULT EQUAL 0)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} MicroGP)
    SET(REVISION ${MicroGP_WC_REVISION})
  ENDIF(INFO_RESULT EQUAL 0)
ENDIF(Subversion_FOUND)
# Create MACROS
ADD_DEFINITIONS(-DUGP3_VERSION_NAME="${VERSION_NAME}")
ADD_DEFINITIONS(-DUGP3_VERSION="${VERSION}")
ADD_DEFINITIONS(-DUGP3_SUBVERSION="${SUBVERSION}")
ADD_DEFINITIONS(-DUGP3_REVISION="${REVISION}")
# Full tagline
IF(REVISION)
  SET(TAGLINE "v3.${VERSION}.${SUBVERSION}r${REVISION} (${VERSION_NAME})")
ELSE(REVISION)
  SET(TAGLINE "v3.${VERSION}.${SUBVERSION} (${VERSION_NAME})")
ENDIF(REVISION)
MESSAGE(STATUS "This is MicroGP " ${TAGLINE} )

#########################################################################
# Type of build
# (use -DCMAKE_INSTALL_PREFIX:PATH=/my/path to change installation dirs)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING
      "Choose the type of build, options are: DEBUG RELEASE RelWithDebInfo XWIN32"
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF( (NOT CMAKE_BUILD_TYPE STREQUAL DEBUG) 
    AND (NOT CMAKE_BUILD_TYPE STREQUAL RELEASE) 
    AND (NOT CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo) 
    AND (NOT CMAKE_BUILD_TYPE STREQUAL XWIN32) )
  MESSAGE(STATUS "Unknown CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
  MESSAGE(STATUS "Supported types: DEBUG RELEASE RelWithDebInfo XWIN32")
  MESSAGE(FATAL_ERROR "Aborting")
ENDIF((NOT CMAKE_BUILD_TYPE STREQUAL DEBUG) AND (NOT CMAKE_BUILD_TYPE STREQUAL RELEASE) AND (NOT CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo) AND (NOT CMAKE_BUILD_TYPE STREQUAL XWIN32) )

#########################################################################
# BUILDING WINDOZE EXECUTABLES FROM THE COMFORT OF LINUX?
IF(CMAKE_BUILD_TYPE STREQUAL XWIN32)
  SET(WIN32 TRUE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL XWIN32)

#########################################################################
# Static options
ADD_DEFINITIONS(-DUSE_STACK_TRACE)
ADD_DEFINITIONS(-DUGP3_ROOT_DIR="${PROJECT_SOURCE_DIR}")

#########################################################################
# Portability (termios)
INCLUDE(CheckIncludeFiles)
check_include_files (termios.h HAVE_TERMIOS_H)
IF(HAVE_TERMIOS_H)
  ADD_DEFINITIONS(-DUSE_TERMIOS)
ENDIF(HAVE_TERMIOS_H)

#########################################################################
# Portability (environment)
INCLUDE(CheckFunctionExists)
check_function_exists(putenv HAVE_PUTENV)
check_function_exists(setenv HAVE_SETENV)
check_function_exists(getenv HAVE_GETENV)
IF(HAVE_SETENV AND HAVE_GETENV)
  MESSAGE(STATUS "Using setenv & getenv")
  ADD_DEFINITIONS(-DUSE_SETENV -DUSE_GETENV)
ELSEIF(HAVE_PUTENV AND HAVE_GETENV)
  MESSAGE(STATUS "Using putenv & getenv")
  ADD_DEFINITIONS(-DUSE_PUTENV -DUSE_GETENV)
ELSEIF(HAVE_GETENV)
  MESSAGE(STATUS "Using only getenv")
  ADD_DEFINITIONS(-DUSE_GETENV)
ENDIF(HAVE_SETENV AND HAVE_GETENV)

#########################################################################
# Portability (32 vs. 64 bit -- yet another cmake stunt!)
INCLUDE(CheckTypeSize)
check_type_size("time_t" SIZEOF_TIME_T)
check_type_size("long" SIZEOF_LONG)
IF(NOT(SIZEOF_TIME_T EQUAL SIZEOF_LONG))
  MESSAGE(STATUS "Sizes of time_t and long differ (" ${SIZEOF_TIME_T} " vs. " ${SIZEOF_LONG} ")")
  ADD_DEFINITIONS(-DUSE_64BIT_TIME_T)
ENDIF(NOT(SIZEOF_TIME_T EQUAL SIZEOF_LONG))

#########################################################################
# C/C++ flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -ggdb -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG} -Wextra -Woverloaded-virtual -Wattributes -Wbuiltin-macro-redefined -Wc++0x-compat -Wcast-align -Wdiv-by-zero -Wdouble-promotion -Wenum-compare -Winit-self -Wint-to-pointer-cast -Wlogical-op -Wmissing-braces -Wmissing-field-initializers -Woverflow -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsign-compare -Wsign-promo -Wtype-limits -Wuninitialized -Wwrite-strings -Wno-unused-parameter -Wno-multichar -Wno-strict-aliasing")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG")

#INCLUDE(CheckCxxCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("-s" OPTIMIZE_SIZE)

#########################################################################
# BUILDING WIN32 EXECUTABLES
IF(WIN32)
  MESSAGE(STATUS "Yeuch, compiling for Win32")
  SET(CMAKE_EXECUTABLE_SUFFIX .exe)
  SET(CPACK_STRIP_FILES TRUE)
  check_include_files(windows.h HAVE_WINDOWS_H)
  IF(HAVE_WINDOWS_H)
    ADD_DEFINITIONS(-DWIN32_CONSOLE)
  ENDIF(HAVE_WINDOWS_H)
  SET(USE_RC TRUE)
  ADD_DEFINITIONS(-DUSE_FAKE_LOCK)
  ADD_DEFINITIONS(-DWINDOWS)
ENDIF(WIN32)

#########################################################################
# BUILDING WINDOZE EXECUTABLES FROM THE COMFORT OF LINUX?
IF(CMAKE_BUILD_TYPE STREQUAL XWIN32)
  # the name of the target operating system
  SET(CMAKE_SYSTEM_NAME Windows)
  SET(CMAKE_EXECUTABLE_SUFFIX .exe)
  SET(CPACK_STRIP_FILES TRUE)
  # which compilers to use for C AND C++
  SET(CMAKE_C_COMPILER   i686-w64-mingw32-gcc)
  SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
  SET(CMAKE_RC_COMPILER  i686-w64-mingw32-windres)
  SET(CMAKE_AR           i686-w64-mingw32-ar)
  SET(CMAKE_RANLIB       i686-w64-mingw32-ranlib)
  SET(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --target=pe-i386")
  # Update C/C++ flags
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -O99 -DNDEBUG")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  # Update defs
  REMOVE_DEFINITIONS(-DUSE_SETENV -DUSE_TERMIOS)
  ADD_DEFINITIONS(-DUSE_PUTENV -DUSE_GETENV)
  # adjust the default behaviour of the FIND_XXX() commands:
  # search headers AND libraries in the target environment, search 
  # programs in the host environment
  SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  
  # Forcing the use of Win32 console...
  ADD_DEFINITIONS(-DWIN32_CONSOLE)
  ADD_DEFINITIONS(-DUSE_FAKE_LOCK)
  # That's all...
  MESSAGE(STATUS "X-compiling (freedom through obsolescence)")

  SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> cr <TARGET> <LINK_FLAGS> <OBJECTS>")
  SET(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> r  <TARGET> <LINK_FLAGS> <OBJECTS>")
  SET(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
  SET(CMAKE_CXX_ARCHIVE_CREATE ${CMAKE_C_ARCHIVE_CREATE})
  SET(CMAKE_CXX_ARCHIVE_APPEND ${CMAKE_C_ARCHIVE_APPEND})
  SET(CMAKE_CXX_ARCHIVE_FINISH ${CMAKE_C_ARCHIVE_FINISH})
  
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
  SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

ENDIF(CMAKE_BUILD_TYPE STREQUAL XWIN32)

#########################################################################
# Random generator
SET(UGP3_USE_MERSENNE_TWISTER "ON" CACHE BOOL "Use the Mersenne Twister random number generator.")
IF (UGP3_USE_MERSENNE_TWISTER)
    ADD_DEFINITIONS(-DUSE_MERSENNE_TWISTER)
ENDIF (UGP3_USE_MERSENNE_TWISTER)

#########################################################################
# Operator Selection Testing
SET(UGP3_TEST_OPERATOR_SELECTION "OFF" CACHE BOOL "Test operator selection schemes.")
SET(UGP3_LINEAGE_RECURSION_DEPTH "1" CACHE STRING "Number of past generations to include in the lineage information.")
IF (UGP3_TEST_OPERATOR_SELECTION)
    ADD_DEFINITIONS(-DTEST_OPERATOR_SELECTION)
    ADD_DEFINITIONS(-DLINEAGE_RECURSION_DEPTH=${UGP3_LINEAGE_RECURSION_DEPTH})
ENDIF (UGP3_TEST_OPERATOR_SELECTION)

#########################################################################
# Header files
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR})

#########################################################################
# STRIP
IF(CMAKE_BUILD_TYPE STREQUAL RELEASE)
  MESSAGE(STATUS "Release build: stripping executables")
  SET(CPACK_STRIP_FILES TRUE)
ELSE(CMAKE_BUILD_TYPE STREQUAL RELEASE)
  SET(CPACK_STRIP_FILES FALSE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL RELEASE)

#########################################################################
# Header files
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Frontends/ugp3)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/Constraints)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/EvolutionaryCore)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/EvolutionaryCore/Operators)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/EvolutionaryCore/OperatorSelectors)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/Graph)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/Log)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/Shared)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/Shared/Exceptions)
INCLUDE_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs/XmlParser)
# Link
LINK_DIRECTORIES(${MicroGP_SOURCE_DIR}/Libs)
LINK_DIRECTORIES(${MicroGP_SOURCE_DIR}/Frontends)
# Subdirectories
ADD_SUBDIRECTORY(Libs)
ADD_SUBDIRECTORY(Frontends)
ADD_SUBDIRECTORY(Contrib)

#########################################################################
# STATUS
#MESSAGE(STATUS "Note: current build type: " ${CMAKE_BUILD_TYPE})
#MESSAGE(STATUS "Note install prefix: " ${CMAKE_INSTALL_PREFIX})
