echo "Removing old files..."
./cleanup.sh
echo "Compiling evaluator..."
g++ zdt2.cpp -std=c++11 -lm -o zdt2
echo "Starting ugp3..."
ugp3
echo "Calling plot script..."
python plot-results.py
