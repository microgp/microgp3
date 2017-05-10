#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
using namespace std;

#define MAX_WEIGHT 1.0
#define ITEMS_DATABASE_FILE "items.db"
#define FILE_OUTPUT "fitness.output"

struct Fitness
{
    double inverseWeight;
    double profit;

    Fitness() : inverseWeight(0), profit(0) { }
};

struct Item
{
    double weight;
    double profit;
    unsigned int id;

    Item() : weight(0), profit(0), id(0) { }
};

map<unsigned int, Item*> items;

void loadItems(const string& fileName) throw(exception)
{
    ifstream input(fileName.c_str());
    
    if(input.is_open() == false)
    {
         throw runtime_error("cannot open database file " + fileName);
    }
    
    while(input.eof() == false)
    {
        Item* item = new Item();
        input >> item->id;
        input >> item->weight;
        input >> item->profit; 
    
        if(input.eof() == true)
        {
            delete item;
            break;
        }
        else if(input.fail() == true)
        {
            throw runtime_error("cannot read from database");
        }  

        items[item->id] = item;     
    }

    input.close();
}

Fitness evaluate(const string& fileName)
{
    ifstream input(fileName.c_str());

    Fitness fitness;



    double totalWeight = 0;
    while(input.eof() == false)
    {
        char* buffer = new char[10*1024*1024];
        input.getline(buffer, 10*1024*1024);
        const string& line = buffer;

        for(unsigned int i = 0; i < line.length(); i++)
        {
	        if(line.at(i) == '1')
            {
                Item* item = (*items.find(i)).second;
                fitness.profit += item->profit;
                totalWeight += item->weight;
            }
	        else if(line.at(i) != '\n' && line.at(i) != '\r' && line.at(i) != '0')
	        {
		        cerr << "Evaluator: unexpected character at position " << i << "." << endl;

	
		        break;
	        }
        }
       
        
    }

    if(totalWeight > 0)
    {
        fitness.inverseWeight = 1.0 / totalWeight;
    }

    if(totalWeight > MAX_WEIGHT)
    {
        fitness.profit = 0;
    }

    return fitness;
}

int main(unsigned int argc, const char** argv)
{
    // load the knapsack items
    loadItems(ITEMS_DATABASE_FILE);

    // open the file where the results will be saved
    ofstream output(FILE_OUTPUT);

    // parse and evaluate the individuals
    for(unsigned int i = 1; i < argc; i++)
    {
        const string& argument = argv[i];
        const Fitness& fitness = evaluate(argument);

        output << fitness.profit << " " << fitness.inverseWeight << endl;
    }

    output.close();  

    return EXIT_SUCCESS;  
}
