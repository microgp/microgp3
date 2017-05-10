#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <memory>
#include <map>
#include <cassert>
#include <stdexcept> 
using namespace std;

static const double xMin = -10;
static const double xMax = 10;
static const double xStep = 0.01;

double toDouble(const string& token)
{
    istringstream stream;
    stream.str(token);

    double value = 0;
    stream >> value;

    if(stream.fail() == true)
    {
        cerr << "Could not convert the string \"" << token << "\" into double." << endl;
        exit(EXIT_FAILURE);
    }

    return value;
}

struct Operator
{
private:
    char value;
 
    Operator(const Operator& op);
    Operator(char value);
public:
    Operator();
    

    static const Operator Add;
    static const Operator Mul;
    static const Operator Div;
    static const Operator Sub;
    static const Operator None;

    inline bool operator==(const Operator& op) const
    {
        return value == op.value;
    }
};

Operator::Operator()
: value('n')
{ }

Operator::Operator(char value)
: value(value)
{ }

const Operator Operator::Add('a');
const Operator Operator::Mul('m');
const Operator Operator::Div('d');
const Operator Operator::Sub('s');
const Operator Operator::None('n');

struct Expression
{
private:
    string id;
    double value;
    Operator op;
    bool isX;
    
    Expression* left;
    Expression* right;

public:
    Expression(const string& id);
    Expression(const string& id, const string& token, map<string, string>& tokens);

    double evaluate(double x);
    
};


Expression::Expression(const string& id)
: id(id), value(0), left(nullptr), right(nullptr), isX(false)
{ }

Expression::Expression(const string& id, const string& token, map<string, string>& tokens)
: id(id), value(0), left(nullptr), right(nullptr), isX(false)
{ 
    istringstream stream;
    stream.str(token);

    string op, element1, element2;
    stream >> op >> element1 >> element2;

    if(op.empty())
    {
        cerr << "Invalid expression format: empty line." << endl;
        exit(EXIT_FAILURE);
    }

    if(element1.empty())
    // there is only one parameter
    {
        if(op == "x")
        {
            this->isX = true;
        }
        else
        {
            this->value = toDouble(op);
        }
    }
    else
    // two parameters
    {
        if(op == "add")
        {
            this->op = Operator::Add;
        }
        else if(op == "mul")
        {
            this->op = Operator::Mul;
        }
        else if(op == "div")
        {
            this->op = Operator::Div;
        }
        else if(op == "sub")
        {
            this->op = Operator::Sub;
        }
        else 
        {
            cerr << "Invalid operation \"" << op << "\"." << endl;
            exit(EXIT_FAILURE);
        }

        // element1
        if(tokens.find(element1) == tokens.end())
        {
            throw runtime_error("Looping espression.");    
        }

        string nextExpression1 = tokens.find(element1)->second;
        if(nextExpression1.find("add") != string::npos || nextExpression1.find("mul") != string::npos || nextExpression1.find("div") != string::npos || nextExpression1.find("sub") != string::npos)
        {
            tokens.erase(element1);
        }
        
        this->left = new Expression(element1, nextExpression1, tokens);

        // element2
        if(tokens.find(element2) == tokens.end())
        {
            throw runtime_error("Looping espression."); 
        }

        string nextExpression2 = tokens.find(element2)->second;
        if(nextExpression2.find("add") != string::npos || nextExpression2.find("mul") != string::npos || nextExpression2.find("div") != string::npos || nextExpression2.find("sub") != string::npos)
        {
            tokens.erase(element2);
        }

        this->right = new Expression(element2, nextExpression2, tokens);
    }
}

double Expression::evaluate(double x)
{
    if(this->isX)
    {
        return x;
    }

    if(this->left == nullptr && this->right == nullptr)
    {
        return this->value;
    }
    else if(this->left == nullptr || this->right == nullptr)
    {
        cerr << "Invalid expression" << endl;
        exit(EXIT_FAILURE);
    }

    if(this->op == Operator::Add)
    {
        return this->left->evaluate(x) + this->right->evaluate(x);
    }
    else if(this->op == Operator::Mul)
    {
        return this->left->evaluate(x) * this->right->evaluate(x);
    }
    else if(this->op == Operator::Div)
    {
        return this->left->evaluate(x) / this->right->evaluate(x);
    }
    else if(this->op == Operator::Sub)
    {
        return this->left->evaluate(x) - this->right->evaluate(x);
    }

    cerr << "Invalid operator" << endl;
    exit(EXIT_FAILURE);

    return 0;
}

// the fitness with its two dimensions
struct Fitness
{
public:
    // the lower is the Mean Square Error, the higher will be the fitness
    double error;
    // the shorter is the expression, the higher will be the fitness
    double size;
};

// the target function that has to be found
double function(double x)
{
    return 5.7 * x * x + 0.4 * x;
}


