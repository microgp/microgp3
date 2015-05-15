#include "ugp3-extractor.h"

/* The two-argument signature for main has int as the first argument. GCC 4.3 rigorously enforces this. */
int main(int argc, char* argv[])
{
    try
    {
        setupLogging();
        
        displayHeader();

        bool exitRequired = parseArguments(argc, argv);
        if(exitRequired == false)
        {
            saveDump();
        } 
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << e.what() << std::ends;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

