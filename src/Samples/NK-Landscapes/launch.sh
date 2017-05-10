echo "Removing files from previous executions..."
rm status.xml statistics*.xml *.input
echo "Compiling evaluator..."
cd evaluator-source
./compile.sh
mv nk-landscapes ../evaluator
cd ..
echo "Starting ugp3..."
ugp3