double evaluateExpression(unique_ptr<Expression> expression)
{
  //  cout << "Evaluating expression ... " << endl;

    // start from the lower bound of the evaluation interval (xMin)
    double x = xMin;
    double error = 0;
    while(x < xMax)
    // move forward until the upper bound is reached (xMax)
    {
        // get the exact value of the target function in the point x
        double expected = function(x);
   
        // get the value of the expression in the point x
        double obtained = expression->evaluate(x);

        // get the square error
        error += pow(expected - obtained, 2);

        // step forward
        x += xStep;
    }

    // use the inverse of the MSE in order to have values in (0, 1]
    return 1.0 / (1.0 + error);
}

// Transform the lines of a file into an expression tree
unique_ptr<Expression> parseExpression(vector<string>& lines)
{
    // build a map separating the label from the body of the expression 
    map<string, string> expressions; 
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        const string& line = lines[i];

	// get the size of the label
        string::size_type position = line.find(":");
        assert(position != string::npos);

	// split the line
        const string label = line.substr(0, position);  // substring before ":"
        string expression = line.substr(position + 1);  // substring after ":"

        // store it in the map
        expressions[label] = expression;
    }

    // search for the start element ("start: <firstLabel>")
    map<string, string>::iterator iterator = expressions.find("start");
    if(iterator == expressions.end())
    {
        cerr << "Invalid expression: no start element found." << endl;
        exit(EXIT_FAILURE);
    }

    // get the start element ("<firstLabel>")
    const string startElement = iterator->second;

    // erase it from the map
    expressions.erase("start");

    // remove leading/trailing spaces
    string startElementTrimmed;
    istringstream stream;
    stream.str(startElement);
    stream >> startElementTrimmed;

    // search for the first element ("<firstLabel>: <firstExpression>")
    iterator = expressions.find(startElementTrimmed);
    if(iterator == expressions.end())
    {
        cerr << "Invalid expression: no first element found." << endl;
        exit(EXIT_FAILURE);
    }
    
    // get the first expression ("<firstExpression>")
    string startExpression = iterator->second;

    // erase it from the map
    expressions.erase(startElementTrimmed);

    // recursively create the expression tree:
    // The root expression element is called "start"
    return unique_ptr<Expression>( new Expression("start", startExpression, expressions) );
}

vector<string> readFile(const string& fileName)
{
    // open the file for reading
    ifstream stream;
    stream.open(fileName.c_str());
    if(stream.is_open() == false)
    {
        cerr << "Could not open input file \"" << fileName << "\"." << endl;
        exit(EXIT_FAILURE);
    }

    // store here all the lines
    vector<string> lines;
    
    // load all the lines of the file
    char* buffer = new char[1000];
    while(stream.eof() == false)
    {
        stream.getline(buffer, 1000);
        string line = buffer;

        // Discard empty or unlabeled lines.
	// Every line is an expression element and every element is referenced by
	// other elements of the expression tree. 
        // Elements that are not referenced do not carry any label
        // and must not be considered.
        if(line.empty() == false && line.find(":") != string::npos)
        {
           lines.push_back(line);
        }
    }
    delete buffer;
    
    // close the input file
    stream.close();

    return lines;
}

Fitness getFitness(const string& fileName)
{
    // read all the lines from the file
    vector<string> lines = readFile(fileName);

    // compute the fitness
    Fitness fitness;

    // one of the dimensions of the fitness is the length of the expression:
    // since there is one element per line, the length of the expression is equal to the 
    // number of lines
    unsigned int expressionLength = lines.size();

    // the shorter is the expression the higher will be its fitness
    fitness.size = 1.0/expressionLength;

    try
    // now compute the main fitness dimension
    {
        // build the expression tree from the input lines
        unique_ptr<Expression> expression = parseExpression(lines);

        // evaluate the expression and get the Mean Square Error as
        // a measure of the fitness
        fitness.error = evaluateExpression(expression);
    }
    catch(exception& e)
    {
        cerr << "WARNING: " <<  e.what() << endl;
        fitness.size = fitness.error = 0;
    }

    // return the given fitness
    return fitness;
}   

int main (unsigned int argCount, const char** args)
{
    if(argCount <= 1)
    {
        cerr << "Not enough commandline arguments." << endl;
        exit(EXIT_FAILURE);
    }

    // open the output file
    ofstream output;
    output.open("fitness.output");
    if(output.is_open() == false)
    {
        cerr << "Could not open output file." << endl;
        exit(EXIT_FAILURE);
    }

    // open and evaluate each input file
    for(unsigned int i = 1; i < argCount; i++)
    {
        // get the i-th file name
        const string fileName = args[i];

        // get the fitness for the file
        Fitness fitness = getFitness(fileName);

        // save it on a line of the output file
        output.precision(15);
        output << fitness.error << " " << fitness.size << endl;

        // also print the fitness on the console
        // cout << "fitness " << fitness.error << " \t" << fitness.size << endl;
    }

    // close the output file
    output.close();

    return EXIT_SUCCESS;
}
