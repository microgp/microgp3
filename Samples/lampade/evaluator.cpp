// Lamp Evaluator for Group Evolution
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <iterator>
#include <cmath>

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define OPTION_PARAM "--param"
#define OPTION_GROUP "--group"
#define OPTION_MULTIRUN "--multirun"
#define OPTION_INDIVIDUALNUMBER "--individualNumber"
#define OPTION_GROUPMULTIRUN "--groupMultirun"
#define OPTION_SINGLEINDIVIDUAL "--singleIndividual"
#define OPTION_NUMBERINFITNESS "--numberInFitness"

#define STRING_RADIUS "r"
#define STRING_HEIGHT "h"
#define STRING_WIDTH "w"

#define DEBUG 0

using namespace std;

class Lamp
{
      public:
             unsigned int radius;
             unsigned int x;
             unsigned int y;    
      
      Lamp(unsigned int radius, unsigned int x, unsigned int y);
      void printLamp();
      
};

Lamp::Lamp(unsigned int radius, unsigned int x, unsigned int y)
{
                    this->radius = radius;
                    this->x = x;
                    this->y = y;
}

void Lamp::printLamp()
{
             //cout << "Radius=" << this->radius << "; X=" << this->x << "; Y=" << this->y << "." << endl;             
}

int main(int argc, char** argv)
{
    // if no arguments are passed, print help and exit
    if(argc == 1)
    {
            cout << "Usage: " << argv[0] <<  " --param <parameters.txt> --group <group.txt> [OPTIONAL]" << endl;
            cout << "OPTIONAL:" << endl;
            cout << "\t" << OPTION_MULTIRUN << ": use it to print only the area covered by the last individual, excluding the area covered by the previous ones." << endl;        
            cout << "\t" << OPTION_INDIVIDUALNUMBER <<": use it to give two fitness values, the second being the number of individuals in a group." << endl;        
            cout << "\t"<< OPTION_GROUPMULTIRUN <<": use it to print multirun fitness for all individuals in the group, along with group fitness." << endl;        
            cout << "\t"<< OPTION_SINGLEINDIVIDUAL << ": all lamps coordinates are in a single file." << endl;
            cout << "\t"<< OPTION_NUMBERINFITNESS << ": only one global fitness value [0,1] taking into account the number of individuals (the lower, the better)." << endl; 
	    return 0;
    }
    
    // parse arguments (argv[0] is the program name)
    string parametersFile, groupFile;
    bool multirun = false, individualNumber = false, groupMultirun = false, singleIndividual = false, numberInFitness = false;
    for(unsigned int a = 1; a < argc; a++)
    {
            string s (argv[a]);
            
            if(s.compare(OPTION_PARAM) == 0)
            {
                  parametersFile.assign(argv[a+1]);   
            }
            else if(s.compare(OPTION_GROUP) == 0)
            {
                  groupFile.assign(argv[a+1]);     
            }
            else if(s.compare(OPTION_MULTIRUN) == 0)
            {
                  multirun = true;     
            }
            else if(s.compare(OPTION_INDIVIDUALNUMBER) == 0)
            {
                  individualNumber = true;     
            }
            else if(s.compare(OPTION_GROUPMULTIRUN) == 0)
            {
                  groupMultirun = true;     
            }
            else if(s.compare(OPTION_SINGLEINDIVIDUAL) == 0)
            {
                  singleIndividual = true;     
            }      
            else if(s.compare(OPTION_NUMBERINFITNESS) == 0)
            {
                  numberInFitness = true;     
            }      
    }
    
    // debug
    //cout << "Parameters file: " << parametersFile << endl;
    //cout << "Group file: " << groupFile << endl;
    //if(multirun)
    //   cout << "This is a multirun execution." << endl;
    
    // parsing of input files
    // parsing of parameters
    unsigned int lampRadius = 0, height = 0, width = 0;
    ifstream inputStream(parametersFile.c_str());
    if(inputStream.is_open())
    {
        while(inputStream.good())
        {
           // parsing   
           string line;
           getline(inputStream, line);
           
           if(sscanf(line.c_str(), "r=%d", &lampRadius))
           {
                           
           }
           else if(sscanf(line.c_str(), "h=%d", &height))
           {
           
           }
           else if(sscanf(line.c_str(), "w=%d", &width));
        }
        
        inputStream.close();                    
    }
    else
    {
        // print error and exit
        cout << "Error: could not find file " << parametersFile << endl;
        return -1;    
    }
    
    if(lampRadius == 0 || width == 0 || height == 0)
    {
        cout << "Error: one of the parameters read is 0 or parameters not found in file " << parametersFile << "." << endl;
        return -1;              
    }
    
    // debug
    //cout << "LampRadius: " << lampRadius << "; Height: " << height << "; Width: " << width << endl;
    list<Lamp> lamps;
    if(singleIndividual == true) // the argument is a single file, with all lamps coordinates in it
    {
        ifstream inputStream2(groupFile.c_str());
        if(inputStream2.is_open())
        {
            while(inputStream2.good())
            {
                 // get line from input
                 string line;
                 getline(inputStream2, line);
                 
                 // parse line for coords e store it into Lamp
	 Lamp lamp(lampRadius,0,0);
	 if( sscanf(line.c_str(), "%d,%d", &lamp.x, &lamp.y) == 2)
                 {
		     sscanf(line.c_str(), "%d,%d", &lamp.x, &lamp.y);
                     lamps.push_back(lamp);
                 }
                 //else
                 //{
                  //   cout << "Error in file " << groupFile.c_str() << endl;
                  //   return -1;
                 //}                                              
            }
        }
        else
        {
            cout << "Error while opening file " << groupFile.c_str() << endl;
            return -1;             
        }  
    }
    else // the argument is a group file, with individuals' filenames inside
    {
        // parsing of group file
        //cout << "Opening file " << groupFile << endl;
        list<string> individualFiles;
        ifstream inputStream2(groupFile.c_str());
        if(inputStream2.is_open())
        {
            while(inputStream2.good())
            {
                 string line;
                 getline(inputStream2, line);
		 if(line.size() > 1)
                 	individualFiles.push_back(line); 
            }
            inputStream2.close();                         
        }
        else
        {
            cout << "Error: could not find file " << groupFile << endl;
            return -1;       
        }
        
        // parsing of individual files
	//cout << individualFiles.size() << " individuals in the group." << endl;
        for(list<string>::iterator it = individualFiles.begin(); it != individualFiles.end(); it++)
        {
             //cout << "Opening file " << (*it) << endl;
             ifstream inputStream3((*it).c_str());
             if(inputStream3.is_open() && inputStream3.good())
             {
                 Lamp lamp(lampRadius,0,0);
                 string line;
                 getline(inputStream3, line);
                 if(sscanf(line.c_str(), "%d", &lamp.x))
                 {
                    getline(inputStream3, line);
                    if(sscanf(line.c_str(), "%d", &lamp.y))
                     {}
                     else
                     {
                         cout << "Error in file " << it->c_str() << endl;
                         return -1;
                     }
                 }
                 else
                 {
                     cout << "Error in file " << it->c_str() << endl;
                     return -1;
                 }
                 inputStream3.close();
                 lamps.push_back(lamp);
             }
             else
             {
                 cout << "Error while opening file " << it->c_str() << endl;
                 return -1;             
             }
        }
    
    } // end if singleIndividual == true
    
    // debug
    for(list<Lamp>::iterator it = lamps.begin(); it != lamps.end(); it++)
    {
       //cout << "Lampadaaaa!";
       (*it).printLamp();
    }
    
    // finally we can compute the fitness value(s)!
    unsigned int groupFitness = 0, individualFitness[lamps.size()], multirunFitness[lamps.size()], overlap = 0;
    // zero all fitness values
    for(unsigned int i = 0; i < lamps.size(); i++)
    {
                 individualFitness[i] = 0;
                 multirunFitness[i] = 0;
    }
    // debug: needed to use the output directly as a .pbm image
    
    ofstream image;
    if (DEBUG) {
      image.open(("/tmp/" + groupFile + ".ppm").c_str());
      image << "P2\n#code needed to generate a .ppm image\n" << width << " " << height << "\n" << lamps.size() << endl; 
    }

    // for each point in the field, evaluate if it is lightened
    for(unsigned int x = 0; x < width; x++)
    {
    for(unsigned int y = 0; y < height; y++)
    {
        unsigned int enlightened = 0;
        // is the point inside the circumference of a lamp?
        unsigned int lampNumber = 0;
        for(list<Lamp>::iterator it = lamps.begin(); it != lamps.end(); it++, lampNumber++)
        {
            float result = pow((double)it->x - x, 2) + pow((double)it->y -y, 2);
            if(result <= pow((double) (it->radius), 2))
            {
                      enlightened++;
                      individualFitness[lampNumber]++;
                      if(enlightened == 1) // first lamp to enlighten point
                           multirunFitness[lampNumber]++;
            }
        }
        
	// if the point is enlightened by at least one lamp
        if(enlightened > 0)
            groupFitness++;
        //else // debug
             //cout << "Point " << x << "," << y << " is not enlightened." << endl;

	// take into account the overlap
	if(enlightened > 1)
	    overlap++;

	if(DEBUG) image << enlightened << " ";
    }
	if(DEBUG) image << endl;
    }    
    
    // since fitness is proportional to 1/overlap, the line below is to avoid infinity results
    if(overlap == 0)
	overlap = 1;    

    // debug
    /*
    cout << "Group fitness is " << groupFitness << endl;
    unsigned int lampNumber = 0;
    for(list<Lamp>::iterator it = lamps.begin(); it != lamps.end(); it++, lampNumber++)
    {
       cout << "Lamp #" << lampNumber << " has fitness " << individualFitness[lampNumber]
            << " and multirun fitness " << multirunFitness[lampNumber] << endl;
    }
    */
    
    // weightedFitness
    double weightedFitness = 0;

    // write fitness values to file
    ofstream outputFile("fitness.output");
    if(outputFile.is_open())
    {
         if(multirun)
         {
            outputFile << multirunFitness[lamps.size() - 1] << endl;
         }
         else if(singleIndividual)
         {
             // group fitness
	     if( numberInFitness == false)                
             	outputFile << groupFitness;
	     else
	     {
		weightedFitness = 0.70 * groupFitness / (width * height) + 0.20 * 1/lamps.size() + 0.10 * (width * height - overlap)/(width * height);
		outputFile << weightedFitness;
		if(DEBUG) cout << "Coverage = " << (float) groupFitness / (width * height) << "; Lamps = " << (float) lamps.size() << "; Overlap = " << (float) ((width * height) - overlap)/(width * height) << endl;
		if(DEBUG) cout << "groupFitness = 0.70 * " << groupFitness << "/" << (width * height) << " + 0.20 * 1 / " << lamps.size() << " + 0.10 * " << (width * height) - overlap << "/" << width * height << endl;
             }
            
	    if(individualNumber) outputFile << " " << (float) 1 / lamps.size();
            outputFile << endl;          
         }
         else
         {
             // group fitness
	     if( numberInFitness == false)                
             	outputFile << groupFitness;
	     else
	     {
		weightedFitness = 0.70 * groupFitness / (width * height) + 0.20 * 1/lamps.size() + 0.10 * (width * height - overlap)/(width * height);
		outputFile << weightedFitness;
		if(DEBUG) cout << "Coverage = " << (float) groupFitness / (width * height) << "; Lamps = " << (float) lamps.size() << "; Overlap = " << (float) ((width * height) - overlap)/(width * height) << endl;
		if(DEBUG) cout << "groupFitness = 0.70 * " << groupFitness << "/" << (width * height) << " + 0.20 * 1 / " << lamps.size() << " + 0.10 * " << (width * height) - overlap << "/" << width * height << endl;
             }

             if(individualNumber)
                      outputFile << " " << (float) 1 / lamps.size();
             
	     // FIXME group fitness evaluator expects all values on the same line
             //outputFile << endl;
	     outputFile << " ";
             
             // individual fitness
             unsigned int lampNumber = 0;
             for(list<Lamp>::iterator it = lamps.begin(); it != lamps.end(); it++, lampNumber++)
             {
                   if(!groupMultirun)
                   {
                        outputFile << individualFitness[lampNumber];
                   }
                   else
                   {
                       outputFile << multirunFitness[lampNumber];
                   }
                   
                   if(individualNumber)
                        outputFile << " 0";
                   
		   // FIXME group fitness evaluator expects all values on the same line
                   //outputFile << endl;       
		   outputFile << " ";
             }  
         }                      
    }
    outputFile.close();

    // write some statistics to file
    // does the file exist?
    struct stat stFileInfo;
    bool blnReturn;
    int isFirstTime;

    // attempt to get the file attributes
    isFirstTime = stat("statistics.csv",&stFileInfo); 

    ofstream statistics;
    statistics.open("statistics.csv", ios::app);

    if(isFirstTime != 0)
    {
	statistics << "generation;fitness;coverage;lamps;overlapping" << endl;
	statistics << "INT;DOUBLE;DOUBLE;INT;DOUBLE" << endl;
    }
    
    // get generation from environmental variable 
    // generation; fitness; coverage; lamps; overlapping
    if( getenv("UGP3_GENERATION") == NULL)
	 statistics << "0";
    else
	statistics <<  getenv("UGP3_GENERATION");
 
    statistics << ";" << weightedFitness << ";" << (double) groupFitness / (width * height) << ";" << lamps.size() << ";" << (double) (overlap) / (width * height) << endl;

    statistics.close();

    // also, write some more details to output in a file
    if(DEBUG)
    {
	ofstream extendedFitness;
	extendedFitness.open("extended.fitness.output");
	
	extendedFitness << "fitness=" << weightedFitness << endl;
	extendedFitness << "coverage=" << (double) groupFitness / (width * height) << endl;
	extendedFitness << "lamps=" << lamps.size() << endl;
	extendedFitness << "overlap=" << (double) (overlap) / (width * height);	
    	extendedFitness.close();
    }
}
