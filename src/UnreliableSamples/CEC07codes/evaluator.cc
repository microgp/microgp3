#include <dlfcn.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <fstream>
using namespace std;

typedef void (*PPROC) (double *, double *, int, int);
#define LIBHANDLE void *
#define GetProcedure dlsym
#define CloseDynalink dlclose

// global pointer to dynamically linked procedure
PPROC target = nullptr; 

//Compiling your code:
//Linux: use the flag -ldl

void displayUsage(const string& programName) throw()
{
    cout << "Usage: " << endl << "    " << programName;
    cout << " <function> <input_dim> <output_dim> <file1> <file2> ..." << endl << endl;
    cout << " where <function> is one of: OKA2 SYMPART S_ZDT1 S_ZDT2 S_ZDT4 R_ZDT4 S_ZDT6 S_DTLZ2 R_DTLZ2 S_DTLZ3 WFG1 WFG8 WFG9" << endl;
    cout << "       <input_dim> is the size of the input vector x." << endl;
    cout << "       <output_dim> is the size of the output vector y." << endl;
    cout << "       <fileN> is a file to evaluate." << endl;
}

const string evaluate(
    const string& inputFileName, 
    unsigned int inputDimensions, 
    unsigned int outputDimensions) throw(exception)
{
    assert(target != nullptr);

    ifstream inputFile(inputFileName.c_str());
    if(inputFile.is_open() == false)
    {
        throw runtime_error("Cannot access input file " + inputFileName);
    }
    
    double* y = new double[outputDimensions];
    double* x = new double[inputDimensions];
    for(unsigned int i = 0; i < inputDimensions; i++)
    {
        inputFile >> x[i];
        if(inputFile.fail())
        {
            throw runtime_error("Error while reading from file " + inputFileName);        
        }
    }

    //now call the test function
    target (x, y, inputDimensions, outputDimensions);

    // where inputDimensions is the number of variables, 
    //       outputDimensions is the number of objective functions, 
    //       y is the objective function values. The function will return the values y.

    ostringstream outputFitness;
    // this is a minimization problem but microgp maximizes only:
    // invert the value
    for(unsigned int i = 0; i < outputDimensions; i++)
    {
        y[i] = 1000.0 - y[i];
        outputFitness << y[i] << " ";
    }

    return outputFitness.str();
}


int main(unsigned int argCount, const char** arguments) throw()
{
    try
    {        if(argCount < 4) 
        {
            displayUsage(arguments[0]);
            throw runtime_error("Invalid number of program arguments");
        }

        // load the library fsuite 
        LIBHANDLE hLibrary = dlopen ("./fsuite.so", RTLD_NOW);

        // get a pointer to the procedure
        const char* functionName = arguments[1];
        target = (PPROC) dlsym (hLibrary, functionName); 

        // get the input and output dimensions
        unsigned int inputDimensions = 0;
        unsigned int outputDimensions = 0;
        istringstream input(arguments[2]);
        input >> inputDimensions;
        if(input.fail())
            throw runtime_error("Invalid value for commandline argument #2");
        istringstream output(arguments[3]);
        output >> outputDimensions;
        if(output.fail())
            throw runtime_error("Invalid value for commandline argument #3");

        // open the file where fitnesses are saves
        ofstream fitnessFile("fitness.txt");

        // evaluate the files
        for(unsigned int i = 4; i < argCount; i++)
        {
            const string& fileName = arguments[i];
            const string& fitnessLine = evaluate(fileName, inputDimensions, outputDimensions);

            fitnessFile << fitnessLine << endl;
        }

        // close the output file
        fitnessFile.close();

        // close the library
        dlclose (hLibrary);

        exit(EXIT_SUCCESS);
    }
    catch(const exception& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
