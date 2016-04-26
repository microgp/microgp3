#include <cmath>
#include <float.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <string.h>
#include <vector>

#define FILE_EVALUATED "history.txt"
#define MAX_AVG_DIFFERENCE 10.0
#define MAX_DIFFERENCE 1000.0
#define MAX_INVALIDNODES 100
#define MAX_NODES 100
#define MAX_PENALIZE 10.0
#define OP_TREE "--tree"
#define OP_DATA "--data"
#define OP_ZEROCLONES "--zeroClones"
#define VALUE "value"

#define DEBUG 0

using namespace std;

// tokenizer function
void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


// node
class TreeNode
{
	public:
	// attributes
	TreeNode* left;
	TreeNode* right;
	string content;

	// functions	
	TreeNode();
	TreeNode(char* content);

	~TreeNode();
};

TreeNode::~TreeNode()
{}

TreeNode::TreeNode() :
left(nullptr),
right(nullptr),
content("")
{}

TreeNode::TreeNode(char* content) : 
left(nullptr),
right(nullptr),
content(content)
{}

// tree
class Tree
{
	public:
	TreeNode *root;
	unsigned int nodes;
	unsigned int invalidNodes;
	
	Tree(string fileName);
	~Tree();
	
	TreeNode* buildNode(string node, list<string> nodeList);
	string getFunction(TreeNode* node);
	double evaluateFunction(TreeNode* node, map<string, double> terminalSymbols);
	void freeTree(TreeNode* node);
};

Tree::~Tree()
{
	// visit in-order and remove leaves
	this->freeTree(this->root);
}

void Tree::freeTree(TreeNode *node)
{
	if( node->left == nullptr )
	{
		delete(node);
	}
	else if( node->right == nullptr )
	{
		this->freeTree(node->left);
		delete(node);
	}
	else
	{
		this->freeTree(node->left);
		this->freeTree(node->right);
		delete(node);
	}
}

Tree::Tree(string fileName) :
root(nullptr),
nodes(0),
invalidNodes(0)
{
	ifstream fileIn(fileName.c_str());

	if( !fileIn.is_open() )
	{
		return;
	}

	// search for root node
	string buffer;
	getline(fileIn, buffer);
	
	while( buffer.c_str()[0] == '#' )
	{
		getline(fileIn, buffer);
	}
	
	if(DEBUG) cout << "Line is " << buffer << endl;
	string rootNodeName = buffer;

	// get the rest of the file
	list<string> nodeList;
	while( fileIn.good() )
	{
		getline(fileIn, buffer);
		// if the line has a label, add it to the list
		if( buffer.find_first_of(":") != string::npos )
		{
			nodeList.push_front(buffer);
		}
		else if( buffer.c_str()[0] != '#' && buffer.size() > 0)
		{
			if(DEBUG) cout << "Invalid node: " << buffer << endl;
			invalidNodes++;
		}
	}
	fileIn.close();

	if(DEBUG) cout << "Invalid nodes so far: " << invalidNodes << endl;

	// debug
	if(DEBUG) for(list<string>::iterator it = nodeList.begin(); it != nodeList.end(); it++)
		cout << "- " << *it << endl;
	
	// build tree!
	// find root line
	this->root = this->buildNode(rootNodeName, nodeList);
	if(DEBUG) cout << "Content of node " << rootNodeName << ": " << this->root->content << endl;
	
	// compute final number of useless nodes
	if(DEBUG) cout << "Nodes referenced but not used: " << (nodeList.size() - this->nodes) << endl;
	this->invalidNodes += (nodeList.size() - this->nodes);
	if(DEBUG) cout << "There are " << this->nodes << " nodes used and " << this->invalidNodes << " nodes not used." << endl;
}

