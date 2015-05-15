// CMA-ES class

// related classes
#include "CMAES.h"
#include "RandomT.h"
#include "TimingsT.h"

// standard classes
#include <cfloat>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ugp3 classes
#include "Log.h"
#include "ConsoleHandler.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"

// interesting idea: define macros that will be used as overloads
// of LOG_*, in order to put CMA-ES:: in front of it
// TODO: it does not work...
#define CMAES_INFO  "LOG_INFO << \"CMA-ES::\""
#define CMAES_DEBUG "LOG_DEBUG << \"CMA-ES::\""
#define CMAES_ERROR "LOG_ERROR << \"CMA-ES::\""

// TODO: change the things below to parameters read from file
#define FILE_INPUT "input.txt"
#define FILE_FITNESS "fitness.txt"
#define EVALUATOR "./example"
#define SEPARATOR ";"

using namespace std;

/* dirty index sort */
static void Sorted_index(const double *rgFunVal, int *iindex, int n)
{
  int i, j;
  for (i=1, iindex[0]=0; i<n; ++i) {
    for (j=i; j>0; --j) {
      if (rgFunVal[iindex[j-1]] < rgFunVal[i])
        break;
      iindex[j] = iindex[j-1]; /* shift up */
    }
    iindex[j] = i; /* insert i */
  }
}

char *szCat(const char *sz1, const char*sz2, 
            const char *sz3, const char *sz4)
{
  static char szBuf[700];

  if (!sz1){};
    //FATAL("szCat() : Invalid Arguments",0,0,0);

  strncpy ((char *)szBuf, sz1, (unsigned)intMin( (int)strlen(sz1), 698));
  szBuf[intMin( (int)strlen(sz1), 698)] = '\0';
  if (sz2)
    strncat ((char *)szBuf, sz2, 
             (unsigned)intMin((int)strlen(sz2)+1, 698 - (int)strlen((char const *)szBuf)));
  if (sz3)
    strncat((char *)szBuf, sz3, 
            (unsigned)intMin((int)strlen(sz3)+1, 698 - (int)strlen((char const *)szBuf)));
  if (sz4)
    strncat((char *)szBuf, sz4, 
            (unsigned)intMin((int)strlen(sz4)+1, 698 - (int)strlen((char const *)szBuf)));
  return (char *) szBuf;
}

void ERRORMESSAGE( char const *s1, char const *s2, 
                   char const *s3, char const *s4)
{
  /*  static char szBuf[700];  desirable but needs additional input argument 
      sprintf(szBuf, "%f:%f", gen, gen*lambda);
  */
  time_t t = time(nullptr);
  FILE *fp = fopen( "errcmaes.err", "a");
  if (!fp)
    {
      //printf("\nFATAL ERROR: %s\n", s2 ? szCat(s1, s2, s3, s4) : s1);
      printf("cmaes_t could not open file 'errcmaes.err'.");
      printf("\n *** CMA-ES ABORTED *** ");
      fflush(stdout);
      exit(1);
    }
  fprintf( fp, "\n -- %s %s\n", asctime(localtime(&t)), 
           s2 ? szCat(s1, s2, s3, s4) : s1);
  fclose (fp);
}

static char* getTimeStr(void)
{
  time_t tm = time(nullptr);
  static char s[33];

  /* get time */
  strncpy(s, ctime(&tm), 24); /* TODO: hopefully we read something useful */
  s[24] = '\0'; /* cut the \n */
  return s;
}

static double myhypot(double a, double b) 
/* sqrt(a^2 + b^2) numerically stable. */
{
  double r = 0;
  if (fabs(a) > fabs(b)) {
    r = b/a;
    r = fabs(a)*sqrt(1+r*r);
  } else if (b != 0) {
    r = a/b;
    r = fabs(b)*sqrt(1+r*r);
  }
  return r;
}


/* TODO: 	- write on LOG_INFO, LOG_DEBUG
		- allocate memory in a meaningful way (replacing all new_void and new_double with <vector> and the like
		- replace horrible char* stuff with string
*/
CMAES::CMAES() : 
N(0),
evaluatorParallelEvaluations(1)
{
	// initialization function
	this->initialize();
}

CMAES::~CMAES()
{}

// evaluate fitness
double CMAES::fitnessEvaluation(double const* x, int n) throw()
{
	int i, returnValue;
	double fitness = DBL_MAX; /* max value in double */
	string evaluatorCommandLine;
	FILE *fp;
	
	// TODO: 	- create unique IDs for individuals
	//		- manage parallel evaluations
	//		- put individual syntax somewhere

	/* remove old input and fitness files */
	//sprintf(buffer, "rm %s %s 2>/dev/null", FILE_INPUT, FILE_FITNESS);
	//returnValue = system(buffer);
	
	evaluatorCommandLine = this->evaluatorPath;
	for(unsigned int i = 0; i < this->evaluatorParallelEvaluations; i++)
	{
		string individualName = this->evaluatorInputFile; // + uniqueID

		/* create input file for evaluator */
		if( (fp = fopen(individualName.c_str(), "w")) == nullptr)
		{
			//LOG_ERROR << "Error while writing file \"" <<  << << "\"" << ends;
			throw ugp3::Exception("Error while writing on file \"" + individualName + "\".", LOCATION);
		}	

		for(i = 0; i < n; i++)
		{
			fprintf(fp, "%g", x[i]);
			if( i != n-1 ) fprintf(fp, "%s", SEPARATOR);
		}
		fclose(fp);
		
		// put individual name in command line
		evaluatorCommandLine += " " + individualName;
	}

	/* run evaluator */
	returnValue = system( evaluatorCommandLine.c_str() );

	if( returnValue != 0)
	{
		LOG_WARNING << "Process \"" << evaluatorCommandLine << "\" exit with return value " << returnValue << ends;
	}

	/* gather and return fitness value */
	if( (fp = fopen(this->evaluatorOutputFile.c_str(), "r")) == nullptr)
	{
		LOG_ERROR << "Error while reading file \""FILE_FITNESS"\": cannot open the file" << ends;
		exit(-1); /* comment this if the evaluator can produce a non text values */
	}
	else
	{
		// TODO: fitness reading loop here
		if( fscanf(fp, "%lf", &fitness) != 1)
		{
			throw ugp3::Exception("Error while reading file \"" + this->evaluatorOutputFile + "\": wrong output format.", LOCATION);
		}
	}
	fclose(fp);
	
	// TODO: remove temporary files

	return fitness;
}

void CMAES::run() throw()
{
	double *arFunvals, *const* pop, *xfinal;
	int i, j; 

	
	LOG_INFO << "Setting up evolutionary process..." << ends;
	// TODO: I commented here
	arFunvals = this->cmaes_init(0, nullptr, nullptr, 0, 0, "initials.par"); 
	LOG_INFO << this->cmaes_SayHello() << ends;

	this->cmaes_ReadSignals("signals.par");  /* write header and initial values */
	
	int generation = 0;
	/*
	char filename[256];
	sprintf(filename, "cmaes-all-population.csv", generation); // TODO modify here to write/unwrite all population
	FILE* fp = fopen(filename, "a");
	if( fp == nullptr )
	{
		fprintf(stderr, "Error: cannot write on generation file %s!\n", filename);
		return -1;
	}
	*/
	
	while( !this->cmaes_TestForTermination() )
	{ 
		/* generate lambda new search points, sample population */
		LOG_DEBUG << "Sampling population..." << ends;
		pop = this->cmaes_SamplePopulation(); /* do not change content of pop */

		LOG_DEBUG << "Evaluating individuals..." << ends;
		for (unsigned int i = 0; i < this->cmaes_Get("lambda"); ++i) 
		{
			arFunvals[i] = this->fitnessEvaluation(pop[i], (int) this->cmaes_Get("dim"));
		}

		/* update the search distribution used for cmaes_SampleDistribution() */
		LOG_DEBUG << "Updating distribution..." << ends;
		this->cmaes_UpdateDistribution(arFunvals);  

		/* read instructions for printing output or changing termination conditions */ 
		LOG_DEBUG << "Reading signals file..." << ends;
		this->cmaes_ReadSignals("signals.par");   
		fflush(stdout); /* useful in MinGW */
		
		/* update generations */
		generation++;
	}

	LOG_INFO << "Termination condition reached: " << this->cmaes_TestForTermination() << ends; /* print termination reason */
	this->cmaes_WriteToFile("all", "allcmaes.dat");         /* write final results */

	/* get best estimator for the optimum, xmean */
	xfinal = this->cmaes_GetNew("xmean"); /* "xbestever" might be used as well */
	this->cmaes_exit(); /* release memory */ 

	/* do something with final solution and finally release memory */
	string bestIndividualFileName = "best.individual.txt";
	LOG_INFO << "Saving the best solution as \"" << bestIndividualFileName << "\"..." << ends;
	
	ofstream bestIndividualFile(bestIndividualFileName.c_str());
	if( bestIndividualFile.is_open() )
	{
		for(unsigned int i = 0; i < this->N; i++)
			bestIndividualFile << xfinal[i] << ";";
	}
	bestIndividualFile.close();

	free(xfinal); 
}

// this function aims at replacing readpara_init, initializing all variables 
// to default values, before starting to read the configuration file
void CMAES::initialize()
{
	this->N = 0;
	this->seed = 0; 
	this->xstart = nullptr; 
	this->typicalX = nullptr;
	this->typicalXcase = 0;
	this->rgInitialStds = nullptr; 
	this->rgDiffMinChange = nullptr; 
	this->stopMaxFunEvals = -1;
	this->stopMaxIter = -1;
	this->facmaxeval = 1; 
	this->stStopFitness.flg = -1;
	this->stopTolFun = 1e-12; 
	this->stopTolFunHist = 1e-13; 
	this->stopTolX = 0; /* 1e-11*insigma would also be reasonable */ 
	this->stopTolUpXFactor = 1e3; 

	this->lambda = lambda;
	this->mu = -1;
	this->mucov = -1;
	this->weights = nullptr;
	this->weigkey = (char *)new_void(7, sizeof(char)); 
	strcpy(this->weigkey, "log"); // this line is still mysterious

	this->cs = -1;
	this->ccumcov = -1;
	this->damps = -1;
	this->ccov = -1;
	
	this->diagonalCov = 0; /* default is 0, but this might change in future, see below */

	this->updateCmode.modulo = -1;  
	this->updateCmode.maxtime = -1;
	this->updateCmode.flgalways = 0;
	this->facupdateCmode = 1;
	strcpy(this->resumefile, "_no_");
}

