#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "banana.h"

#define OUTPUT_FILE_NAME "fitness.txt"

void saveResults(vector<double>& fitnesses)
{
    ofstream file;
    file.open(OUTPUT_FILE_NAME);
    
    for(unsigned int i = 0; i < fitnesses.size(); i++)
    {
        file << fitnesses[i] << endl; 
    }
    
    file.close();
}

double evaluateFile(const string& fileName)
{
    ifstream file;
    file.open(fileName.c_str());
    

    double x1;
    file >> x1;
    
    double x2;
    file >> x2;
    
    file.close();
    
    // banana function has a minimum value of 0.
    // invert the function in order to get a maximum fitness
    // value when the minimum is found
    double x = -banana(x1, x2);
    double fitness = - 1 / (x - 1);
    
    return  fitness;
}

int main(unsigned int argCount, const char** arguments)
{
    if(argCount == 1) 
    {
        cerr << "Banana evaluator called without arguments." << endl;
        return EXIT_FAILURE;
    }
    
    vector<double> fitnesses;
    for(int i = 1; i < argCount; i++)
    {
        double fitness = evaluateFile(arguments[i]);
        fitnesses.push_back(fitness);
    }
    
    saveResults(fitnesses);
    
    return EXIT_SUCCESS;
}
