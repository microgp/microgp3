echo "Removing old files..."
rm history.txt statistics* status* fitness.output* individual*
echo "Compiling evaluator..."
g++ evaluator.cpp -std=c++11 -lm -o evaluator
echo "Starting ugp3..."
ugp3