void CMAES::readpara_init (
               int dim, 
               int inseed, 
               const double * inxstart, 
               const double * inrgsigma,
               int lambda, 
               const char * filename)
{
  int i, N;
  this->rgsformat = (char **) new_void(55, sizeof(char *));
  this->rgpadr = (void **) new_void(55, sizeof(void *)); 
  this->rgskeyar = (char **) new_void(11, sizeof(char *));
  this->rgp2adr = (double ***) new_void(11, sizeof(double **));
  this->weigkey = (char *)new_void(7, sizeof(char)); 

  /* 	AT
	Basically, this piece of code registers for every parameter,
	the format to be read/written for the parameter itself, and
	its address.
	The format is stored inside rgsformat, the address in rgpadr.
	
	This is going to be completely replaced by the XML function.
  */
  /* All scalars:  */
  i = 0;
  this->rgsformat[i] = " N %d";        
  this->rgpadr[i++] = (void *) &this->N; 
  
  this->rgsformat[i] = " seed %d";    
  this->rgpadr[i++] = (void *) &this->seed;
  
  this->rgsformat[i] = " stopMaxFunEvals %lg"; 
  this->rgpadr[i++] = (void *) &this->stopMaxFunEvals;
  
  this->rgsformat[i] = " stopMaxIter %lg"; 
  this->rgpadr[i++] = (void *) &this->stopMaxIter;
  
  this->rgsformat[i] = " stopFitness %lg"; 
  this->rgpadr[i++]=(void *) &this->stStopFitness.val;
  
  this->rgsformat[i] = " stopTolFun %lg"; 
  this->rgpadr[i++]=(void *) &this->stopTolFun;
  
  this->rgsformat[i] = " stopTolFunHist %lg"; 
  this->rgpadr[i++]=(void *) &this->stopTolFunHist;
  
  this->rgsformat[i] = " stopTolX %lg"; 
  this->rgpadr[i++]=(void *) &this->stopTolX;
  
  this->rgsformat[i] = " stopTolUpXFactor %lg"; 
  this->rgpadr[i++]=(void *) &this->stopTolUpXFactor;
  
  this->rgsformat[i] = " lambda %d";      
  this->rgpadr[i++] = (void *) &this->lambda;
  
  this->rgsformat[i] = " mu %d";          
  this->rgpadr[i++] = (void *) &this->mu;
  
  this->rgsformat[i] = " weights %5s";    
  this->rgpadr[i++] = (void *) this->weigkey;
  
  this->rgsformat[i] = " fac*cs %lg";
  this->rgpadr[i++] = (void *) &this->cs;
  
  this->rgsformat[i] = " fac*damps %lg";   
  this->rgpadr[i++] = (void *) &this->damps;
  
  this->rgsformat[i] = " ccumcov %lg";    
  this->rgpadr[i++] = (void *) &this->ccumcov;
  
  this->rgsformat[i] = " mucov %lg";     
  this->rgpadr[i++] = (void *) &this->mucov;
  
  this->rgsformat[i] = " fac*ccov %lg";  
  this->rgpadr[i++]=(void *) &this->ccov;
  
  this->rgsformat[i] = " diagonalCovarianceMatrix %lg"; 
  this->rgpadr[i++]=(void *) &this->diagonalCov;
  
  this->rgsformat[i] = " updatecov %lg"; 
  this->rgpadr[i++]=(void *) &this->updateCmode.modulo;
  
  this->rgsformat[i] = " maxTimeFractionForEigendecompostion %lg"; 
  this->rgpadr[i++]=(void *) &this->updateCmode.maxtime;
  
  this->rgsformat[i] = " resume %59s";    
  this->rgpadr[i++] = (void *) this->resumefile;
  
  this->rgsformat[i] = " fac*maxFunEvals %lg";   
  this->rgpadr[i++] = (void *) &this->facmaxeval;
  
  this->rgsformat[i] = " fac*updatecov %lg"; 
  this->rgpadr[i++]=(void *) &this->facupdateCmode;
  
  this->n1para = i; 
  this->n1outpara = i-2; /* disregard last parameters in WriteToFile() */

  /* 	AT
  	Same as above, the difference is that this part is related
	to the arrays.
  */
  /* arrays */
  i = 0;
  this->rgskeyar[i]  = " typicalX %d";   
  this->rgp2adr[i++] = &this->typicalX;
  
  this->rgskeyar[i]  = " initialX %d";   
  this->rgp2adr[i++] = &this->xstart;
  
  this->rgskeyar[i]  = " initialStandardDeviations %d"; 
  this->rgp2adr[i++] = &this->rgInitialStds;
  
  this->rgskeyar[i]  = " diffMinChange %d"; 
  this->rgp2adr[i++] = &this->rgDiffMinChange;
  this->n2para = i;  

  this->N = dim;
  this->seed = (unsigned) inseed; 
  this->xstart = nullptr; 
  this->typicalX = nullptr;
  this->typicalXcase = 0;
  this->rgInitialStds = nullptr; 
  this->rgDiffMinChange = nullptr; 
  this->stopMaxFunEvals = -1;
  this->stopMaxIter = -1;
  this->facmaxeval = 1; 
  this->stStopFitness.flg = -1;
  this->stopTolFun = 1e-12; 
  this->stopTolFunHist = 1e-13; 
  this->stopTolX = 0; /* 1e-11*insigma would also be reasonable */ 
  this->stopTolUpXFactor = 1e3; 

  this->lambda = lambda;
  this->mu = -1;
  this->mucov = -1;
  this->weights = nullptr;
  strcpy(this->weigkey, "log");

  this->cs = -1;
  this->ccumcov = -1;
  this->damps = -1;
  this->ccov = -1;

  this->diagonalCov = 0; /* default is 0, but this might change in future, see below */

  this->updateCmode.modulo = -1;  
  this->updateCmode.maxtime = -1;
  this->updateCmode.flgalways = 0;
  this->facupdateCmode = 1;
  strcpy(this->resumefile, "_no_");

  if (strcmp(filename, "non") != 0 && strcmp(filename, "writeonly") != 0)
    // TODO: THIS PART HERE should be modified
    this->readpara_ReadFromFile(filename);

  if (this->N <= 0)
    this->N = dim;

  N = this->N; 
  if (N == 0){}
    //FATAL("readpara_readpara_t(): problem dimension N undefined.\n",
    //      "  (no default value available).",0,0); 
  if (this->xstart == nullptr && inxstart == nullptr && this->typicalX == nullptr) {
    // TODO: this is actually an error, if everything is undefinded, it should crash here
    LOG_WARNING << "initialX undefined. typicalX = 0.5...0.5 will be used." << ends;
  }
  if (this->rgInitialStds == nullptr && inrgsigma == nullptr) {
    /* FATAL("initialStandardDeviations undefined","","",""); */
    // TODO: this is actually an error, if everything is undefinded, it should crash here
    ERRORMESSAGE("Warning: initialStandardDeviations undefined. 0.3...0.3 used.","","","");
    printf("\nWarning: initialStandardDeviations. 0.3...0.3 used.\n");
  }

  if (this->xstart == nullptr)
  {
    this->xstart = new_double(N);

    /* put inxstart into xstart */
    if (inxstart != nullptr)
    { 
      for (i=0; i<N; ++i)
        this->xstart[i] = inxstart[i];
    }
    /* otherwise use typicalX or default */
    else
    {
      this->typicalXcase = 1;
      for (i=0; i<N; ++i)
        this->xstart[i] = (this->typicalX == nullptr) ? 0.5 : this->typicalX[i]; 
    }
  } /* xstart == nullptr */
  
  if (this->rgInitialStds == nullptr) {
    this->rgInitialStds = new_double(N);
    for (i=0; i<N; ++i)
      this->rgInitialStds[i] = (inrgsigma == nullptr) ? 0.3 : inrgsigma[i];
  }

  this->readpara_SupplementDefaults();
  if (strcmp(filename, "non") != 0)
    this->readpara_WriteToFile("actparcmaes.par", filename);
} /* readpara_init */

void CMAES::readpara_exit()
{
  if (this->xstart != nullptr) /* not really necessary */
    free( this->xstart);
  if (this->typicalX != nullptr)
    free( this->typicalX);
  if (this->rgInitialStds != nullptr)
    free( this->rgInitialStds);
  if (this->rgDiffMinChange != nullptr)
    free( this->rgDiffMinChange);
  if (this->weights != nullptr)
    free( this->weights);

  free(this->rgsformat);
  free(this->rgpadr);
  free(this->rgskeyar);
  free(this->rgp2adr);
  free(this->weigkey);
}


