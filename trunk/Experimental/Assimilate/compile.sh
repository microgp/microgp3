# clean the thing
rm borg-temp
# clean
cd ../..
make
cd Experimental/Assimilate
g++-4.9 -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I. -I. -o Program.o ../../Frontends/ugp3/Program.cc
g++-4.9 -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I. -I. -o Program.Run.o ../../Frontends/ugp3/Program.Run.cc
g++-4.9 -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I. -I. -o main.o main.cpp
g++-4.9 -std=c++11 -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -Wall -Wl,-O1 -o borg-temp main.o Program.o Program.Run.o -L/usr/lib/i386-linux-gnu -L../../Libs/XmlParser/ -L../../Libs/Shared/ -L../../Libs/Log/ -L../../Libs/Graph/ -L../../Libs/Constraints/ -L../../Frontends/ugp3/ -L../../Libs/EvolutionaryCore/ -Wl,--start-group ../../Libs/EvolutionaryCore/libEvolutionaryCore.a ../../Frontends/ugp3/libFrontendCommon.a ../../Libs/Constraints/libConstraints.a ../../Libs/Graph/libGraph.a ../../Libs/XmlParser/libXmlParser.a ../../Libs/Shared/libShared.a ../../Libs/Log/libLog.a -Wl,--end-group
# run
./borg-temp --population population.settings.xml --assimilate individualA.s
