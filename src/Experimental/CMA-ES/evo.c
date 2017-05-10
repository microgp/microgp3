/*	CMA-ES loop, modified to call a python external evaluator
	by Alberto Tonda and Daniele Versino, 2011
	
	Modified for Lorenzo Pace's experiments, 2012
	
	Modified again, 2013, to keep track of the whole population (it's time to write a FUCKING C++ WRAPPER)
	
	Modified again, 2013, to work with Sebastien. No, really, WRITE A FUCKING C++ WRAPPER!
*/

#include <stdio.h>
#include <stdlib.h> /* free() */
#include <float.h>
#include "cma/cmaes_interface.h"

/* ROW is the number of values per row in the output */
#define ROW 10
#define MAX 255

#define FILE_INPUT "input.txt"
#define FILE_BEST "best.individual.txt"
#define FILE_FITNESS "fitness.txt"
#define EVALUATOR "perl evaluator.pl"
#define NAME "CMA-ES::"
#define SEPARATOR "\n"

/* fitness function */
/* n is the size of the problem, read in main() from cmaes configuration files */
double fitfun(double const *x, int n)  
{
	int i, returnValue;
	double fitness = DBL_MAX; /* max value in double */
	char buffer[MAX];
	FILE *fp;
	
	/* remove old input and fitness files */
	sprintf(buffer, "rm %s %s 2>/dev/null", FILE_INPUT, FILE_FITNESS);
	returnValue = system(buffer);

	/* create input file for evaluator */
	if( (fp = fopen(FILE_INPUT, "w")) == nullptr)
	{
		printf("%s Error while writing file %s\n", NAME, FILE_INPUT);
		exit(-1);
	}	

	/*
	for(i = 0; i < n; i++)
	{
		fprintf(fp, "%g", x[i]);
		if( (i+1) % ROW == 0)
			fprintf(fp, "\n");
		else	
			fprintf(fp, "\t");
	}
	*/
	for(i = 0; i < n; i++)
	{
		fprintf(fp, "%g", x[i]);
		if( i != n-1 ) fprintf(fp, "%s", SEPARATOR);
	}
	fclose(fp);

	/* run evaluator */
	sprintf(buffer, "%s %s", EVALUATOR, FILE_INPUT);
	returnValue = system(buffer);

	if( returnValue != 0)
	{
		printf("%s Warning, process \"%s %s\" exit with return value %d\n", NAME, EVALUATOR, FILE_INPUT, returnValue);
	}

	/* gather and return fitness value */
	if( (fp = fopen(FILE_FITNESS, "r")) == nullptr)
	{
		printf("%s Error while reading file %s\n", NAME, FILE_FITNESS);
		exit(-1); /* comment this if the evaluator can produce a non text values */
	}
	else
	{
		if( fscanf(fp, "%lf", &fitness) != 1)
		{
			printf("%s Error while reading file %s, wrong output format\n", NAME, FILE_FITNESS);
			exit(-1); 
		}
	}
	fclose(fp);

	return fitness;

}

/* save solution to file */
int save_solution(double* x, char* file, int n)
{
	FILE* fp;
	int i;

	if( (fp = fopen(file, "w")) == nullptr)
	{
		printf("%s Warning, cannot write on file \"%s\"\n", NAME, FILE_BEST);
		return -1; 
	}

	/* if everything is all right, save solution to file */
	for(i = 0; i < n; i++)
	{
		fprintf(fp, "%g", x[i]);
		if( (i+1) % ROW == 0)
			fprintf(fp, "\n");
		else	
			fprintf(fp, "\t");
	}

	fclose(fp);

	return 0;
}

/* the optimization loop */
int main(int argn, char **args) 
{
	cmaes_t evo; /* a CMA-ES type struct or "object" */
	double *arFunvals, *const*pop, *xfinal;
	int i, j; 

	/* initialize everything into the struct evo, 0 means default */
	printf("%s Setting up evolutionary process...\n", NAME);
	arFunvals = cmaes_init(&evo, 0, nullptr, nullptr, 0, 0, "initials.par"); 
	printf("%s\n", cmaes_SayHello(&evo));
	cmaes_ReadSignals(&evo, "signals.par");  /* write header and initial values */

	/* start! */
	/* printf("%s Starting evolutionary process...\n", NAME);*/
	int generation = 0;
	char filename[256];
	sprintf(filename, "cmaes-all-population.csv", generation); // TODO modify here to write/unwrite all population
	FILE* fp = fopen(filename, "a");
	if( fp == nullptr )
	{
		fprintf(stderr, "Error: cannot write on generation file %s!\n", filename);
		return -1;
	}
	
	// first the header
	fprintf(fp, "Generation,Fitness");
	for(i = 0; i < (int) cmaes_Get(&evo, "dim"); i++)
	{
		fprintf(fp, ",Chromosome%d", i);
	}
	fprintf(fp, "\n");
	fclose(fp);

	/* iterate until stop criterion holds */
	while(!cmaes_TestForTermination(&evo))
	{ 
		/* generate lambda new search points, sample population */
		pop = cmaes_SamplePopulation(&evo); /* do not change content of pop */

		/* Here you may resample each solution point pop[i] until it
		 becomes feasible, e.g. for box constraints (variable
		 boundaries). function is_feasible(...) needs to be
		 user-defined.  
		 Assumptions: the feasible domain is convex, the optimum is
		 not on (or very close to) the domain boundary, initialX is
		 feasible and initialStandardDeviations are sufficiently small
		 to prevent quasi-infinite looping.
		*/
		/* for (i = 0; i < cmaes_Get(&evo, "popsize"); ++i) 
		   while (!is_feasible(pop[i])) 
		     cmaes_ReSampleSingle(&evo, i); 
		*/
		
		// TODO make this part optional 
		// prepare and open a file with all the individuals in the generation
		sprintf(filename, "cmaes-all-population.csv", generation); // TODO modify here to write/unwrite all population
		FILE* fp = fopen(filename, "a");
		if( fp == nullptr )
		{
			fprintf(stderr, "Error: cannot write on generation file %s!\n", filename);
			return -1;
		}
		
		/* evaluate the new search points using fitfun from above */ 
		for (i = 0; i < cmaes_Get(&evo, "lambda"); ++i) 
		{
			arFunvals[i] = fitfun(pop[i], (int) cmaes_Get(&evo, "dim"));
			fprintf(fp, "%d,%g", generation, arFunvals[i]);
			for(j = 0; j < (int) cmaes_Get(&evo, "dim"); j++)
			{
				fprintf(fp, ",%g", pop[i][j]);
			}
			fprintf(fp, "\n");
		}
		
		// close file
		fclose(fp);

		/* update the search distribution used for cmaes_SampleDistribution() */
		cmaes_UpdateDistribution(&evo, arFunvals);  

		/* read instructions for printing output or changing termination conditions */ 
		cmaes_ReadSignals(&evo, "signals.par");   
		fflush(stdout); /* useful in MinGW */
		
		/* update generations */
		generation++;
	}

	printf("Stop:\n%s\n",  cmaes_TestForTermination(&evo)); /* print termination reason */
	cmaes_WriteToFile(&evo, "all", "allcmaes.dat");         /* write final results */

	/* get best estimator for the optimum, xmean */
	xfinal = cmaes_GetNew(&evo, "xmean"); /* "xbestever" might be used as well */
	cmaes_exit(&evo); /* release memory */ 

	/* do something with final solution and finally release memory */
	printf("%s saving best solution...\n", NAME);
	save_solution(xfinal, FILE_BEST, (int) cmaes_Get(&evo, "dim"));
	free(xfinal); 

	return 0;
}