void CMAES::readpara_SupplementDefaults()
{
  double t1, t2;
  int N = this->N; 
  clock_t cloc = clock();
  
  if (this->seed < 1) {
    while ((int) (cloc - clock()) == 0)
      ; /* TODO: remove this for time critical applications!? */
    this->seed = (unsigned int)abs(100*time(nullptr)+clock());
  }

  if (this->stStopFitness.flg == -1)
    this->stStopFitness.flg = 0;

  if (this->lambda < 2)
    this->lambda = 4+(int)(3*log((double)N));
  if (this->mu == -1) {
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
  
  if (this->mucov < 1) {
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
  //else // lines commented because it is not really relevant, this->facmaxeval == 1
    //this->stopMaxFunEvals *= this->facmaxeval;

  if (this->stopMaxIter == -1)
    this->stopMaxIter = ceil((double)(this->stopMaxFunEvals / this->lambda)); 

  if (this->damps < 0) 
    this->damps = 1; /* otherwise a factor was read */
  this->damps = this->damps 
    * (1 + 2*douMax(0., sqrt((this->mueff-1.)/(N+1.)) - 1))     /* basic factor */
    * douMax(0.3, 1. -                                       /* modify for short runs */
                  (double)N / (1e-6+douMin(this->stopMaxIter, this->stopMaxFunEvals/this->lambda))) 
    + this->cs;                                                 /* minor increment */

  if (this->updateCmode.modulo < 0)
    this->updateCmode.modulo = 1./this->ccov/(double)(N)/10.;
  this->updateCmode.modulo *= this->facupdateCmode;
  if (this->updateCmode.maxtime < 0)
    this->updateCmode.maxtime = 0.20; /* maximal 20% of CPU-time */

} /* readpara_SupplementDefaults() */

void CMAES::readpara_WriteToFile(const char *filenamedest, 
                     const char *filenamesource)
{
  int ipara, i; 
  size_t len;
  time_t ti = time(nullptr);
  FILE *fp = fopen( filenamedest, "a"); 
  if(fp == nullptr) {
    ERRORMESSAGE("cmaes_WriteToFile(): could not open '", 
                 filenamedest, "'",0);
    return;
  }
  fprintf(fp, "\n# Read from %s at %s\n", filenamesource, 
          asctime(localtime(&ti))); /* == ctime() */
  for (ipara=0; ipara < 1; ++ipara)
  {
    fprintf(fp, this->rgsformat[ipara], *(int *)this->rgpadr[ipara]);
    fprintf(fp, "\n");
  }
  for (ipara=0; ipara < this->n2para; ++ipara) 
  {
    if(*this->rgp2adr[ipara] == nullptr)
      continue;
    fprintf(fp, this->rgskeyar[ipara], this->N);
    fprintf(fp, "\n");
    for (i=0; i<this->N; ++i)
      fprintf(fp, "%7.3g%c", (*this->rgp2adr[ipara])[i], (i%5==4)?'\n':' ');
    fprintf(fp, "\n");
  }
  for (ipara=1; ipara < this->n1outpara; ++ipara) 
  {
    if (strncmp(this->rgsformat[ipara], " stopFitness ", 13) == 0)
      if(this->stStopFitness.flg == 0) 
      {
        fprintf(fp, " stopFitness\n");
        continue;
      }
    len = strlen(this->rgsformat[ipara]);
    if (this->rgsformat[ipara][len-1] == 'd') /* read integer */
      fprintf(fp, this->rgsformat[ipara], *(int *)this->rgpadr[ipara]);
    else if (this->rgsformat[ipara][len-1] == 's') /* read string */
      fprintf(fp, this->rgsformat[ipara], (char *)this->rgpadr[ipara]);
    else { 
      if (strncmp(" fac*", this->rgsformat[ipara], 5) == 0) {
        fprintf(fp, " ");
        fprintf(fp, this->rgsformat[ipara]+5, *(double *)this->rgpadr[ipara]);
      } else
        fprintf(fp, this->rgsformat[ipara], *(double *)this->rgpadr[ipara]);
    }
    fprintf(fp, "\n");
  } /* for */
  fprintf(fp, "\n");
  fclose(fp); 
} /* readpara_WriteToFile() */

void CMAES::readpara_ReadFromFile(const char * filename)
{
  char s[1000], *ss = "initials.par";
  int ipara, i;
  int size;
  FILE *fp;

  if (filename == nullptr)
	filename = ss;
  
  fp = fopen( filename, "r"); 
  if(fp == nullptr) 
  {
    ERRORMESSAGE("cmaes_ReadFromFile(): could not open '", filename, "'",0);
    return;
  }

  for (ipara=0; ipara < this->n1para; ++ipara)
  {
	rewind(fp);
        while(fgets(s, sizeof(s), fp) != nullptr)
        { /* skip comments  */
        	if (s[0] == '#' || s[0] == '%')
            	continue;
          	if(sscanf(s, this->rgsformat[ipara], this->rgpadr[ipara]) == 1) 
		{
            		if (strncmp(this->rgsformat[ipara], " stopFitness ", 13) == 0)
              		this->stStopFitness.flg = 1;
            		break;
          	}
        }

    } /* for */

  if (this->N <= 0){}
    //FATAL("readpara_ReadFromFile(): No valid dimension N",0,0,0); 

  for(ipara=0; ipara < this->n2para; ++ipara)
  {
      rewind(fp);
      while(fgets(s, sizeof(s), fp) != nullptr) /* read one line */
      { /* skip comments  */
      	if (s[0] == '#' || s[0] == '%')
        	continue;
          
	if(sscanf(s, this->rgskeyar[ipara], &size) == 1) 
	{ /* size==number of values to be read */
            if (size > 0) 
	    {
		*this->rgp2adr[ipara] = new_double(this->N);
              	for (i=0;i<size&&i<this->N;++i) /* start reading next line */
                	if (fscanf(fp, " %lf", &(*this->rgp2adr[ipara])[i]) != 1)
                  		break;
              
		if (i<size && i < this->N) 
		{
                	ERRORMESSAGE("readpara_ReadFromFile ", filename, ": ",0); 
                //FATAL( "'", this->rgskeyar[ipara], 
                //       "' not enough values found.\n", 
                //       "   Remove all comments between numbers.");
              	}

              	for (; i < this->N; ++i) /* recycle */
                	(*this->rgp2adr[ipara])[i] = (*this->rgp2adr[ipara])[i%size];
            }
          }
        }  
    } /* for */

  fclose(fp);
  return;
} /* readpara_ReadFromFile() */

void CMAES::readpara_SetWeights(const char * mode)
{
	double s1, s2;
	int i;

	if(this->weights != nullptr)
		free( this->weights); 
	
	this->weights = new_double(this->mu);
	
	if (strcmp(mode, "lin") == 0)
		for (i=0; i<this->mu; ++i) 
			this->weights[i] = this->mu - i;
	else if (strncmp(mode, "equal", 3) == 0)
		for (i=0; i<this->mu; ++i) 
			this->weights[i] = 1;
	else if (strcmp(mode, "log") == 0) 
		for (i=0; i<this->mu; ++i) 
			this->weights[i] = log(this->mu+1.)-log(i+1.); 
	else
		for (i=0; i<this->mu; ++i) 
	      		this->weights[i] = log(this->mu+1.)-log(i+1.); 

  /* normalize weights vector and set mueff */
  for (i=0, s1=0, s2=0; i<this->mu; ++i) {
    s1 += this->weights[i];
    s2 += this->weights[i]*this->weights[i];
  }
  this->mueff = s1*s1/s2;
  for (i=0; i<this->mu; ++i) 
    this->weights[i] /= s1;

  if(this->mu < 1 || this->mu > this->lambda || 
     (this->mu==this->lambda && this->weights[0]==this->weights[this->mu-1])){}
    //FATAL("readpara_SetWeights(): invalid setting of mu or lambda",0,0,0);

} /* readpara_SetWeights() */
/* --------------------------------------------------------- */
/* --------------------------------------------------------- */

char* CMAES::cmaes_SayHello()
{
  /* write initial message */
  sprintf(this->sOutString, 
          "(%d,%d)-CMA-ES(mu_eff=%.1f), Ver=\"%s\", dimension=%d, diagonalIterations=%ld, randomSeed=%d (%s)", 
          this->mu, this->lambda, this->mueff, this->version, this->N, (long)this->diagonalCov,
          this->seed, getTimeStr());

  return this->sOutString; 
}

double* CMAES::cmaes_init(/* "this" */
                int dimension, 
                double *inxstart,
                double *inrgstddev, /* initial stds */
                long int inseed,
                int lambda, 
                const char *input_parameter_filename) 
{
  int i, j, N;
  double dtest, trace;

  this->version = "3.11.00.beta";

  // TODO: 	I commented this line, because the parameters should be already provided through
  //		the "real" XML configuration file
  //this->readpara_init (dimension, inseed, inxstart, inrgstddev, lambda, input_parameter_filename);
 
  this->seed = this->rand.random_init( (long unsigned int) this->seed);

  N = this->N; /* for convenience */
  
  /* initialization  */
  for (i = 0, trace = 0.; i < N; ++i)
    trace += this->rgInitialStds[i] * this->rgInitialStds[i];

  this->sigma = sqrt(trace/N); /* this->sp.mueff/(0.2*this->sp.mueff+sqrt(N)) * sqrt(trace/N); */

  this->chiN = sqrt((double) N) * (1. - 1./(4.*N) + 1./(21.*N*N));
  this->flgEigensysIsUptodate = 1;
  this->flgCheckEigen = 0; 
  this->genOfEigensysUpdate = 0;
  this->eigenTimings.timings_init();
  this->flgIniphase = 0; /* do not use iniphase, hsig does the job now */
  this->flgresumedone = 0;
  this->flgStop = 0;

  for (dtest = 1.; dtest && dtest < 1.1 * dtest; dtest *= 2.) 
    if (dtest == dtest + 1.)
      break;

  this->dMaxSignifKond = dtest / 1000.; /* not sure whether this is really save -> safe?, 100 does not work well enough */

  this->gen = 0;
  this->countevals = 0;
  this->state = 0;
  this->dLastMinEWgroesserNull = 1.0;
  this->printtime = this->writetime = this->firstwritetime = this->firstprinttime = 0; 

  this->rgpc = new_double(N);
  this->rgps = new_double(N);
  this->rgdTmp = new_double(N+1);
  this->rgBDz = new_double(N);
  this->rgxmean = new_double(N+2); this->rgxmean[0] = N; ++this->rgxmean;
  this->rgxold = new_double(N+2); this->rgxold[0] = N; ++this->rgxold; 
  this->rgxbestever = new_double(N+3); this->rgxbestever[0] = N; ++this->rgxbestever; 
  this->rgout = new_double(N+2); this->rgout[0] = N; ++this->rgout;
  this->rgD = new_double(N);
  this->C = (double**)new_void(N, sizeof(double*));
  this->B = (double**)new_void(N, sizeof(double*));
  this->publicFitness = new_double(this->lambda); 
  this->rgFuncValue = new_double(this->lambda+1); 
  this->rgFuncValue[0]=this->lambda; ++this->rgFuncValue;
  this->arFuncValueHist = new_double(10+(int)ceil(3.*10.*N/this->lambda)+1);
  this->arFuncValueHist[0] = (double)(10+(int)ceil(3.*10.*N/this->lambda));
  this->arFuncValueHist++; 

  for(i = 0; i < N; ++i) 
  {
      this->C[i] = new_double(i+1);
      this->B[i] = new_double(N);
  }

  this->index = (int *) new_void(this->lambda, sizeof(int));
  for (i = 0; i < this->lambda; ++i) 
    this->index[i] = i; /* should not be necessary */
 
 this->rgrgx = (double **)new_void(this->lambda, sizeof(double*));
 for (i = 0; i < this->lambda; ++i) 
 {
    this->rgrgx[i] = new_double(N+2);
    this->rgrgx[i][0] = N; 
    this->rgrgx[i]++;
  }

  /* Initialize newed space  */

  for (i = 0; i < N; ++i)
    for (j = 0; j < i; ++j)
       this->C[i][j] = this->B[i][j] = this->B[j][i] = 0.;
        
  for (i = 0; i < N; ++i)
  {
      this->B[i][i] = 1.;
      this->C[i][i] = this->rgD[i] = this->rgInitialStds[i] * sqrt(N / trace);
      this->C[i][i] *= this->C[i][i];
      this->rgpc[i] = this->rgps[i] = 0.;
  }

  this->minEW = rgdouMin(this->rgD, N); this->minEW = this->minEW * this->minEW;
  this->maxEW = rgdouMax(this->rgD, N); this->maxEW = this->maxEW * this->maxEW; 

  this->maxdiagC=this->C[0][0]; for(i=1;i<N;++i) if(this->maxdiagC < this->C[i][i]) this->maxdiagC=this->C[i][i];
  this->mindiagC=this->C[0][0]; for(i=1;i<N;++i) if(this->mindiagC > this->C[i][i]) this->mindiagC=this->C[i][i];

  /* set xmean */
  for (i = 0; i < N; ++i)
    this->rgxmean[i] = this->rgxold[i] = this->xstart[i]; 
  
  /* use in case xstart as typicalX */
  if (this->typicalXcase) 
    for (i = 0; i < N; ++i)
      this->rgxmean[i] += this->sigma * this->rgD[i] * this->rand.random_Gauss();

  if (strcmp(this->resumefile, "_no_")  != 0)
    this->cmaes_resume_distribution(this->resumefile);

  return(this->publicFitness); 

} /* cmaes_init() */

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */

void CMAES::cmaes_resume_distribution(char *filename)
{
  int i, j, res, n; 
  double d; 
  FILE *fp = fopen( filename, "r"); 
  if(fp == nullptr) {
    ERRORMESSAGE("cmaes_resume_distribution(): could not open '", 
                 filename, "'",0);
    return;
  }
  /* count number of "resume" entries */
  i = 0; res = 0;
  while (1) {
    if ((res = fscanf(fp, " resume %lg", &d)) == EOF)
      break;
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      i += 1;
  }

  /* go to last "resume" entry */
  n = i; i = 0; res = 0; rewind(fp);
  while (i<n) {
    if ((res = fscanf(fp, " resume %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): Unexpected error, bug",0,0,0); TODO: exception
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      ++i;
  }
  if (d != this->N){};
    //FATAL("cmaes_resume_distribution(): Dimension numbers do not match",0,0,0); TODO: exception
  
  /* find next "xmean" entry */  
  while (1) {
    if ((res = fscanf(fp, " xmean %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): 'xmean' not found",0,0,0); TODO: exception
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      break;
  }
  
  /* read xmean */
  this->rgxmean[0] = d; res = 1; 
  for(i = 1; i < this->N; ++i)
    res += fscanf(fp, " %lg", &this->rgxmean[i]);
  if (res != this->N){};
    //FATAL("cmaes_resume_distribution(): xmean: dimensions differ",0,0,0); TODO: exception

  /* find next "path for sigma" entry */  
  while (1) {
    if ((res = fscanf(fp, " path for sigma %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): 'path for sigma' not found",0,0,0); TODO: exception
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      break;
  }
  
  /* read ps */
  this->rgps[0] = d; res = 1;
  for(i = 1; i < this->N; ++i)
    res += fscanf(fp, " %lg", &this->rgps[i]);
  if (res != this->N){};
    //FATAL("cmaes_resume_distribution(): ps: dimensions differ",0,0,0);TODO: exception 
  
  /* find next "path for C" entry */  
  while (1) {
    if ((res = fscanf(fp, " path for C %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): 'path for C' not found",0,0,0); TODO: exception 
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      break;
  }
  /* read pc */
  this->rgpc[0] = d; res = 1;
  for(i = 1; i < this->N; ++i)
    res += fscanf(fp, " %lg", &this->rgpc[i]);
  if (res != this->N){};
    //FATAL("cmaes_resume_distribution(): pc: dimensions differ",0,0,0); TODO: exception

  /* find next "sigma" entry */  
  while (1) {
    if ((res = fscanf(fp, " sigma %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): 'sigma' not found",0,0,0);TODO: exception 
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      break;
  }
  this->sigma = d;

  /* find next entry "covariance matrix" */
  while (1) {
    if ((res = fscanf(fp, " covariance matrix %lg", &d)) == EOF){}
      //FATAL("cmaes_resume_distribution(): 'covariance matrix' not found",0,0,0);TODO: exception 
    else if (res==0) 
      fscanf(fp, " %*s");
    else if(res > 0)
      break;
  }
  /* read C */
  this->C[0][0] = d; res = 1;
  for (i = 1; i < this->N; ++i)
    for (j = 0; j <= i; ++j)
      res += fscanf(fp, " %lg", &this->C[i][j]);
  if (res != (this->N * this->N + this->N)/2)
    //FATAL("cmaes_resume_distribution(): C: dimensions differ",0,0,0);TODO: exception 
   
  this->flgIniphase = 0;
  this->flgEigensysIsUptodate = 0;
  this->flgresumedone = 1;
  this->cmaes_UpdateEigensystem(1);
  
} /* cmaes_resume_distribution() */
/* --------------------------------------------------------- */
/* --------------------------------------------------------- */

void CMAES::cmaes_exit()
{
  int i, N = this->N;
  this->state = -1; /* not really useful at the moment */
  free( this->rgpc );
  free( this->rgps );
  free( this->rgdTmp );
  free( this->rgBDz );
  free( --this->rgxmean );
  free( --this->rgxold ); 
  free( --this->rgxbestever ); 
  free( --this->rgout ); 
  free( this->rgD );

  for (i = 0; i < N; ++i)
  {
    free( this->C[i]);
    free( this->B[i]);
  }

  for (i = 0; i < this->lambda; ++i) 
    free( --this->rgrgx[i] );

  free( this->rgrgx ); 
  free( this->C );
  free( this->B );
  free( this->index );
  free( this->publicFitness );
  free( --this->rgFuncValue );
  free( --this->arFuncValueHist );
  this->rand.random_exit();
  this->readpara_exit(); 

} /* cmaes_exit() */


/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double const * CMAES::cmaes_SetMean(const double *xmean)
/*
 * Distribution mean could be changed before SamplePopulation().
 * This might lead to unexpected behaviour if done repeatedly. 
 */
{
  int i, N=this->N;

  if (this->state >= 1 && this->state < 3){};
    //FATAL("cmaes_SetMean: mean cannot be set inbetween the calls of ",
    //      "SamplePopulation and UpdateDistribution",0,0); TODO: exception

  if (xmean != nullptr && xmean != this->rgxmean)
    for(i = 0; i < N; ++i)
      this->rgxmean[i] = xmean[i];
  else 
    xmean = this->rgxmean; 

  return xmean; 
}
 
/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * const * CMAES::cmaes_SamplePopulation()
{
  int iNk, i, j, N=this->N;

  int flgdiag = ((this->diagonalCov == 1) || (this->diagonalCov >= this->gen)); 
  double sum;
  double const *xmean = this->rgxmean; 

  /* cmaes_SetMean(t, xmean); * xmean could be changed at this point */

  /* calculate eigensystem  */
  if (!this->flgEigensysIsUptodate) 
  {
    if (!flgdiag)
      cmaes_UpdateEigensystem(0);
    else
    {
        for (i = 0; i < N; ++i)
          this->rgD[i] = sqrt(this->C[i][i]);

        this->minEW = douSquare(rgdouMin(this->rgD, N)); 
        this->maxEW = douSquare(rgdouMax(this->rgD, N));
        this->flgEigensysIsUptodate = 1;
        this->eigenTimings.timings_start();
    }
  }

  /* treat minimal standard deviations and numeric problems */
  this->TestMinStdDevs(); 

  for (iNk = 0; iNk < this->lambda; ++iNk)
    { /* generate scaled random vector (D * z)    */
      for (i = 0; i < N; ++i)
        if (flgdiag)
          this->rgrgx[iNk][i] = xmean[i] + this->sigma * this->rgD[i] * this->rand.random_Gauss();
        else
          this->rgdTmp[i] = this->rgD[i] * this->rand.random_Gauss();

      if (!flgdiag)
        /* add mutation (sigma * B * (D*z)) */
        for (i = 0; i < N; ++i) {
          for (j = 0, sum = 0.; j < N; ++j)
            sum += this->B[i][j] * this->rgdTmp[j];
          this->rgrgx[iNk][i] = xmean[i] + this->sigma * sum;
        }
    }
  if(this->state == 3 || this->gen == 0)
    ++this->gen;
  this->state = 1; 

  return(this->rgrgx);
} /* SamplePopulation() */

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double const* CMAES::cmaes_ReSampleSingle_old( double *rgx )
{
  int i, j, N=this->N;
  double sum; 

  if (rgx == nullptr){};
    //FATAL("cmaes_ReSampleSingle(): Missing input double *x",0,0,0); TODO: exception

  for (i = 0; i < N; ++i)
    this->rgdTmp[i] = this->rgD[i] * this->rand.random_Gauss();

  /* add mutation (sigma * B * (D*z)) */
  for (i = 0; i < N; ++i)
  {
    for (j = 0, sum = 0.; j < N; ++j)
      sum += this->B[i][j] * this->rgdTmp[j];

    rgx[i] = this->rgxmean[i] + this->sigma * sum;
  }

  return rgx;
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * const * CMAES::cmaes_ReSampleSingle( int iindex )
{
  int i, j, N=this->N;
  double *rgx; 
  double sum; 
  static char s[99];

  if (iindex < 0 || iindex >= this->lambda) 
  {
    sprintf(s, "index==%d must be between 0 and %d", iindex, this->lambda);
    //FATAL("cmaes_ReSampleSingle(): Population member ",s,0,0); TODO: exception
  }
  rgx = this->rgrgx[iindex];

  for (i = 0; i < N; ++i)
    this->rgdTmp[i] = this->rgD[i] * this->rand.random_Gauss();

  /* add mutation (sigma * B * (D*z)) */
  for (i = 0; i < N; ++i) {
    for (j = 0, sum = 0.; j < N; ++j)
      sum += this->B[i][j] * this->rgdTmp[j];
    rgx[i] = this->rgxmean[i] + this->sigma * sum;
  }
  return(this->rgrgx);
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * CMAES::cmaes_SampleSingleInto( double *rgx )
{
  int i, j, N=this->N;
  double sum; 

  if (rgx == nullptr)
    rgx = new_double(N);

  for (i = 0; i < N; ++i)
    this->rgdTmp[i] = this->rgD[i] * this->rand.random_Gauss();
  /* add mutation (sigma * B * (D*z)) */
  for (i = 0; i < N; ++i) {
    for (j = 0, sum = 0.; j < N; ++j)
      sum += this->B[i][j] * this->rgdTmp[j];
    rgx[i] = this->rgxmean[i] + this->sigma * sum;
  }
  return rgx;
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * CMAES::cmaes_PerturbSolutionInto( double *rgx, double const *xmean, double eps )
{
  int i, j, N=this->N;
  double sum; 

  if (rgx == nullptr)
    rgx = new_double(N);
  if (xmean == nullptr){};
    //FATAL("cmaes_PerturbSolutionInto(): xmean was not given",0,0,0);

  for (i = 0; i < N; ++i)
    this->rgdTmp[i] = this->rgD[i] * this->rand.random_Gauss();
  /* add mutation (sigma * B * (D*z)) */

  for (i = 0; i < N; ++i)
  {
    for (j = 0, sum = 0.; j < N; ++j)
      sum += this->B[i][j] * this->rgdTmp[j];

    rgx[i] = xmean[i] + eps * this->sigma * sum;
  }

  return rgx;
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
double * CMAES::cmaes_UpdateDistribution(const double *rgFunVal)
{
	int i, j, iNk, hsig, N=this->N;
	int flgdiag = ((this->diagonalCov == 1) || (this->diagonalCov >= this->gen)); 
	double sum; 
	double psxps; 

	if(this->state == 3)
	{
		LOG_ERROR << "cmaes_UpdateDistribution(): You need to call SamplePopulation() before update can take place." << ends;
		exit(-1); // TODO add exception
	}

	if(rgFunVal == nullptr)
	{
		LOG_ERROR << "cmaes_UpdateDistribution(): Fitness function value array input is missing." << ends;
		exit(-1); // TODO add exception
	} 

	if(this->state == 1)  /* function values are delivered here */
		this->countevals += this->lambda;
	else
	{
		LOG_ERROR << "cmaes_UpdateDistribution(): unexpected state=" << this->state << ends;
		exit(-1);
	} // ERRORMESSAGE("cmaes_UpdateDistribution(): unexpected state",0,0,0);

	/* assign function values */
	for (i=0; i < this->lambda; ++i) 
		this->rgrgx[i][N] = this->rgFuncValue[i] = rgFunVal[i];

	/* Generate index */
	Sorted_index(rgFunVal, this->index, this->lambda);
	LOG_DEBUG << "Index sorted." << ends;

	/* Test if function values are identical, escape flat fitness */
	if (this->rgFuncValue[this->index[0]] == this->rgFuncValue[this->index[(int)this->lambda/2]] ) 
	{
		this->sigma *= exp(0.2 + this->cs / this->damps);
		
		LOG_WARNING << "Warning: sigma increased due to equal function values: Reconsider the formulation of the objective function" << ends;
	}

	LOG_DEBUG << "Updating function value history..." << ends;
	/* update function value history */
	for(i = (int)*(this->arFuncValueHist-1)-1; i > 0; --i) /* for(i = this->arFuncValueHist[-1]-1; i > 0; --i) */
		this->arFuncValueHist[i] = this->arFuncValueHist[i-1];
	this->arFuncValueHist[0] = rgFunVal[this->index[0]];

	LOG_DEBUG << "Updating xbestever..." << ends;
	/* update xbestever */
	if (this->rgxbestever[N] > this->rgrgx[this->index[0]][N] || this->gen == 1)
		for (i = 0; i <= N; ++i) 
		{
			this->rgxbestever[i] = this->rgrgx[this->index[0]][i];
			this->rgxbestever[N+1] = this->countevals;
		}

	LOG_DEBUG << "Computing xmean and rgxmean..." << ends;
	/* calculate xmean and rgBDz~N(0,C) */
	for (i = 0; i < N; ++i)
	{
		this->rgxold[i] = this->rgxmean[i]; 
		this->rgxmean[i] = 0.;
	
		for (iNk = 0; iNk < this->mu; ++iNk) 
			this->rgxmean[i] += this->weights[iNk] * this->rgrgx[this->index[iNk]][i];
		this->rgBDz[i] = sqrt(this->mueff)*(this->rgxmean[i] - this->rgxold[i])/this->sigma; 
	}

	LOG_DEBUG << "Computing z..." << ends;
	/* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
	for (i = 0; i < N; ++i)
	{
		if (!flgdiag)
			for (j = 0, sum = 0.; j < N; ++j)
				sum += this->B[j][i] * this->rgBDz[j];
			else
				sum = this->rgBDz[i];

		this->rgdTmp[i] = sum / this->rgD[i];
	}

	LOG_DEBUG << "Computing cumulation for sigma (ps)..." << ends;
	/* cumulation for sigma (ps) using B*z */
	for (i = 0; i < N; ++i) 
	{
		if (!flgdiag)
		for (j = 0, sum = 0.; j < N; ++j)
		sum += this->B[i][j] * this->rgdTmp[j];
		else
		sum = this->rgdTmp[i];
		this->rgps[i] = (1. - this->cs) * this->rgps[i] + 
		sqrt(this->cs * (2. - this->cs)) * sum;
	}

	LOG_DEBUG << "Computing norm(ps)^2..." << ends;
	/* calculate norm(ps)^2 */
	for (i = 0, psxps = 0.; i < N; ++i)
		psxps += this->rgps[i] * this->rgps[i];

/* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
hsig = sqrt(psxps) / sqrt(1. - pow(1.-this->cs, 2*this->gen)) / this->chiN
< 1.4 + 2./(N+1);
for (i = 0; i < N; ++i) {
this->rgpc[i] = (1. - this->ccumcov) * this->rgpc[i] + 
      hsig * sqrt(this->ccumcov * (2. - this->ccumcov)) * this->rgBDz[i];
  }
  
  /* stop initial phase */
  if (this->flgIniphase && 
      this->gen > douMin(1/this->cs, 1+N/this->mucov)) 
    {
      if (psxps / this->damps / (1.-pow((1. - this->cs), this->gen)) < N * 1.05) 
        this->flgIniphase = 0;
    }
  
#if 0
  /* remove momentum in ps, if ps is large and fitness is getting worse */
  /* This is obsolete due to hsig and harmful in a dynamic environment */
  if(psxps/N > 1.5 + 10.*sqrt(2./N) 
     && this->arFuncValueHist[0] > this->arFuncValueHist[1]
     && this->arFuncValueHist[0] > this->arFuncValueHist[2]) {
    double tfac = sqrt((1 + douMax(0, log(psxps/N))) * N / psxps);
    for (i=0; i<N; ++i) 
      this->rgps[i] *= tfac;
    psxps *= tfac*tfac; 
  }
#endif
  
  /* update of C  */

  this->Adapt_C2(hsig);
  
  /* Adapt_C(t); not used anymore */

#if 0
  if (this->sp.ccov != 0. && this->flgIniphase == 0) {
    int k; 

    this->flgEigensysIsUptodate = 0;

    /* update covariance matrix */
    for (i = 0; i < N; ++i)
      for (j = 0; j <=i; ++j) {
        this->C[i][j] = (1 - this->sp.ccov) * this->C[i][j] 
          + this->sp.ccov * (1./this->sp.mucov) 
            * (this->rgpc[i] * this->rgpc[j] 
               + (1-hsig)*this->sp.ccumcov*(2.-this->sp.ccumcov) * this->C[i][j]);
        for (k = 0; k < this->sp.mu; ++k) /* additional rank mu update */
          this->C[i][j] += this->sp.ccov * (1-1./this->sp.mucov) * this->sp.weights[k]  
            * (this->rgrgx[this->index[k]][i] - this->rgxold[i]) 
            * (this->rgrgx[this->index[k]][j] - this->rgxold[j])
            / this->sigma / this->sigma; 
      }
  }
#endif


  /* update of sigma */
  this->sigma *= exp(((sqrt(psxps)/this->chiN)-1.)*this->cs/this->damps);

  this->state = 3;

  return (this->rgxmean);

} /* cmaes_UpdateDistribution() */


/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void CMAES::Adapt_C2(int hsig)
{
  int i, j, k, N=this->N;
  int flgdiag = ((this->diagonalCov == 1) || (this->diagonalCov >= this->gen)); 

  if (this->ccov != 0. && this->flgIniphase == 0)
  {

    /* definitions for speeding up inner-most loop */
    double ccov1 = douMin(this->ccov * (1./this->mucov) * (flgdiag ? (N+1.5) / 3. : 1.), 1.);
    double ccovmu = douMin(this->ccov * (1-1./this->mucov)* (flgdiag ? (N+1.5) / 3. : 1.), 1. - ccov1); 
    double sigmasquare = this->sigma * this->sigma; 

    this->flgEigensysIsUptodate = 0;

    /* update covariance matrix */
    for (i = 0; i < N; ++i)
      for (j = flgdiag ? i : 0; j <= i; ++j) {
        this->C[i][j] = (1 - ccov1 - ccovmu) * this->C[i][j] 
          + ccov1
            * (this->rgpc[i] * this->rgpc[j] 
               + (1-hsig)*this->ccumcov*(2.-this->ccumcov) * this->C[i][j]);
        for (k = 0; k < this->mu; ++k) { /* additional rank mu update */
          this->C[i][j] += ccovmu * this->weights[k]  
            * (this->rgrgx[this->index[k]][i] - this->rgxold[i]) 
            * (this->rgrgx[this->index[k]][j] - this->rgxold[j])
            / sigmasquare;
        }
      }
    /* update maximal and minimal diagonal value */
    this->maxdiagC = this->mindiagC = this->C[0][0];
    for (i = 1; i < N; ++i) {
      if (this->maxdiagC < this->C[i][i])
        this->maxdiagC = this->C[i][i];
      else if (this->mindiagC > this->C[i][i])
        this->mindiagC = this->C[i][i];
    }
  } /* if ccov... */
}


/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void CMAES::TestMinStdDevs()
  /* increases sigma */
{
  int i, N = this->N; 
  if (this->rgDiffMinChange == nullptr)
    return;

  for (i = 0; i < N; ++i)
    while (this->sigma * sqrt(this->C[i][i]) < this->rgDiffMinChange[i]) 
      this->sigma *= exp(0.05+this->cs/this->damps);

} /* cmaes_TestMinStdDevs() */


/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void CMAES::cmaes_WriteToFile(const char *key, const char *name)
{ 
  this->cmaes_WriteToFileAW(key, name, "a"); /* default is append */
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void CMAES::cmaes_WriteToFileAW(const char *key, const char *name, 
                         char *appendwrite)
{ 
  char *s = "tmpcmaes.dat"; 
  FILE *fp;
  
  if (name == nullptr)
    name = s; 

  fp = fopen( name, appendwrite); 

  if(fp == nullptr)
  {
    ERRORMESSAGE("cmaes_WriteToFile(): could not open '", name, 
                 "' with flag ", appendwrite);
    return;
  }

  if (appendwrite[0] == 'w')
  {
    /* write a header line, very rudimentary */
    fprintf(fp, "%% # %s (randomSeed=%d, %s)\n", key, this->seed, getTimeStr());
  }
  else if (this->gen > 0 || strncmp(name, "outcmaesfit", 11) != 0)
      this->cmaes_WriteToFilePtr(key, fp); /* do not write fitness for gen==0 */

  fclose(fp);

} /* WriteToFile */

/* --------------------------------------------------------- */
void CMAES::cmaes_WriteToFilePtr(const char *key, FILE *fp)

/* this hack reads key words from input key for data to be written to
 * a file, see file signals.par as input file. The length of the keys
 * is mostly fixed, see key += number in the code! If the key phrase
 * does not match the expectation the output might be strange.  for
 * cmaes_t *t == nullptr it solely prints key as a header line. Input key
 * must be zero terminated.
 */
{ 
  int i, k, N = this->N; 
  char const *keyend, *keystart; 
  char *s = "few";
  if (key == nullptr)
    key = s; 
  keystart = key; /* for debugging purpose */ 
  keyend = key + strlen(key);

  while (key < keyend)
    {
      if (strncmp(key, "axisratio", 9) == 0)
        {
          fprintf(fp, "%.2e", sqrt(this->maxEW/this->minEW));
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "idxminSD", 8) == 0)
        {
          int mini=0; for(i=N-1;i>0;--i) if(this->mindiagC==this->C[i][i]) mini=i; 
          fprintf(fp, "%d", mini+1);
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "idxmaxSD", 8) == 0)
        {
          int maxi=0; for(i=N-1;i>0;--i) if(this->maxdiagC==this->C[i][i]) maxi=i; 
          fprintf(fp, "%d", maxi+1);
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      /* new coordinate system == all eigenvectors */
      if (strncmp(key, "B", 1) == 0) 
        {
          /* int j, index[N]; */
          int j, *iindex=(int*)(new_void(N,sizeof(int))); /* MT */
          Sorted_index(this->rgD, iindex, N); /* should not be necessary, see end of QLalgo2 */
          /* One eigenvector per row, sorted: largest eigenvalue first */
          for (i = 0; i < N; ++i)
            for (j = 0; j < N; ++j)
              fprintf(fp, "%g%c", this->B[j][iindex[N-1-i]], (j==N-1)?'\n':'\t');
          ++key; 
          free(iindex); /* MT */
        }
      /* covariance matrix */
      if (strncmp(key, "C", 1) == 0) 
        {
          int j;
          for (i = 0; i < N; ++i)
            for (j = 0; j <= i; ++j)
              fprintf(fp, "%g%c", this->C[i][j], (j==i)?'\n':'\t');
          ++key; 
        }
      /* (processor) time (used) since begin of execution */ 
      if (strncmp(key, "clock", 4) == 0)
        {
          this->eigenTimings.timings_update();
          fprintf(fp, "%.1f %.1f",  this->eigenTimings.totaltotaltime, 
                  this->eigenTimings.tictoctime);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      /* ratio between largest and smallest standard deviation */
      if (strncmp(key, "stddevratio", 11) == 0) /* std dev in coordinate axes */
        {
          fprintf(fp, "%g", sqrt(this->maxdiagC/this->mindiagC));
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      /* standard deviations in coordinate directions (sigma*sqrt(C[i,i])) */
      if (strncmp(key, "coorstddev", 10) == 0 
          || strncmp(key, "stddev", 6) == 0) /* std dev in coordinate axes */
        {
          for (i = 0; i < N; ++i)
            fprintf(fp, "%s%g", (i==0) ? "":"\t", this->sigma*sqrt(this->C[i][i]));
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      /* diagonal of D == roots of eigenvalues, sorted */
      if (strncmp(key, "diag(D)", 7) == 0)
        {
          for (i = 0; i < N; ++i)
            this->rgdTmp[i] = this->rgD[i];
          qsort(this->rgdTmp, (unsigned) N, sizeof(double), &SignOfDiff); /* superfluous */
          for (i = 0; i < N; ++i)
            fprintf(fp, "%s%g", (i==0) ? "":"\t", this->rgdTmp[i]);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "dim", 3) == 0)
        {
          fprintf(fp, "%d", N);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "eval", 4) == 0)
        {
          fprintf(fp, "%.0f", this->countevals);
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "few(diag(D))", 12) == 0)/* between four and six axes */
        { 
          int add = (int)(0.5 + (N + 1.) / 5.); 
          for (i = 0; i < N; ++i)
            this->rgdTmp[i] = this->rgD[i];
          qsort(this->rgdTmp, (unsigned) N, sizeof(double), &SignOfDiff);
          for (i = 0; i < N-1; i+=add)        /* print always largest */
            fprintf(fp, "%s%g", (i==0) ? "":"\t", this->rgdTmp[N-1-i]);
          fprintf(fp, "\t%g\n", this->rgdTmp[0]);        /* and smallest */
          break; /* number of printed values is not determined */
        }
      if (strncmp(key, "fewinfo", 7) == 0) { 
        fprintf(fp," Iter Fevals  Function Value          Sigma   ");
        fprintf(fp, "MaxCoorDev MinCoorDev AxisRatio   MinDii      Time in eig\n");
        while (*key != '+' && *key != '\0' && key < keyend)
          ++key;
      }
      if (strncmp(key, "few", 3) == 0) { 
        fprintf(fp, " %4.0f ", this->gen); 
        fprintf(fp, " %5.0f ", this->countevals); 
        fprintf(fp, "%.15e", this->rgFuncValue[this->index[0]]);
        fprintf(fp, "  %.2e  %.2e %.2e", this->sigma, this->sigma*sqrt(this->maxdiagC), 
                this->sigma*sqrt(this->mindiagC));
        fprintf(fp, "  %.2e  %.2e", sqrt(this->maxEW/this->minEW), sqrt(this->minEW));
        while (*key != '+' && *key != '\0' && key < keyend)
          ++key;
        fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
      }
      if (strncmp(key, "funval", 6) == 0 || strncmp(key, "fitness", 6) == 0)
        {
          fprintf(fp, "%.15e", this->rgFuncValue[this->index[0]]);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "fbestever", 9) == 0)
        {
          fprintf(fp, "%.15e", this->rgxbestever[N]); /* f-value */
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "fmedian", 7) == 0)
        {
          fprintf(fp, "%.15e", this->rgFuncValue[this->index[(int)(this->lambda/2)]]);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "fworst", 6) == 0)
        {
          fprintf(fp, "%.15e", this->rgFuncValue[this->index[this->lambda-1]]);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "arfunval", 8) == 0 || strncmp(key, "arfitness", 8) == 0)
        {
          for (i = 0; i < N; ++i)
            fprintf(fp, "%s%.10e", (i==0) ? "" : "\t", 
                    this->rgFuncValue[this->index[i]]);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "gen", 3) == 0)
        {
          fprintf(fp, "%.0f", this->gen);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "iter", 4) == 0)
        {
          fprintf(fp, "%.0f", this->gen);
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "sigma", 5) == 0)
        {
          fprintf(fp, "%.4e", this->sigma);
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "minSD", 5) == 0) /* minimal standard deviation */
        {
          fprintf(fp, "%.4e", sqrt(this->mindiagC));
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "maxSD", 5) == 0)
        {
          fprintf(fp, "%.4e", sqrt(this->maxdiagC));
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "mindii", 6) == 0)
        {
          fprintf(fp, "%.4e", sqrt(this->minEW));
          while (*key != '+' && *key != '\0' && key < keyend)
           ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "0", 1) == 0)
        {
          fprintf(fp, "0");
          ++key; 
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "lambda", 6) == 0)
        {
          fprintf(fp, "%d", this->lambda);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "N", 1) == 0)
        {
          fprintf(fp, "%d", N);
          ++key; 
          fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
        }
      if (strncmp(key, "resume", 6) == 0)
        {
          fprintf(fp, "\n# resume %d\n", N);
          fprintf(fp, "xmean\n");
          this->cmaes_WriteToFilePtr("xmean", fp);
          fprintf(fp, "path for sigma\n");
          for(i=0; i<N; ++i)
            fprintf(fp, "%g%s", this->rgps[i], (i==N-1) ? "\n":"\t");
          fprintf(fp, "path for C\n");
          for(i=0; i<N; ++i)
            fprintf(fp, "%g%s", this->rgpc[i], (i==N-1) ? "\n":"\t");
          fprintf(fp, "sigma %g\n", this->sigma);
          /* note than B and D might not be up-to-date */
          fprintf(fp, "covariance matrix\n"); 
          cmaes_WriteToFilePtr("C", fp);
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
        }
      if (strncmp(key, "xbest", 5) == 0) { /* best x in recent generation */
        for(i=0; i<N; ++i)
          fprintf(fp, "%s%g", (i==0) ? "":"\t", this->rgrgx[this->index[0]][i]);
        while (*key != '+' && *key != '\0' && key < keyend)
          ++key;
        fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
      }
      if (strncmp(key, "xmean", 5) == 0) {
        for(i=0; i<N; ++i)
          fprintf(fp, "%s%g", (i==0) ? "":"\t", this->rgxmean[i]);
        while (*key != '+' && *key != '\0' && key < keyend)
          ++key;
        fprintf(fp, "%c", (*key=='+') ? '\t':'\n');
      }
      if (strncmp(key, "all", 3) == 0)
        {
          time_t ti = time(nullptr);
          fprintf(fp, "\n# --------- %s\n", asctime(localtime(&ti)));
          fprintf(fp, " N %d\n", N);
          fprintf(fp, " seed %d\n", this->seed);
          fprintf(fp, "function evaluations %.0f\n", this->countevals);
          fprintf(fp, "elapsed (CPU) time [s] %.2f\n", this->eigenTimings.totaltotaltime);
          fprintf(fp, "function value f(x)=%g\n", this->rgrgx[this->index[0]][N]);
          fprintf(fp, "maximal standard deviation %g\n", this->sigma * sqrt(this->maxdiagC));
          fprintf(fp, "minimal standard deviation %g\n", this->sigma * sqrt(this->mindiagC));
          fprintf(fp, "sigma %g\n", this->sigma);
          fprintf(fp, "axisratio %g\n", rgdouMax(this->rgD, N)/rgdouMin(this->rgD, N));
          fprintf(fp, "xbestever found after %.0f evaluations, function value %g\n", 
                  this->rgxbestever[N+1], this->rgxbestever[N]);
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->rgxbestever[i], 
                    (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "xbest (of last generation, function value %g)\n", 
                  this->rgrgx[this->index[0]][N]); 
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->rgrgx[this->index[0]][i], 
                    (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "xmean \n");
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->rgxmean[i], 
                    (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "Standard deviation of coordinate axes (sigma*sqrt(diag(C)))\n");
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->sigma*sqrt(this->C[i][i]), 
                    (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "Main axis lengths of mutation ellipsoid (sigma*diag(D))\n");
          for (i = 0; i < N; ++i)
            this->rgdTmp[i] = this->rgD[i];
          qsort(this->rgdTmp, (unsigned) N, sizeof(double), &SignOfDiff);
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->sigma*this->rgdTmp[N-1-i], 
                    (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "Longest axis (b_i where d_ii=max(diag(D))\n");
          k = MaxIdx(this->rgD, N);
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->B[i][k], (i%5==4||i==N-1)?'\n':' ');
          fprintf(fp, "Shortest axis (b_i where d_ii=max(diag(D))\n");
          k = MinIdx(this->rgD, N);
          for(i=0; i<N; ++i)
            fprintf(fp, " %12g%c", this->B[i][k], (i%5==4||i==N-1)?'\n':' ');
          while (*key != '+' && *key != '\0' && key < keyend)
            ++key;
        } /* "all" */

#if 0 /* could become generic part */
      s0 = key;
      d = cmaes_Get(t, key); /* TODO find way to detect whether key was found */
      if (key == s0) /* this does not work, is always true */
        {
          /* write out stuff, problem: only generic format is available */
          /* move in key until "+" or end */
        }
#endif 

      if (*key == '\0') 
        break; 
      else if (*key != '+') { /* last key was not recognized */
        ERRORMESSAGE("cmaes_t:WriteToFilePtr(): unrecognized key '", key, "'", 0); 
        while (*key != '+' && *key != '\0' && key < keyend)
          ++key;
      }
      while (*key == '+') 
        ++key; 
    } /* while key < keyend */ 

  if (key > keyend);
    //FATAL("cmaes_t:WriteToFilePtr(): BUG regarding key sequence",0,0,0);

} /* WriteToFilePtr */

/* --------------------------------------------------------- */
double  CMAES::cmaes_Get(char const *s)
{
  int N=this->N;

  if (strncmp(s, "axisratio", 5) == 0) { /* between lengths of longest and shortest principal axis of the distribution ellipsoid */
    return (rgdouMax(this->rgD, N)/rgdouMin(this->rgD, N));
  }
  else if (strncmp(s, "eval", 4) == 0) { /* number of function evaluations */
    return (this->countevals);
  }
  else if (strncmp(s, "fctvalue", 6) == 0
           || strncmp(s, "funcvalue", 6) == 0
           || strncmp(s, "funvalue", 6) == 0
           || strncmp(s, "fitness", 3) == 0) { /* recent best function value */
    return(this->rgFuncValue[this->index[0]]);
  }
  else if (strncmp(s, "fbestever", 7) == 0) { /* ever best function value */
    return(this->rgxbestever[N]);
  }
  else if (strncmp(s, "generation", 3) == 0
           || strncmp(s, "iteration", 4) == 0) { 
    return(this->gen);
  }
  else if (strncmp(s, "maxeval", 4) == 0
           || strncmp(s, "MaxFunEvals", 8) == 0
           || strncmp(s, "stopMaxFunEvals", 12) == 0) { /* maximal number of function evaluations */
    return(this->stopMaxFunEvals);
  }
  else if (strncmp(s, "maxgen", 4) == 0
           || strncmp(s, "MaxIter", 7) == 0
           || strncmp(s, "stopMaxIter", 11) == 0) { /* maximal number of generations */
    return(ceil(this->stopMaxIter));
  }
  else if (strncmp(s, "maxaxislength", 5) == 0) { /* sigma * max(diag(D)) */
    return(this->sigma * sqrt(this->maxEW));
  }
  else if (strncmp(s, "minaxislength", 5) == 0) { /* sigma * min(diag(D)) */
    return(this->sigma * sqrt(this->minEW));
  }
  else if (strncmp(s, "maxstddev", 4) == 0) { /* sigma * sqrt(max(diag(C))) */
    return(this->sigma * sqrt(this->maxdiagC));
  }
  else if (strncmp(s, "minstddev", 4) == 0) { /* sigma * sqrt(min(diag(C))) */
    return(this->sigma * sqrt(this->mindiagC));
  }
  else if (strncmp(s, "N", 1) == 0 || strcmp(s, "n") == 0 || 
           strncmp(s, "dimension", 3) == 0) {
    return (N);
  }
  else if (strncmp(s, "lambda", 3) == 0
           || strncmp(s, "samplesize", 8) == 0
           || strncmp(s, "popsize", 7) == 0) { /* sample size, offspring population size */
    return(this->lambda);
  }
  else if (strncmp(s, "sigma", 3) == 0) {
    return(this->sigma);
  }
  // FATAL( "cmaes_Get(cmaes_t, char const * s): No match found for s='", s, "'",0); // TODO: exception
  return(0);
} /* cmaes_Get() */

/* --------------------------------------------------------- */
double * CMAES::cmaes_GetInto( char const *s, double *res )
{
  int i, N = this->N;
  double const * res0 = this->cmaes_GetPtr(s);
  if (res == nullptr)
	  res = new_double(N);
  for (i = 0; i < N; ++i)
	  res[i] = res0[i];
  return res; 
}

/* --------------------------------------------------------- */
double * CMAES::cmaes_GetNew( char const *s )
{
	return (this->cmaes_GetInto(s, nullptr));
}

/* --------------------------------------------------------- */
const double * CMAES::cmaes_GetPtr( char const *s )
{
	int i, N=this->N;

	/* diagonal of covariance matrix */
	if (strncmp(s, "diag(C)", 7) == 0) { 
		for (i = 0; i < N; ++i)
			this->rgout[i] = this->C[i][i]; 
		return(this->rgout);
	}
	/* diagonal of axis lengths matrix */
	else if (strncmp(s, "diag(D)", 7) == 0) { 
		return(this->rgD);
	}
	/* vector of standard deviations sigma*sqrt(diag(C)) */
	else if (strncmp(s, "stddev", 3) == 0) { 
		for (i = 0; i < N; ++i)
			this->rgout[i] = this->sigma * sqrt(this->C[i][i]); 
		return(this->rgout);
	}
	/* bestever solution seen so far */
	else if (strncmp(s, "xbestever", 7) == 0)
		return(this->rgxbestever);
	/* recent best solution of the recent population */
	else if (strncmp(s, "xbest", 5) == 0)
		return(this->rgrgx[this->index[0]]);
	/* mean of the recent distribution */
	else if (strncmp(s, "xmean", 1) == 0)
		return(this->rgxmean);

	return(nullptr);
}

/* --------------------------------------------------------- */
/* tests stopping criteria 
 *   returns a string of satisfied stopping criterion for each line
 *   otherwise nullptr 
 */
const char* CMAES::cmaes_TestForTermination()
{
	double range, fac;
	int iAchse, iKoo;
	int flgdiag = ((this->diagonalCov == 1) || (this->diagonalCov >= this->gen)); 
	static char sTestOutString[3024];
	char * cp = sTestOutString;
	int i, cTemp, N=this->N; 
	cp[0] = '\0';

	/* function value reached */
	if ((this->gen > 1 || this->state > 1) && this->stStopFitness.flg && 
			this->rgFuncValue[this->index[0]] <= this->stStopFitness.val) 
		cp += sprintf(cp, "Fitness: function value %7.2e <= stopFitness (%7.2e)\n", 
				this->rgFuncValue[this->index[0]], this->stStopFitness.val);

	/* TolFun */
	range = douMax(rgdouMax(this->arFuncValueHist, (int)douMin(this->gen,*(this->arFuncValueHist-1))), 
			rgdouMax(this->rgFuncValue, this->lambda)) -
		douMin(rgdouMin(this->arFuncValueHist, (int)douMin(this->gen, *(this->arFuncValueHist-1))), 
				rgdouMin(this->rgFuncValue, this->lambda));

	if (this->gen > 0 && range <= this->stopTolFun) {
		cp += sprintf(cp, 
				"TolFun: function value differences %7.2e < stopTolFun=%7.2e\n", 
				range, this->stopTolFun);
	}

	/* TolFunHist */
	if (this->gen > *(this->arFuncValueHist-1)) {
		range = rgdouMax(this->arFuncValueHist, (int)*(this->arFuncValueHist-1)) 
			- rgdouMin(this->arFuncValueHist, (int)*(this->arFuncValueHist-1));
		if (range <= this->stopTolFunHist)
			cp += sprintf(cp, 
					"TolFunHist: history of function value changes %7.2e stopTolFunHist=%7.2e", 
					range, this->stopTolFunHist);
	}

	/* TolX */
	for(i=0, cTemp=0; i<N; ++i) {
		cTemp += (this->sigma * sqrt(this->C[i][i]) < this->stopTolX) ? 1 : 0;
		cTemp += (this->sigma * this->rgpc[i] < this->stopTolX) ? 1 : 0;
	}
	if (cTemp == 2*N) {
		cp += sprintf(cp, 
				"TolX: object variable changes below %7.2e \n", 
				this->stopTolX);
	}

	/* TolUpX */
	for(i=0; i<N; ++i) {
		if (this->sigma * sqrt(this->C[i][i]) > this->stopTolUpXFactor * this->rgInitialStds[i])
			break;
	}
	if (i < N) {
		cp += sprintf(cp, 
				"TolUpX: standard deviation increased by more than %7.2e, larger initial standard deviation recommended \n", 
				this->stopTolUpXFactor);
	}

	/* Condition of C greater than dMaxSignifKond */
	if (this->maxEW >= this->minEW * this->dMaxSignifKond) {
		cp += sprintf(cp, 
				"ConditionNumber: maximal condition number %7.2e reached. maxEW=%7.2e,minEW=%7.2e,maxdiagC=%7.2e,mindiagC=%7.2e\n", 
				this->dMaxSignifKond, this->maxEW, this->minEW, this->maxdiagC, this->mindiagC);
	} /* if */

	/* Principal axis i has no effect on xmean, ie. 
	   x == x + 0.1 * sigma * rgD[i] * B[i] */
	if (!flgdiag) {
		for (iAchse = 0; iAchse < N; ++iAchse)
		{
			fac = 0.1 * this->sigma * this->rgD[iAchse];
			for (iKoo = 0; iKoo < N; ++iKoo){ 
				if (this->rgxmean[iKoo] != this->rgxmean[iKoo] + fac * this->B[iKoo][iAchse])
					break;
			}
			if (iKoo == N)        
			{
				/* this->sigma *= exp(0.2+this->sp.cs/this->sp.damps); */
				cp += sprintf(cp, 
						"NoEffectAxis: standard deviation 0.1*%7.2e in principal axis %d without effect\n", 
						fac/0.1, iAchse);
				break;
			} /* if (iKoo == N) */
		} /* for iAchse             */
	} /* if flgdiag */
	/* Component of xmean is not changed anymore */
	for (iKoo = 0; iKoo < N; ++iKoo)
	{
		if (this->rgxmean[iKoo] == this->rgxmean[iKoo] + 
				0.2 * this->sigma*sqrt(this->C[iKoo][iKoo]))
		{
			/* this->C[iKoo][iKoo] *= (1 + this->sp.ccov); */
			/* flg = 1; */
			cp += sprintf(cp, 
					"NoEffectCoordinate: standard deviation 0.2*%7.2e in coordinate %d without effect\n", 
					this->sigma * sqrt(this->C[iKoo][iKoo]), iKoo); 
			break;
		}

	} /* for iKoo */
	/* if (flg) this->sigma *= exp(0.05+this->sp.cs/this->sp.damps); */

	if(this->countevals >= this->stopMaxFunEvals) 
		cp += sprintf(cp, "MaxFunEvals: conducted function evaluations %.0f >= %g\n", 
				this->countevals, this->stopMaxFunEvals);
	if(this->gen >= this->stopMaxIter) 
		cp += sprintf(cp, "MaxIter: number of iterations %.0f >= %g\n", 
				this->gen, this->stopMaxIter); 
	if(this->flgStop)
		cp += sprintf(cp, "Manual: stop signal read\n");

#if 0
	else if (0) {
		for(i=0, cTemp=0; i<N; ++i) {
			cTemp += (sigma * sqrt(C[i][i]) < stopdx) ? 1 : 0;
			cTemp += (sigma * rgpc[i] < stopdx) ? 1 : 0;
		}
		if (cTemp == 2*N)
			flgStop = 1;
	}
#endif

	if (cp - sTestOutString > 320)
		ERRORMESSAGE("Bug in cmaes_t:Test(): sTestOutString too short",0,0,0);

	if (cp != sTestOutString) {
		return sTestOutString;
	}

	return(nullptr);

} /* cmaes_Test() */

/* --------------------------------------------------------- */
void CMAES::cmaes_ReadSignals(char const *filename)
{
	const char *s = "signals.par"; 
	FILE *fp;
	if (filename == nullptr)
		filename = s;
	fp = fopen( filename, "r"); 
	if(fp == nullptr) {
		return;
	}
	this->cmaes_ReadFromFilePtr( fp );
	fclose(fp);
}
/* --------------------------------------------------------- */
void CMAES::cmaes_ReadFromFilePtr( FILE *fp )
	/* reading commands e.g. from signals.par file 
	 */
{
	char *keys[15];
	char s[199], sin1[99], sin2[129], sin3[99], sin4[99];
	int ikey, ckeys, nb; 
	double d; 
	static int flglockprint = 0;
	static int flglockwrite = 0;
	static long countiterlastwritten; 
	static long maxdiffitertowrite; /* to prevent long gaps at the beginning */
	int flgprinted = 0;
	int flgwritten = 0; 
	double deltaprinttime = time(nullptr)-this->printtime; /* using clock instead might not be a good */
	double deltawritetime = time(nullptr)-this->writetime; /* idea as disc time is not CPU time? */
	double deltaprinttimefirst = this->firstprinttime ? time(nullptr)-this->firstprinttime : 0; /* time is in seconds!? */
	double deltawritetimefirst = this->firstwritetime ? time(nullptr)-this->firstwritetime : 0; 
	if (countiterlastwritten > this->gen) { /* probably restarted */
		maxdiffitertowrite = 0;
		countiterlastwritten = 0;
	}

	keys[0] = " stop%98s %98s";        /* s=="now" or eg "MaxIter+" %lg"-number */
	/* works with and without space */
	keys[1] = " print %98s %98s";       /* s==keyword for WriteFile */
	keys[2] = " write %98s %128s %98s"; /* s1==keyword, s2==filename */
	keys[3] = " check%98s %98s";
	keys[4] = " maxTimeFractionForEigendecompostion %98s";
	ckeys = 5; 
	strcpy(sin2, "tmpcmaes.dat");

	if (cmaes_TestForTermination()) 
	{
		deltaprinttime = time(nullptr); /* forces printing */
		deltawritetime = time(nullptr);
	}
	while(fgets(s, sizeof(s), fp) != nullptr)
	{ 
		if (s[0] == '#' || s[0] == '%') /* skip comments  */
			continue; 
		sin1[0] = sin2[0] = sin3[0] = sin4[0] = '\0';
		for (ikey=0; ikey < ckeys; ++ikey)
		{
			if((nb=sscanf(s, keys[ikey], sin1, sin2, sin3, sin4)) >= 1) 
			{
				switch(ikey) {
					case 0 : /* "stop", reads "stop now" or eg. stopMaxIter */
						if (strncmp(sin1, "now", 3) == 0) 
							this->flgStop = 1; 
						else if (strncmp(sin1, "MaxFunEvals", 11) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopMaxFunEvals = d; 
						}
						else if (strncmp(sin1, "MaxIter", 4) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopMaxIter = d; 
						}
						else if (strncmp(sin1, "Fitness", 7) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
							{
								this->stStopFitness.flg = 1; 
								this->stStopFitness.val = d; 
							}
						}
						else if (strncmp(sin1, "TolFunHist", 10) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopTolFunHist = d; 
						}
						else if (strncmp(sin1, "TolFun", 6) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopTolFun = d; 
						}
						else if (strncmp(sin1, "TolX", 4) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopTolX = d; 
						}
						else if (strncmp(sin1, "TolUpXFactor", 4) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) 
								this->stopTolUpXFactor = d; 
						}
						break;
					case 1 : /* "print" */
						d = 1; /* default */
						if (sscanf(sin2, "%lg", &d) < 1 && deltaprinttimefirst < 1)
							d = 0; /* default at first time */
						if (deltaprinttime >= d && !flglockprint) {
							cmaes_WriteToFilePtr(sin1, stdout);
							flgprinted = 1;
						}
						if(d < 0) 
							flglockprint += 2;
						break; 
					case 2 : /* "write" */
						/* write header, before first generation */
						if (this->countevals < this->lambda && this->flgresumedone == 0) 
							cmaes_WriteToFileAW(sin1, sin2, "w"); /* overwrite */
						d = 0.9; /* default is one with smooth increment of gaps */
						if (sscanf(sin3, "%lg", &d) < 1 && deltawritetimefirst < 2)
							d = 0; /* default is zero for the first second */
						if(d < 0) 
							flglockwrite += 2;
						if (!flglockwrite) {
							if (deltawritetime >= d) {
								this->cmaes_WriteToFile(sin1, sin2);
								flgwritten = 1; 
							} else if (d < 1 
									&& this->gen-countiterlastwritten > maxdiffitertowrite) {
								this->cmaes_WriteToFile(sin1, sin2);
								flgwritten = 1; 
							}
						}
						break; 
					case 3 : /* check, checkeigen 1 or check eigen 1 */
						if (strncmp(sin1, "eigen", 5) == 0) {
							if (sscanf(sin2, " %lg", &d) == 1) {
								if (d > 0)
									this->flgCheckEigen = 1;
								else
									this->flgCheckEigen = 0;
							}
							else
								this->flgCheckEigen = 0;
						}
						break;
					case 4 : /* maxTimeFractionForEigendecompostion */
						if (sscanf(sin1, " %lg", &d) == 1) 
							this->updateCmode.maxtime = d;
						break; 
					default :
						break; 
				}
				break; /* for ikey */
			} /* if line contains keyword */
		} /* for each keyword */
	} /* while not EOF of signals.par */
	if (this->writetime == 0) 
		this->firstwritetime = time(nullptr); 
	if (this->printtime == 0)
		this->firstprinttime = time(nullptr); 

	if (flgprinted)
		this->printtime = time(nullptr);
	if (flgwritten) {
		this->writetime = time(nullptr);
		if (this->gen-countiterlastwritten > maxdiffitertowrite)
			++maxdiffitertowrite; /* smooth prolongation of writing gaps/intervals */
		countiterlastwritten = (long int) this->gen;
	}
	--flglockprint;
	--flglockwrite;
	flglockprint = (flglockprint > 0) ? 1 : 0;
	flglockwrite = (flglockwrite > 0) ? 1 : 0;
} /*  cmaes_ReadFromFilePtr */ 

/* ========================================================= */
int CMAES::Check_Eigen( int N,  double **C, double *diag, double **Q) 
	/* 
	   exhaustive test of the output of the eigendecomposition
	   needs O(n^3) operations 

	   writes to error file 
	   returns number of detected inaccuracies 
	 */
{
	/* compute Q diag Q^T and Q Q^T to check */
	int i, j, k, res = 0;
	double cc, dd; 
	static char s[324];

	for (i=0; i < N; ++i)
		for (j=0; j < N; ++j) {
			for (cc=0.,dd=0., k=0; k < N; ++k) {
				cc += diag[k] * Q[i][k] * Q[j][k];
				dd += Q[i][k] * Q[j][k];
			}
			/* check here, is the normalization the right one? */
			if (fabs(cc - C[i>j?i:j][i>j?j:i])/sqrt(C[i][i]*C[j][j]) > 1e-10 
					&& fabs(cc - C[i>j?i:j][i>j?j:i]) > 3e-14) {
				sprintf(s, "%d %d: %.17e %.17e, %e", 
						i, j, cc, C[i>j?i:j][i>j?j:i], cc-C[i>j?i:j][i>j?j:i]);
				ERRORMESSAGE("cmaes_t:Eigen(): imprecise result detected ", 
						s, 0, 0);
				++res; 
			}
			if (fabs(dd - (i==j)) > 1e-10) {
				sprintf(s, "%d %d %.17e ", i, j, dd);
				ERRORMESSAGE("cmaes_t:Eigen(): imprecise result detected (Q not orthog.)", 
						s, 0, 0);
				++res;
			}
		}
	return res; 
}

/* --------------------------------------------------------- */
/* --------------------------------------------------------- */
void CMAES::cmaes_UpdateEigensystem( int flgforce )
{
	int i, N = this->N;

	this->eigenTimings.timings_update();

	if(flgforce == 0) {
		if (this->flgEigensysIsUptodate == 1)
			return; 

		/* return on modulo generation number */ 
		if (this->updateCmode.flgalways == 0 /* not implemented, always ==0 */
				&& this->gen < this->genOfEigensysUpdate + this->updateCmode.modulo
		   )
			return;

		/* return on time percentage */
		if (this->updateCmode.maxtime < 1.00 
				&& this->eigenTimings.tictoctime > this->updateCmode.maxtime * this->eigenTimings.totaltime
				&& this->eigenTimings.tictoctime > 0.0002)
			return; 
	}
	this->eigenTimings.timings_tic();

	this->Eigen( N, this->C, this->rgD, this->B, this->rgdTmp);

	this->eigenTimings.timings_toc();

	/* find largest and smallest eigenvalue, they are supposed to be sorted anyway */
	this->minEW = rgdouMin(this->rgD, N);
	this->maxEW = rgdouMax(this->rgD, N);

	if (this->flgCheckEigen)
		/* needs O(n^3)! writes, in case, error message in error file */ 
		i = this->Check_Eigen( N, this->C, this->rgD, this->B);

#if 0 
	/* Limit Condition of C to dMaxSignifKond+1 */
	if (this->maxEW > this->minEW * this->dMaxSignifKond) {
		ERRORMESSAGE("Warning: Condition number of covariance matrix at upper limit.",
				" Consider a rescaling or redesign of the objective function. " ,"","");
		printf("\nWarning: Condition number of covariance matrix at upper limit\n");
		tmp = this->maxEW/this->dMaxSignifKond - this->minEW;
		tmp = this->maxEW/this->dMaxSignifKond;
		this->minEW += tmp;
		for (i=0;i<N;++i) {
			this->C[i][i] += tmp;
			this->rgD[i] += tmp;
		}
	} /* if */
	this->dLastMinEWgroesserNull = minEW;
#endif   

	for (i = 0; i < N; ++i)
		this->rgD[i] = sqrt(this->rgD[i]);

	this->flgEigensysIsUptodate = 1;
	this->genOfEigensysUpdate = this->gen; 

	return;

} /* cmaes_UpdateEigensystem() */


/* ========================================================= */
void CMAES::Eigen( int N,  double **C, double *diag, double **Q, double *rgtmp)
	/* 
	   Calculating eigenvalues and vectors. 
	   Input: 
	   N: dimension.
	   C: symmetric (1:N)xN-matrix, solely used to copy data to Q
	   niter: number of maximal iterations for QL-Algorithm. 
	   rgtmp: N+1-dimensional vector for temporal use. 
	   Output: 
	   diag: N eigenvalues. 
	   Q: Columns are normalized eigenvectors.
	 */
{
	int i, j;

	if (rgtmp == nullptr){}; /* was OK in former versions */
	//FATAL("cmaes_t:Eigen(): input parameter double *rgtmp must be non-nullptr", 0,0,0); // TODO: exception

	/* copy C to Q */
	if (C != Q) {
		for (i=0; i < N; ++i)
			for (j = 0; j <= i; ++j)
				Q[i][j] = Q[j][i] = C[i][j];
	}

#if 0
	Householder( N, Q, diag, rgtmp);
	QLalgo( N, diag, Q, 30*N, rgtmp+1);
#else
	this->Householder2( N, Q, diag, rgtmp);
	this->QLalgo2( N, diag, rgtmp, Q);
#endif

}  


/* ========================================================= */
void CMAES::QLalgo2 (int n, double *d, double *e, double **V) {
	/*
	   -> n     : Dimension. 
	   -> d     : Diagonale of tridiagonal matrix. 
	   -> e[1..n-1] : off-diagonal, output from Householder
	   -> V     : matrix output von Householder
	   <- d     : eigenvalues
	   <- e     : garbage?
	   <- V     : basis of eigenvectors, according to d

	   Symmetric tridiagonal QL algorithm, iterative 
	   Computes the eigensystem from a tridiagonal matrix in roughtly 3N^3 operations

	   code adapted from Java JAMA package, function tql2. 
	 */

	int i, k, l, m;
	double f = 0.0;
	double tst1 = 0.0;
	double eps = 2.22e-16; /* Math.pow(2.0,-52.0);  == 2.22e-16 */

	/* shift input e */
	for (i = 1; i < n; i++) {
		e[i-1] = e[i];
	}
	e[n-1] = 0.0; /* never changed again */

	for (l = 0; l < n; l++) { 

		/* Find small subdiagonal element */

		if (tst1 < fabs(d[l]) + fabs(e[l]))
			tst1 = fabs(d[l]) + fabs(e[l]);
		m = l;
		while (m < n) {
			if (fabs(e[m]) <= eps*tst1) {
				/* if (fabs(e[m]) + fabs(d[m]+d[m+1]) == fabs(d[m]+d[m+1])) { */
				break;
			}
			m++;
			}

			/* If m == l, d[l] is an eigenvalue, */
			/* otherwise, iterate. */

			if (m > l) {  /* TODO: check the case m == n, should be rejected here!? */
				int iter = 0;
				do { /* while (fabs(e[l]) > eps*tst1); */
					double dl1, h;
					double g = d[l];
					double p = (d[l+1] - g) / (2.0 * e[l]); 
					double r = myhypot(p, 1.); 

					iter = iter + 1;  /* Could check iteration count here */

					/* Compute implicit shift */

					if (p < 0) {
						r = -r;
					}
					d[l] = e[l] / (p + r);
					d[l+1] = e[l] * (p + r);
					dl1 = d[l+1];
					h = g - d[l];
					for (i = l+2; i < n; i++) {
						d[i] -= h;
					}
					f = f + h;

					/* Implicit QL transformation. */

					p = d[m];
					{
						double c = 1.0;
						double c2 = c;
						double c3 = c;
						double el1 = e[l+1];
						double s = 0.0;
						double s2 = 0.0;
						for (i = m-1; i >= l; i--) {
							c3 = c2;
							c2 = c;
							s2 = s;
							g = c * e[i];
							h = c * p;
							r = myhypot(p, e[i]);
							e[i+1] = s * r;
							s = e[i] / r;
							c = p / r;
							p = c * d[i] - s * g;
							d[i+1] = h + s * (c * g + s * d[i]);

							/* Accumulate transformation. */

							for (k = 0; k < n; k++) {
								h = V[k][i+1];
								V[k][i+1] = s * V[k][i] + c * h;
								V[k][i] = c * V[k][i] - s * h;
							}
						}
						p = -s * s2 * c3 * el1 * e[l] / dl1;
						e[l] = s * p;
						d[l] = c * p;
					}

					/* Check for convergence. */

				} while (fabs(e[l]) > eps*tst1);
			}
			d[l] = d[l] + f;
			e[l] = 0.0;
		}

		/* Sort eigenvalues and corresponding vectors. */
#if 1
		/* TODO: really needed here? So far not, but practical and only O(n^2) */
		{
			int j; 
			double p;
			for (i = 0; i < n-1; i++) {
				k = i;
				p = d[i];
				for (j = i+1; j < n; j++) {
					if (d[j] < p) {
						k = j;
						p = d[j];
					}
				}
				if (k != i) {
					d[k] = d[i];
					d[i] = p;
					for (j = 0; j < n; j++) {
						p = V[j][i];
						V[j][i] = V[j][k];
						V[j][k] = p;
					}
				}
			}
		}
#endif 
	} /* QLalgo2 */ 


	/* ========================================================= */
	void CMAES::Householder2(int n, double **V, double *d, double *e) {
		/* 
		   Householder transformation of a symmetric matrix V into tridiagonal form. 
		   -> n             : dimension
		   -> V             : symmetric nxn-matrix
		   <- V             : orthogonal transformation matrix:
		   tridiag matrix == V * V_in * V^t
		   <- d             : diagonal
		   <- e[0..n-1]     : off diagonal (elements 1..n-1) 

		   code slightly adapted from the Java JAMA package, function private tred2()  

		 */

		int i,j,k; 

		for (j = 0; j < n; j++) {
			d[j] = V[n-1][j];
		}

		/* Householder reduction to tridiagonal form */

		for (i = n-1; i > 0; i--) {

			/* Scale to avoid under/overflow */

			double scale = 0.0;
			double h = 0.0;
			for (k = 0; k < i; k++) {
				scale = scale + fabs(d[k]);
			}
			if (scale == 0.0) {
				e[i] = d[i-1];
				for (j = 0; j < i; j++) {
					d[j] = V[i-1][j];
					V[i][j] = 0.0;
					V[j][i] = 0.0;
				}
			} else {

				/* Generate Householder vector */

				double f, g, hh;

				for (k = 0; k < i; k++) {
					d[k] /= scale;
					h += d[k] * d[k];
				}
				f = d[i-1];
				g = sqrt(h);
				if (f > 0) {
					g = -g;
				}
				e[i] = scale * g;
				h = h - f * g;
				d[i-1] = f - g;
				for (j = 0; j < i; j++) {
					e[j] = 0.0;
				}

				/* Apply similarity transformation to remaining columns */

				for (j = 0; j < i; j++) {
					f = d[j];
					V[j][i] = f;
					g = e[j] + V[j][j] * f;
					for (k = j+1; k <= i-1; k++) {
						g += V[k][j] * d[k];
						e[k] += V[k][j] * f;
					}
					e[j] = g;
				}
				f = 0.0;
				for (j = 0; j < i; j++) {
					e[j] /= h;
					f += e[j] * d[j];
				}
				hh = f / (h + h);
				for (j = 0; j < i; j++) {
					e[j] -= hh * d[j];
				}
				for (j = 0; j < i; j++) {
					f = d[j];
					g = e[j];
					for (k = j; k <= i-1; k++) {
						V[k][j] -= (f * e[k] + g * d[k]);
					}
					d[j] = V[i-1][j];
					V[i][j] = 0.0;
				}
			}
			d[i] = h;
		}

		/* Accumulate transformations */

		for (i = 0; i < n-1; i++) {
			double h; 
			V[n-1][i] = V[i][i];
			V[i][i] = 1.0;
			h = d[i+1];
			if (h != 0.0) {
				for (k = 0; k <= i; k++) {
					d[k] = V[k][i+1] / h;
				}
				for (j = 0; j <= i; j++) {
					double g = 0.0;
					for (k = 0; k <= i; k++) {
						g += V[k][i+1] * V[k][j];
					}
					for (k = 0; k <= i; k++) {
						V[k][j] -= g * d[k];
					}
				}
			}
			for (k = 0; k <= i; k++) {
				V[k][i+1] = 0.0;
			}
		}
		for (j = 0; j < n; j++) {
			d[j] = V[n-1][j];
			V[n-1][j] = 0.0;
		}
		V[n-1][n-1] = 1.0;
		e[0] = 0.0;

	} /* Housholder() */


#if 0
	/* ========================================================= */
	static void
		WriteMaxErrorInfo(cmaes_t *t)
		{
			int i,j, N=this->sp.N; 
			char *s = (char *)new_void(200+30*(N+2), sizeof(char)); s[0] = '\0';

			sprintf( s+strlen(s),"\nKomplett-Info\n");
			sprintf( s+strlen(s)," Gen       %20.12g\n", this->gen);
			sprintf( s+strlen(s)," Dimension %d\n", N);
			sprintf( s+strlen(s)," sigma     %e\n", this->sigma);
			sprintf( s+strlen(s)," lastminEW %e\n", 
					this->dLastMinEWgroesserNull);
			sprintf( s+strlen(s)," maxKond   %e\n\n", this->dMaxSignifKond);
			sprintf( s+strlen(s),"     x-Vektor          rgD     Basis...\n");
			ERRORMESSAGE( s,0,0,0);
			s[0] = '\0';
			for (i = 0; i < N; ++i)
			{
				sprintf( s+strlen(s), " %20.12e", this->rgxmean[i]);
				sprintf( s+strlen(s), " %10.4e", this->rgD[i]);
				for (j = 0; j < N; ++j)
					sprintf( s+strlen(s), " %10.2e", this->B[i][j]);
				ERRORMESSAGE( s,0,0,0);
				s[0] = '\0';
			}
			ERRORMESSAGE( "\n",0,0,0);
			free( s);
		} /* WriteMaxErrorInfo() */
#endif
