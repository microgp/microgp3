/* --------------------------------------------------------- */
/* --------------- A Very Short Example -------------------- */
/* --------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h> /* free() */
#include <float.h>
#include "cmaes_interface.h"

#define DIM 3
#define MAX 255

#define FILE_INPUT "input.txt"
#define FILE_FITNESS "fitness.txt"
#define EVALUATOR "evaluator.py"

/*double fitfun(double const *x, int dim); 
double fitfun2(double const *x, int dim); */

double fitfun2(double const *x, int N)  /* function "cigtab" */
{
	int i, j;
	double fitness = DBL_MAX; /* max value in double */
	char buffer[MAX];
	FILE *fp;

	/* create input file for evaluator */
	if( (fp = fopen(FILE_INPUT, "w")) == nullptr)
	{
		printf("CMA-ES:: Error while writing file %s\n", FILE_INPUT);
		exit(-1);
	}	

	for(i = 0; i < N; i++)
	{
		fprintf(fp, "%g", x[i]);
		if( (i+1) % DIM == 0)
			fprintf(fp, "\n");
		else	
			fprintf(fp, "\t");
	}
	fclose(fp);

	/* run evaluator */
	sprintf(buffer, "python %s %s", EVALUATOR, FILE_INPUT);
	system(buffer);

	/* gather and return fitness value */
	if( (fp = fopen(FILE_FITNESS, "r")) == nullptr)
	{
		printf("CMA-ES:: Error while reading file %s\n", FILE_FITNESS);
		/*exit(-1);*/
	}
	else
	{
		fscanf(fp, "%lf", &fitness);
	}

	return fitness;

}
/* the objective (fitness) function to be minized */
double fitfun(double const *x, int N) { /* function "cigtab" */
  int i; 
  double sum = 1e4*x[0]*x[0] + 1e-4*x[1]*x[1];
  for(i = 2; i < N; ++i)  
    sum += x[i]*x[i]; 
  return sum;
}

/* the optimization loop */
int main(int argn, char **args) {
  cmaes_t evo; /* an CMA-ES type struct or "object" */
  double *arFunvals, *const*pop, *xfinal;
  int i; 

  /* Initialize everything into the struct evo, 0 means default */
  arFunvals = cmaes_init(&evo, 0, nullptr, nullptr, 0, 0, "initials.par"); 
  printf("%s\n", cmaes_SayHello(&evo));
  cmaes_ReadSignals(&evo, "signals.par");  /* write header and initial values */

  /* Iterate until stop criterion holds */
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

      /* evaluate the new search points using fitfun from above */ 
      for (i = 0; i < cmaes_Get(&evo, "lambda"); ++i) {
	arFunvals[i] = fitfun(pop[i], (int) cmaes_Get(&evo, "dim"));
      }

      /* update the search distribution used for cmaes_SampleDistribution() */
      cmaes_UpdateDistribution(&evo, arFunvals);  

      /* read instructions for printing output or changing termination conditions */ 
      cmaes_ReadSignals(&evo, "signals.par");   
      fflush(stdout); /* useful in MinGW */
    }
  printf("Stop:\n%s\n",  cmaes_TestForTermination(&evo)); /* print termination reason */
  cmaes_WriteToFile(&evo, "all", "allcmaes.dat");         /* write final results */

  /* get best estimator for the optimum, xmean */
  xfinal = cmaes_GetNew(&evo, "xmean"); /* "xbestever" might be used as well */
  cmaes_exit(&evo); /* release memory */ 

  /* do something with final solution and finally release memory */
  free(xfinal); 

  return 0;
}

