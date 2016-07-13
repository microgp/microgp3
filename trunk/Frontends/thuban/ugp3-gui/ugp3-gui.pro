######################################################################
# Manually generated by Alberto Tonda, 2015 
# Important note: Qwt (6.0+) has to be installed
######################################################################

TEMPLATE = app

TARGET = ugp3-gui

QT += widgets

CONFIG += qwt

QMAKE_CXXFLAGS += -std=c++11 -g

DEPENDPATH += 	../. \ 
		../../../ \
		../../ugp3/ \
		../../../Libs/Constraints \
		../../../Libs/EvolutionaryCore \
		../../../Libs/EvolutionaryCore/Operators \
		../../../Libs/EvolutionaryCore/OperatorSelectors \
		../../../Libs/Graph \
		../../../Libs/Log \
		../../../Libs/Shared \
		../../../Libs/Shared/Exceptions \
		../../../Libs/XmlParser/

INCLUDEPATH += 	../. \
		../../../ \
		../../ugp3/ \
		../../../Libs/Constraints \
		../../../Libs/EvolutionaryCore \
		../../../Libs/EvolutionaryCore/Operators \
		../../../Libs/EvolutionaryCore/OperatorSelectors \
		../../../Libs/Graph \
		../../../Libs/Log \
		../../../Libs/Shared \
		../../../Libs/Shared/Exceptions \
		../../../Libs/XmlParser

LIBS +=		-lqwt \
		-L../../../Libs/XmlParser/ \
		-L../../../Libs/Shared/ \
		-L../../../Libs/Log/ \
		-L../../../Libs/Graph/ \
		-L../../../Libs/Constraints/ \
		-L../../ugp3/ \
		-L../../../Libs/EvolutionaryCore/ \
		-Wl,--start-group ../../../Libs/EvolutionaryCore/libEvolutionaryCore.a ../../ugp3/libFrontendCommon.a ../../../Libs/Constraints/libConstraints.a ../../../Libs/Graph/libGraph.a ../../../Libs/XmlParser/libXmlParser.a ../../../Libs/Shared/libShared.a ../../../Libs/Log/libLog.a -Wl,--end-group
		
# Input

FORMS += 	ugp3-visualize-individual.ui \
		ugp3-gui.ui

HEADERS =	../../ugp3/Program.h \
		MicroGPVisualizeIndividual.h \
		EvolutionThread.h \
		MicroGPMainWindow.h

SOURCES = 	../../ugp3/Program.cc \
		../../ugp3/Program.Run.cc \
		MicroGPVisualizeIndividual.cpp \	
		EvolutionThread.cpp \
		MicroGPMainWindow.cpp \
		main.cpp