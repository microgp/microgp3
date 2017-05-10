#include "CMAES.h"

// standard classes
#include <cmath>

// ugp3 classes
#include "Exception.h"
#include "Exceptions/SchemaException.h"
#include "Log.h"
#include "tinyxml.h"

using namespace std;

// XML tags for the configuration file
const string CMAES::XML_PARAMETERS = "parameters";
const string CMAES::XML_ATTRIBUTE_VALUE = "value";

const string CMAES::XML_DIMENSION = "dimension";
const string CMAES::XML_INITIALX = "initialX";
const string CMAES::XML_TYPICALX = "typicalX";
const string CMAES::XML_INITIALSTANDARDDEVIATIONS = "initialStandardDeviations";

const string CMAES::XML_LAMBDA = "lambda";
const string CMAES::XML_RESTARTS = "restarts";
const string CMAES::XML_RESTARTINCREASERATIO = "restartIncreaseRatio";
const string CMAES::XML_STOPMAXFUNEVALS = "stopMaxFunEvals";
const string CMAES::XML_STOPMAXITER = "stopMaxIter";
const string CMAES::XML_STOPTOLFUN = "stopTolFun";
const string CMAES::XML_STOPTOLFUNHIST = "stopTolFunHist";
const string CMAES::XML_STOPTOLX = "stopTolX";
const string CMAES::XML_STOPTOLUPXFACTOR = "stopTolUpXFactor";

const string CMAES::XML_SEED = "seed";

const string CMAES::XML_EVALUATOR = "evaluator";
const string CMAES::XML_PATH = "path";
const string CMAES::XML_INPUTFILE = "inputFile";
const string CMAES::XML_OUTPUTFILE = "outputFile";

const string CMAES::XML_LOGGING = "logging";
const string CMAES::XML_LOG = "log";
const string CMAES::XML_ATTRIBUTE_NAME = "name";
const string CMAES::XML_ATTRIBUTE_LEVEL = "level";

// not really used, so far
const string CMAES::XML_DIFFMINCHANGE = "diffMinChange";
const string CMAES::XML_MAXTIMEFRACTIONFOREIGENDECOMPOSTION = "maxTimeFractionForEigendecompostion"; // yes, it's spelled uncorrectly in the original...
const string CMAES::XML_UPDATECOV = "updatecov";
const string CMAES::XML_FACUPDATECOV = "facUpdatecov";
const string CMAES::XML_RESUME = "resume";
const string CMAES::XML_PARALLELEVALUATIONS = "parallelEvaluations";

// utility function, used to parse text
void Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
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

