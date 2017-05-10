echo "Removing old files..."
rm history.txt statistics* status* fitness.output* individual*
echo "Compiling evaluator..."
./compile.sh
echo "Starting ugp3..."
ugp3
