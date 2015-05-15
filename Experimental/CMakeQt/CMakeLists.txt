PROJECT(helloworld)

cmake_minimum_required(VERSION 2.6.0) 

FIND_PACKAGE(Qt4 REQUIRED)

# find and setup Qt4 for this project
find_package(Qt4 REQUIRED)

# include Qt4 cmake file
include(${QT_USE_FILE})

# add definitions needed for Qt4
add_definitions(${QT_DEFINITIONS})

SET(helloworld_SOURCES Main.cc MainWindow.cc)
SET(helloworld_HEADERS hellowindow.h)
SET(helloworld_FORMS MainWindow.ui)

QT4_WRAP_CPP(helloworld_HEADERS_MOC ${helloworld_HEADERS})
QT4_WRAP_UI(helloworld_FORMS_HEADERS ${helloworld_FORMS})

ADD_EXECUTABLE(helloworld ${helloworld_SOURCES} 
    ${helloworld_FORMS_HEADERS})

TARGET_LINK_LIBRARIES(helloworld ${QT_LIBRARIES})
