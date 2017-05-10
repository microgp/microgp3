#CXX=g++-5
CXX=g++

# clean the thing
echo "Removing executable..."
rm borg-temp
# clean
echo "Compiling ugp3's libraries..."
cd ../..
make
echo "Compiling local files..."
cd Experimental/Assimilate

echo "Compiling Program.cc ..."
${CXX} -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I../../Libs/EvolutionaryCore/Operators -I../../Libs/EvolutionaryCore/OperatorSelectors -I. -I. -o Program.o ../../Frontends/ugp3/Program.cc

echo "Compiling Program.Run.cc ..."
${CXX} -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I../../Libs/EvolutionaryCore/Operators -I../../Libs/EvolutionaryCore/OperatorSelectors -I. -I. -o Program.Run.o ../../Frontends/ugp3/Program.Run.cc

${CXX} -std=c++11 -c -pipe -O2 -Wall -W -I. -I../.. -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I../../Libs/EvolutionaryCore/Operators -I../../Libs/EvolutionaryCore/OperatorSelectors -I. -I. -o main.o main.cpp

${CXX} -std=c++11 -I../../Libs/Shared -I../../Libs/Shared/Exceptions -I../../Libs/Log -I../../Libs/XmlParser -I../../Libs/Graph -I../../Libs/Constraints -I../../Frontends/ugp3 -I../../Libs/EvolutionaryCore -I../../Libs/EvolutionaryCore/Operators -I../../Libs/EvolutionaryCore/OperatorSelectors -Wall -Wl,-O1 -o borg-temp main.o Program.o Program.Run.o -L/usr/lib/i386-linux-gnu -L../../Libs/XmlParser/ -L../../Libs/Shared/ -L../../Libs/Log/ -L../../Libs/Graph/ -L../../Libs/Constraints/ -L../../Frontends/ugp3/ -L../../Libs/EvolutionaryCore/ -Wl,--start-group ../../Libs/EvolutionaryCore/libEvolutionaryCore.a ../../Frontends/ugp3/libFrontendCommon.a ../../Libs/Constraints/libConstraints.a ../../Libs/Graph/libGraph.a ../../Libs/XmlParser/libXmlParser.a ../../Libs/Shared/libShared.a ../../Libs/Log/libLog.a -Wl,--end-group

# run
./borg-temp --population population.settings.xml --assimilate individualA.s
#./borg-temp --population population.settings.xml --assimilate individualA.in
