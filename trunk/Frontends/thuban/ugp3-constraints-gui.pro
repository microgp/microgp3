######################################################################
# Automatically generated by qmake (2.01a) Mon Oct 1 21:57:46 2012
######################################################################

TEMPLATE = app

TARGET = ugp3-constraints-gui

QT += widgets

FLAGS += -std=c++11

DEPENDPATH += 	. \ 
		../../ \
		../../Libs/Shared \
		../../Libs/Log \
		../../Libs/XmlParser \
		../../Libs/Graph \
		../../Libs/Shared/Exceptions \
		../../Libs/Shared \
		../../Libs/Log \
		../../Libs/XmlParser/ \
		../../Libs/Graph \
		../../Libs/Constraints \
		../../Libs/EvolutionaryCore

INCLUDEPATH += 	. \
		../../ \
		../../Libs/Shared \
		../../Libs/Log \
		../../Libs/XmlParser \
		../../Libs/Graph \
		../../Libs/Constraints \
		../../Libs/EvolutionaryCore

LIBS +=		-L../../Libs/XmlParser/ \
		-L../../Libs/Shared/ \
		-L../../Libs/Log/ \
		-L../../Libs/Graph/ \
		-L../../Libs/Constraints/ \
		-L../ugp3/ \
		-L../../Libs/EvolutionaryCore/ \
		-Wl,--start-group ../../Libs/EvolutionaryCore/libEvolutionaryCore.a ../ugp3/libFrontendCommon.a ../../Libs/Constraints/libConstraints.a ../../Libs/Graph/libGraph.a ../../Libs/XmlParser/libXmlParser.a ../../Libs/Shared/libShared.a ../../Libs/Log/libLog.a -Wl,--end-group
		
# Input

FORMS += 	ugp3-constraints.ui

HEADERS =	XmlHighlighter.h \
		CodeEditor.h \
		ConstraintsEditor.h \
		MicroGPApplication.h

SOURCES = 	XmlHighlighter.cpp \
		CodeEditor.cpp \
		ConstraintsEditor.cpp \
		main-constraints-editor.cpp