// read an XML file
void CMAES::readXml(string fileName) throw()
{
	_STACK;

	LOG_DEBUG 	<< "Parsing XML file \"" << fileName << "\""
			<< ends;
	
	// load the document
	xml::Document configurationFile;
	try
	{
		configurationFile.LoadFile( fileName );
	}
	catch(const ugp3::Exception& e)
	{
		LOG_ERROR << "File \"" << fileName << "\" does not seem to be a valid XML file..." << ends;
		throw e;
	}
	
	// these two tokens are needed because there's some post-processing that assumes all other values have been read
	vector<string> initialXtokens;
	vector<string> typicalXtokens;
	vector<string> initialSTDtokens;

	// take the root
	xml::Element* rootElement = configurationFile.RootElement();
	if( rootElement->ValueStr() != XML_PARAMETERS )
	{
		throw xml::SchemaException("Expected element '" + XML_PARAMETERS + "' (found '" + rootElement->ValueStr() + "')", LOCATION);
	}
	
	// and now, it's TIME TO READ
	xml::Element* element = rootElement->FirstChildElement();
	while( element != nullptr )
	{
		const string elementName = element->ValueStr();
		if( elementName == XML_LOGGING )
		{
			// logging context should be analyzed first
			xml::Element* childElement = element->FirstChildElement();
			while( childElement != nullptr )
			{
				const string childElementName = childElement->ValueStr();
				if( childElementName == XML_LOG )
				{
					// open the log with the specified option
					string outputName = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_NAME);
					string outputLevel = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_LEVEL);
					
					// TODO: maybe this part needs to be re-thought...
				
					// debug
					LOG_DEBUG 	<< "Opening console named \"" << outputName 
							<< "\" with level \"" << outputLevel << "\"..." << ends;
				} 
				childElement = childElement->NextSiblingElement();
			}
		}
		else if( elementName == XML_DIMENSION ) 
		{
			this->N = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_DIMENSION << " = " << this->N << ends;
		}
		else if( elementName == XML_INITIALX )
		{
			// this is tricky: the initialX is a set of numbers, that will be repeated inside the array
			string initialXstring = xml::Utility::attributeValueToString(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_INITIALX << " string is \"" << initialXstring << "\"" << ends;
			
			// parse the string
			Tokenize(initialXstring, initialXtokens, " ");
		}
		else if( elementName == XML_TYPICALX )
		{
			// this is tricky: the typicalX is a set of numbers, that will be repeated inside the array
			string typicalXstring = xml::Utility::attributeValueToString(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_TYPICALX << " string is \"" << typicalXstring << "\"" << ends;
			
			// parse the string
			Tokenize(typicalXstring, typicalXtokens, " ");
		}
		else if( elementName == XML_INITIALSTANDARDDEVIATIONS )
		{
			// this is tricky: the standard deviations are a set of numbers, that will be repeated inside the array
			string initialSTDstring = xml::Utility::attributeValueToString(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG 	<< XML_INITIALSTANDARDDEVIATIONS 
					<< " string is \"" << initialSTDstring << "\"" << ends;
			
			// parse the string
			Tokenize(initialSTDstring, initialSTDtokens, " ");
		}
		else if( elementName == XML_LAMBDA )
		{
			this->lambda = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_LAMBDA << " = " << this->lambda << ends;
			// also, mu is always 1/2 * lambda
			this->mu = this->lambda / 2;
		}
		else if( elementName == XML_RESTARTS )
		{
			this->restarts = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_RESTARTS << " = " << this->restarts << ends;
		}
		else if( elementName == XML_RESTARTINCREASERATIO )
		{
			this->restartIncreaseRatio = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_RESTARTINCREASERATIO << " = " << this->restartIncreaseRatio << ends;
		}
		else if( elementName == XML_STOPMAXFUNEVALS )
		{
			this->stopMaxFunEvals = xml::Utility::attributeValueToDouble(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPMAXFUNEVALS << " = " << this->stopMaxFunEvals << ends;
		}
		else if( elementName == XML_STOPMAXITER )
		{
			this->stopMaxIter = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPMAXITER << " = " << this->stopMaxIter << ends;
		}
		else if( elementName == XML_STOPTOLFUN )
		{
			this->stopTolFun = xml::Utility::attributeValueToDouble(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPTOLFUN << " = " << this->stopTolFun << ends;
		}
		else if( elementName == XML_STOPTOLFUNHIST )
		{
			this->stopTolFunHist = xml::Utility::attributeValueToDouble(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPTOLFUNHIST << " = " << this->stopTolFunHist << ends;
		}
		else if( elementName == XML_STOPTOLX )
		{
			this->stopTolX = xml::Utility::attributeValueToDouble(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPTOLX << " = " << this->stopTolX << ends;
		}
		else if( elementName == XML_STOPTOLUPXFACTOR )
		{
			this->stopTolUpXFactor = xml::Utility::attributeValueToDouble(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_STOPTOLUPXFACTOR << " = " << this->stopTolUpXFactor << ends;
		}
		// seed
		else if( elementName == XML_SEED )
		{
			this->seed = xml::Utility::attributeValueToUInt(*element, XML_ATTRIBUTE_VALUE);
			LOG_DEBUG << XML_SEED << " = " << this->seed << ends;
		}
		// evaluator
		else if( elementName == XML_EVALUATOR )
		{
			// create child element and parse stuff
			LOG_DEBUG << "Parsing element \"" << elementName << "\"..." << ends; 
			xml::Element* childElement = element->FirstChildElement();
			
			while( childElement != nullptr )
			{
				const string childElementName = childElement->ValueStr();
				if( childElementName == XML_PATH )
				{
					this->evaluatorPath = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE);
				}
				else if( childElementName == XML_INPUTFILE )
				{
					this->evaluatorInputFile = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE);
				}
				else if( childElementName == XML_OUTPUTFILE )
				{
					this->evaluatorOutputFile =xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE);
				}
				else if( childElementName == XML_PARALLELEVALUATIONS )
				{
					this->evaluatorParallelEvaluations = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE);
				}
				childElement = childElement->NextSiblingElement();
			}
			
			// some debug output
			LOG_DEBUG 	<< "Evaluator path is \"" << this->evaluatorPath << "\", input file pattern is \"" 
					<< this->evaluatorInputFile << "\", output file pattern is \"" << this->evaluatorOutputFile
					<< "\", number of parallel evaluation is " << this->evaluatorParallelEvaluations << ends;
		}

		element = element->NextSiblingElement();
	}
	
	// allocate memory for typicalX and the rest
	LOG_DEBUG << "Allocating memory..." << ends;
	this->xstart = (double *)malloc(sizeof(double) * N);
	this->typicalX = (double *)malloc(sizeof(double) * N);
	this->rgInitialStds = (double *)malloc(sizeof(double) * N);

	LOG_DEBUG << "Filling vectors..." << ends;
	// now, after reading all values, some checks are needed
	if( this->N % initialXtokens.size() != 0 || initialXtokens.size() == 1 ) // this->N != 1 used because this->N % 1 == 0
	{
		LOG_WARNING << "The size of initialX specified cannot fit exactly in the problem dimension. Only the first value specified by initialX (" << initialXtokens[0] << ") will be used." << ends;
		
		//initialXvalues.push_back( atof( initialXtokens[0].c_str() ) );
		for(unsigned int i = 0; i < this->N; i++)
			this->xstart[i] = atof( initialXtokens[0].c_str() );
	}
	else
	{
		for(unsigned int i = 0; i < this->N; i++)
			this->xstart[i] = atof( initialXtokens[i].c_str() );
	}
	
	if( this->N % typicalXtokens.size() != 0 || typicalXtokens.size() == 1 )
	{
		LOG_WARNING << "The size of typicalX specified cannot fit exactly in the problem dimension. Only the first value specified by typicalX (" << typicalXtokens[0] << ") will be used." << ends;
		
		//typicalXvalues.push_back( atof( typicalXtokens[0].c_str() ) );
		for(unsigned int i = 0; i < this->N; i++)
			this->typicalX[i] = atof( typicalXtokens[0].c_str() );
	}
	else
	{
		for(unsigned int i = 0; i < this->N; i++)
			this->typicalX[i] = atof( typicalXtokens[i].c_str() );

	}
	
	if( this->N % initialSTDtokens.size() != 0 || initialSTDtokens.size() == 1 )
	{
		LOG_WARNING << "The size of the initial standard deviations specified cannot fit exactly in the problem dimension. Only the first value (" << initialSTDtokens[0] << ") will be used." << ends;
		
		//initialSTDvalues.push_back( atof( initialSTDtokens[0].c_str() ) );
		for(unsigned int i = 0; i < this->N; i++)
			this->rgInitialStds[i] = atof( initialSTDtokens[0].c_str() );
	}
	else
	{
		for(unsigned int i = 0; i < this->N; i++)
			this->rgInitialStds[i] = atof( initialSTDtokens[i].c_str() );
	}

	for(unsigned int i = 0; i < this->N; i++)
		LOG_DEBUG << "xstart[" << i << "] = " << this->xstart[i] << ends;
	for(unsigned int i = 0; i < this->N; i++)
		LOG_DEBUG << "typicalX[" << i << "] = " << this->typicalX[i] << ends;
	for(unsigned int i = 0; i < this->N; i++)
		LOG_DEBUG << "rgInitialStds[" << i << "] = " << this->rgInitialStds[i] << ends;

	// this part is taken from cmaes_init() and should
	// be completed
	if (this->N == 0)
	{
		// TODO: raise exception
	}

	if (this->xstart == nullptr && this->typicalX == nullptr)
	{
    		// TODO: this is actually an error, if everything is undefinded, it should crash here
    		LOG_WARNING << "initialX undefined. typicalX = 0.5...0.5 will be used." << ends;
  	}
  	
	if (this->rgInitialStds == nullptr) 
	{
    		/* FATAL("initialStandardDeviations undefined","","",""); */
    		// TODO: this is actually an error, if everything is undefinded, it should crash here
    			LOG_WARNING << "Warning: initialStandardDeviations undefined. 0.3...0.3 will be used." << ends;
  	}

  	if (this->xstart == nullptr)
  	{
    		this->xstart = new_double(this->N);

    		/* put inxstart into xstart */
    		/*if(inxstart != nullptr)
    		{ 
      			for (unsigned int i = 0; i < N; ++i)
        			this->xstart[i] = inxstart[i];
    		}*/
		this->typicalXcase = 1;
		for (unsigned int i = 0; i < N; ++i)
			this->xstart[i] = (this->typicalX == nullptr) ? 0.5 : this->typicalX[i]; 
  	} /* xstart == nullptr */
  
	if (this->rgInitialStds == nullptr) 
	{
    		this->rgInitialStds = new_double(N);
    		for (unsigned int i = 0; i < N; ++i)
      			this->rgInitialStds[i] = 0.3;
  	}

	// after all these checks, provide some default values for unspecified variables and derived ones. This procedure was previously performed by the function this->readpara_SupplementDefaults();
	// TODO some variables need to be initialized here!
	//this->facmaxeval = 1;
	// TODO this should be read in the 
	//this->weigkey = (char *)new_void(7, sizeof(char)); 
	//strcpy(this->weigkey, "log"); // default is log

	double t1, t2;
	clock_t cloc = clock();

	if (this->seed < 1) 
	{
		while ((int) (cloc - clock()) == 0); /* TODO: remove this for time critical applications!? */
		this->seed = (unsigned int)abs(100*time(nullptr)+clock());
	}

	if (this->stStopFitness.flg == -1)
		this->stStopFitness.flg = 0;

	if (this->lambda < 2)
		this->lambda = 4+(int)(3*log((double)N));
	
	if (this->mu == -1)
	{
		this->mu = this->lambda/2; 
		this->readpara_SetWeights(this->weigkey);
	}

	if (this->weights == nullptr)
		this->readpara_SetWeights(this->weigkey);

	if (this->cs > 0) /* factor was read */
		this->cs *= (this->mueff + 2.) / (N + this->mueff + 3.);
	if (this->cs <= 0 || this->cs >= 1)
		this->cs = (this->mueff + 2.) / (N + this->mueff + 3.);

	if (this->ccumcov <= 0 || this->ccumcov > 1)
		this->ccumcov = 4. / (N + 4);

	if (this->mucov < 1) 
	{
		this->mucov = this->mueff;
	}

	t1 = 2. / ((N+1.4142)*(N+1.4142));
	t2 = (2.*this->mueff-1.) / ((N+2.)*(N+2.)+this->mueff);
	t2 = (t2 > 1) ? 1 : t2;
	t2 = (1./this->mucov) * t1 + (1.-1./this->mucov) * t2;
	
	if (this->ccov >= 0) /* ccov holds the read factor */
		this->ccov *= t2;
	
	if (this->ccov < 0 || this->ccov > 1) /* set default in case */
		this->ccov = t2;

	if (this->diagonalCov == -1)
		this->diagonalCov = 2 + 100. * N / sqrt((double)this->lambda); 

	if (this->stopMaxFunEvals == -1)  /* may depend on ccov in near future */
		this->stopMaxFunEvals = this->facmaxeval*900*(N+3)*(N+3); 
	//else // to avoid overflow errors
	//	this->stopMaxFunEvals *= this->facmaxeval;

	if (this->stopMaxIter == -1)
		this->stopMaxIter = ceil((double)(this->stopMaxFunEvals / this->lambda)); 

	if (this->damps < 0) 
		this->damps = 1; /* otherwise a factor was read */

	this->damps = this->damps * (1 + 2*douMax(0., sqrt((this->mueff-1.)/(N+1.)) - 1))     /* basic factor */
	* douMax(0.3, 1. -                                       /* modify for short runs */
		  (double)N / (1e-6+douMin(this->stopMaxIter, this->stopMaxFunEvals/this->lambda))) 
	+ this->cs;                                                 /* minor increment */

	if (this->updateCmode.modulo < 0)
		this->updateCmode.modulo = 1./this->ccov/(double)(N)/10.;
	
	this->updateCmode.modulo *= this->facupdateCmode;
	
	if (this->updateCmode.maxtime < 0)
		this->updateCmode.maxtime = 0.20; /* maximal 20% of CPU-time */
	
	return;
}

void CMAES::writeCSV(string fileName) throw()
{

}