TreeNode* Tree::buildNode(string node, list<string> nodeList)
{
	for(list<string>::iterator it = nodeList.begin(); it != nodeList.end(); it++)
	{
		//char_separator<char> sep(" :");
		//tokenizer< char_separator<char> > tokens(*it, sep);
		vector<string> tokens;
		Tokenize(*it, tokens, " :");

		if(DEBUG) cout << "First token is " << *(tokens.begin()) << endl;	
	
		if( strcmp(node.c_str(), tokens.begin()->c_str() ) == 0 )
		{
			if(DEBUG) cout << "Node name found on line " << *it << endl;
			TreeNode* node = new TreeNode();
	
			string nodeLeft;
			string nodeRight;
			
			// find content and labels to search
			vector<string>::iterator tkIt = tokens.begin();
			tkIt++;
	
			for(tkIt; tkIt != tokens.end(); tkIt++)
			{
				if(DEBUG) cout << "- Token: " << *tkIt << endl;

				if( (*tkIt).find("node") == string::npos )
				{
					// content
					node->content = *tkIt;
				}
				else
				{
					if( node->left == nullptr )
					{
						node->left = this->buildNode(*tkIt, nodeList);
					}
					else if( node->right == nullptr)
					{
						node->right = this->buildNode(*tkIt, nodeList);
					}
				}
			}

			this->nodes++;
			return node;
		}
	}	

	return nullptr;
}

// visit the tree and output the function as it appears
string Tree::getFunction(TreeNode* node)
{
	string function = "";

	if( node->left == nullptr)
	{
		// terminal node
		return node->content;
	}
	else if( node->right == nullptr )
	{
		// one argument operator
		function += node->content;
		function += "(";
		function += this->getFunction(node->left);
		function += ")";
		
		return function;
	}
	else
	{
		// two argument operator
		function += "(";
		function += this->getFunction(node->left);
		function += ")";
		function += node->content;
		function += "(";
		function += this->getFunction(node->right);
		function += ")";

		return function;
	}
}

// evaluate function; it needs a hashMap of all terminal symbols
double Tree::evaluateFunction(TreeNode* node, map<string, double> terminalSymbols)
{
	if( node->left == nullptr)
	{
		// terminal node: either a terminal symbol or a double
		// TODO: what if there is a symbol not in the map?
		if( terminalSymbols.find(node->content) != terminalSymbols.end() )
		{
			return terminalSymbols[node->content];
		}
		else
		{
			double tempValue;
			sscanf(node->content.c_str(), "%lf", &tempValue);
			return tempValue;
		}
	}
	else if( node->right == nullptr )
	{
		// one argument operator
		double tempValue = evaluateFunction(node->left, terminalSymbols);

		// perform operation on tempValue
		// TODO: add other single-operand operators here
		if( node->content.compare("log") == 0)
			return log10(tempValue);
		else if( node->content.compare("ln") == 0)
			return log(tempValue);
		else if( node->content.compare("sin") == 0)
			return sin(tempValue);
		else if( node->content.compare("cos") == 0)
			return cos(tempValue);
		else if( node->content.compare("tan") == 0)
			return tan(tempValue);
		else if( node->content.compare("asin") == 0)
			return asin(tempValue);
		else if( node->content.compare("acos") == 0)
			return acos(tempValue);
		else if( node->content.compare("atan") == 0)
			return atan(tempValue);
		else if( node->content.compare("sqrt") == 0)
			return sqrt(tempValue);
		else
		{
			cerr << "Warning: symbol " << node->content << " not found!" << endl;
			return 0.0;
		}
	}
	else
	{
		// two argument operator
		double tempValue1 = evaluateFunction(node->left, terminalSymbols);
		double tempValue2 = evaluateFunction(node->right, terminalSymbols);

		if( node->content.compare("*") == 0)
			return tempValue1 * tempValue2;
		else if( node->content.compare("/") == 0)
			return tempValue1 / tempValue2;
		else if( node->content.compare("+") == 0)
			return tempValue1 + tempValue2;
		else if( node->content.compare("-") == 0)
			return tempValue1 - tempValue2;
		else if( node->content.compare("pow") == 0)
			return pow(tempValue1, tempValue2);
		else
		{
			cerr << "Warning: symbol " << node->content << " not found!" << endl;
			return 0.0;
		}
	}

}

