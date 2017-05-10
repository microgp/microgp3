#!/bin/bash

# Runs various code analysis tools and finally the sonar test runner.

SOURCES=$(find Libs Frontends/ugp3 -regex ".*\.cc\|.*\.h")

echo $SOURCES | vera++ - -showrules -nodup |& ./vera++Report2checkstyleReport.perl > veraReport.xml

cppcheck -v --enable=all --xml-version=2 -j 4 \
-IFrontends/ugp3               \
-ILibs                         \
-ILibs/Constraints             \
-ILibs/EvolutionaryCore        \
-ILibs/Graph                   \
-ILibs/Log                     \
-ILibs/Shared                  \
-ILibs/Shared/Exceptions       \
-ILibs/XmlParse                \
$SOURCES 2> cppcheckReport.xml

rats -w 3 --xml $SOURCES > ratsReport.xml

~/Téléchargements/sonar-runner-2.4/bin/sonar-runner