// main
int main(int argc, char* argv[])
{
	string dataFile = "";
	string treeFile = "";
	bool zeroClones = false;

	// check the arguments
	for(unsigned int a = 1; a < argc; a++)
	{
		if( strcmp(argv[a], OP_TREE) == 0 )
		{
			if( a+1 < argc ) treeFile = argv[a+1];
			if(DEBUG) cout << "Tree file: " << treeFile << endl;
		}
		else if( strcmp(argv[a], OP_DATA) == 0 )
		{
			if( a+1 < argc) dataFile = argv[a+1];
			if(DEBUG) cout << "Data file: " << dataFile << endl;
		}
		else if( strcmp(argv[a], OP_ZEROCLONES) == 0 )
		{
			zeroClones = true;
			if(DEBUG) cout << "Option zeroClones activated." << dataFile << endl;
		}
		else if( argv[a][0] == '-' && argv[a][1] == '-' )
		{
			cerr << "Warning: option " << argv[a] << " not recognized." << endl;
		}
	}		

	if( treeFile.size() == 0 )
	{
		cerr << "Error: you must specify a tree file." << endl;
		return -1;
	}

	// if we are here, then we can build the tree
	Tree* tree = new Tree(treeFile);
	if( tree->nodes == 0 )
	{
		cerr << "Error: cannot open file " << treeFile << "! Aborting..." << endl;
		return -1;
	}

	string function	= tree->getFunction(tree->root);
	if(DEBUG) cout << "Function: " <<  function << endl;

	// now, compute fitness function: adherence to data
	if( dataFile.size() <= 0 )
	{
		map<string, double> terminalSymbols;
		string t = "t";
		cerr << "Warning: no data file specified. Printout of function for t=1..." << endl;
		terminalSymbols[t] = 1.0;
		cout << "Value (for t=1): " << tree->evaluateFunction(tree->root, terminalSymbols) << endl;
		
		return 0;
	}

	// compute adherence to data
	double difference = 0.0;

	ifstream dataStream(dataFile.c_str());
	if( !dataStream.is_open() )
	{
		cerr << "Error: cannot open file " << dataFile << endl;
		return -1;
	}

	// read header: the value must be called "value"!
	string buffer;
	vector<string> tokens;
	
	getline(dataStream, buffer);
	Tokenize(buffer, tokens, " ,;");

	list<string> symbols;
	for(unsigned int t = 0; t < tokens.size(); t++)
	{
		if(DEBUG)cout << "Symbol: " << tokens[t] << endl;
		symbols.push_back(tokens[t]);
	}

	// symbol map
	map<string, double> symbolsMap;

	// these will be useful to compute the first derivative
	vector<double> valueData;
	vector<double> valueEsteem;

	// read data and compute difference
	double numberOfSamples = 0.0;
	while( dataStream.good() )
	{
		// increase samples
		numberOfSamples++;

		// parse line
		tokens.clear();
		getline(dataStream, buffer);
		Tokenize(buffer, tokens, " ;,");

		// associate values to symbols
		list<string>::iterator symbolsIt = symbols.begin();
		vector<string>::iterator tokensIt = tokens.begin();

		for(symbolsIt, tokensIt ; symbolsIt != symbols.end() && tokensIt != tokens.end(); symbolsIt++, tokensIt++)
		{
			double value = 0.0;
			sscanf((*tokensIt).c_str(), "%lf", &value);
			symbolsMap[*symbolsIt] = value;
			
			if(DEBUG) cout << "Value for parameter " << *symbolsIt << " is " << value << endl;
		}

		// compute function value for the line
		if(DEBUG) cout << "Extimated value for function is " << tree->evaluateFunction(tree->root, symbolsMap) << endl;

		// lineDifference = difference^2
		//double lineDifference = pow( symbolsMap[VALUE] - tree->evaluateFunction(tree->root, symbolsMap), 2 );
		// lineDifference = abs(difference)
		valueData.push_back(symbolsMap[VALUE]);
		valueEsteem.push_back(tree->evaluateFunction(tree->root, symbolsMap));
		double lineDifference = abs( valueData.back() - valueEsteem.back() );
		if(DEBUG) cout << "Difference between the values is " << lineDifference << endl;
	
		// what happens if lineDifference is NAN or +inf?	
		if(std::isnan(lineDifference) || std::isinf(lineDifference))
			difference += MAX_PENALIZE;
		else
			difference += lineDifference;
	}
	dataStream.close();

	if(DEBUG) cout << "Total squared difference: " << difference << endl;
	if(DEBUG) cout << "Average squared difference: " << difference / numberOfSamples << endl;

	// adherence
	double adherence = (double) MAX_AVG_DIFFERENCE - (difference / numberOfSamples);
	if( adherence < 0 ) adherence = 0;

	// now, adherence of the first derivative
	double derivativeDifference = 0.0;
	for(unsigned i = 1; i < valueData.size(); i++)
	{
		double derivativeData = abs(valueData[i] - valueData[i-1]);
		double derivativeEsteem = abs(valueEsteem[i] - valueEsteem[i-1]);
		double lineDifference = abs(derivativeData - derivativeEsteem);

	        if(std::isnan(lineDifference) || std::isinf(lineDifference))
                        derivativeDifference += MAX_PENALIZE;
                else
                        derivativeDifference += lineDifference;
	}
	double derivativeAdherence = (double) MAX_AVG_DIFFERENCE - (derivativeDifference / numberOfSamples);
	if( derivativeAdherence < 0 ) derivativeAdherence = 0;

	// if the option zeroClones is activated
	if( zeroClones )
	{
		// hash map of all functions evaluated so far
		map<string, bool> history;

		ifstream historyIn(FILE_EVALUATED);
		if( !historyIn.is_open() )
		{
			cerr << "Warning: cannot open file " << FILE_EVALUATED << endl;
		}
		else
		{
			while( historyIn.good() )
			{
				getline(historyIn, buffer);
				history[buffer] = true;
			}		
		}
		historyIn.close();

		// check if the function has been already evaluated
		if( history.find(function) == history.end() )
		{
			// function not found in history, add it to map, save file
			history[function] = true;

			ofstream historyOut(FILE_EVALUATED);
			if(! historyOut.is_open() )
			{
				cerr << "Error: cannot write on file " << FILE_EVALUATED << " with option " << OP_ZEROCLONES << " active." << endl;
				return -1;
			}

			for(map<string, bool>::iterator it = history.begin(); it != history.end(); it++)
			{
				historyOut << (*it).first << endl;
			}
			historyOut.close();
		}
		else
		{
			// function already evaluated
			adherence = 0;
			derivativeAdherence = 0;
			function = "already_evaluated";
			tree->nodes = MAX_NODES;
		}
	}

	// TODO: 	this part should be optional (OP_NUMBER_OF_DIFFERENT_SYMBOLS?)
	//		for the moment, there must be at least one parameter, or adherence is halved
	bool atLeastOneSymbol = false;
	for(list<string>::iterator it = symbols.begin(); it != symbols.end() && atLeastOneSymbol == false; it++)
	{
		string stringToFind = "(";
		stringToFind += *it;
		stringToFind += ")";

		if(DEBUG) cout << "Now searching for string " << stringToFind << " inside " << function;

		if( function.find(stringToFind) != string::npos )
		{
			atLeastOneSymbol = true;
			if(DEBUG) cout << " -> found!";
		}
		if(DEBUG) cout << endl;
	}
	if( atLeastOneSymbol == false) adherence /= 2;

	// save fitness value to file
	ofstream fileOut("fitness.output");
	if( fileOut.is_open() )
	{
		// fitness: adherence / percentage of invalid nodes / parcimony function / comment string (function + average squared distance)
		fileOut 
			//<< adherence * 0.5 + derivativeAdherence * 0.5 << " " 
			<< derivativeAdherence << " "
			<< adherence << " " 
			<< MAX_NODES - tree->nodes << " " 
			<< (double) tree->nodes / (tree->nodes + tree->invalidNodes) << " "
			// below, comment string (not part of the fitness)
			<< function << "->" << difference/numberOfSamples << ":" << (atLeastOneSymbol ? "contains_var" : "does_not_contain_var") << endl; 
	}	
	fileOut.close();	

	return 0;	
}
