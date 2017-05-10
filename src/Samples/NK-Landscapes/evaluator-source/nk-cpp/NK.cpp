/*
** NK.cpp: Implementation of the NK-landscape model of Stuart Kauffman.
**
**         This implementation uses an idea and some code from Terry Jones
**         to calculate the fitness values. Instead of storing a table
**         with N*(2^K) fitness contributions, a unique (long) integer is
**         deterministically calculated for each gene and every possible
**         neighborhood configuration. This unique integer is then used
**         as the seed for drawing a random number which is the fitness
**         contribution of that gene given that neighborhood configuration.
**
** Wim Hordijk   Last modified: 23 August 2010
*/

#include "NK.h"
#include <stdlib.h>
#include <iostream>
using namespace std;


/*
** NK_Landscape: Constructor. Check the parameters and initialize the
**               variables of the model.
**
** Parameters:
**   - n:   The length of the genomes (i.e., number of genes; n > 0).
**   - k:   The number of epistatic dependencies (0 <= k <= n-1).
**   - nbh: The type of epistatic interactions (optional). Allowed values:
**            ADJ: Adjacent (default).
**            RND: Random.
**   - a:   The alphabet size (optional; default = 2).
**   - s:   Seed for the random number generator (optional).
*/

NK_Landscape::NK_Landscape (int n, int k, int nbh, int a, long s)
{
  int i, j, rnd_loc, *loc;

  init_OK = true;

  /*
  ** Check the parameter values.
  */
  if (n < 1)
  {
    init_OK = false;
    cerr << "Invalid genome length: " << n << endl;
    goto End_of_Routine;
  }
  if ((k < 0) || (k >= n))
  {
    init_OK = false;
    cerr << "Invalid number of epistatic dependencies: " << k << endl;
    goto End_of_Routine;
  }
  if ((nbh != ADJ) && (nbh != RND))
  {
    init_OK = false;
    cerr << "Unknown epistatic interaction type" << endl;
    goto End_of_Routine;
  }
  if (a < 2)
  {
    init_OK = false;
    cerr << "Invalid alphabet size: " << a << endl;
    goto End_of_Routine;
  }

  /*
  ** Initialize the relevant variables and allocate the necessary memory.
  */
  N = n;
  K = k;
  epi_int = nbh;
  A = a;
  epi_loc = new int*[N];
  for (i = 0; i < N; i++)
  {
    epi_loc[i] = new int[K];
  }
  nseed = new long[N];
  kseed = new long[N];

  /*
  ** Create a random number generator.
  */
  rnd = new Random ();
  seed = rnd->SetSeed (s);

  /*
  ** Assign the epistatic interactions.
  */
  if (epi_int == ADJ)
  {
    /*
    ** Adjacent epistatic interactions. If K is odd, there will be one more
    ** interaction on the right side than on the left side.
    */
    for (i = 0; i < N; i++)
    {
      /*
      ** Locations to the left of gene i.
      */
      for (j = 0; j < K/2; j++)
      {
	epi_loc[i][j] = (i+j-(K/2)+N) % N;
      }
      /*
      ** Locations to the right of gene i.
      */
      for (j = K/2; j < K; j++)
      {
	epi_loc[i][j] = (i+1+j-(K/2)+N) % N;
      }
    }
  }
  else
  {
    /*
    ** Random epistatic interactions.
    */
    loc = new int[N];
    for (i = 0; i < N; i++)
    {
      /*
      ** Initialize available locations.
      */
      for (j = 0; j < N; j++)
      {
	loc[j] = j;
      }
      loc[i] = N-1;
      /*
      ** Choose K locations at random.
      */
      for (j = 0; j < K; j++)
      {
	rnd_loc = rnd->Unif (N-1-j);
	epi_loc[i][j] = loc[rnd_loc];
	loc[rnd_loc] = loc[N-2-j];
      }
    }
    delete loc;
  }

  /*
  ** Initialize the seed values for the fitness calculation.
  */
  for (i = 0; i < N; i++)
  {
    nseed[i] = rnd->Unif (MAX_LONG);
    kseed[i] = rnd->Unif (MAX_LONG);
  }

 End_of_Routine:
  ;
}


/*
** ~NK_Landscape: Clean up the mess.
*/

NK_Landscape::~NK_Landscape ()
{
  int i;

  if (init_OK)
  {
    for (i = 0; i < N; i++)
    {
      delete epi_loc[i];
    }
    delete epi_loc;
    delete nseed;
    delete kseed;
    delete rnd;
  }
}


/*
** Test: Test the initialization result. If successful, print out variables.
*/

void NK_Landscape::Test ()
{
  int i, j;

  if (init_OK)
  {
    cout << "NK-landscape successfully initialized." << endl
	 << "  N    = " << N << endl
	 << "  K    = " << K << endl
	 << "  nbh  = ";
    if (epi_int == ADJ)
    {
      cout << "adjacent" << endl;
    }
    else
    {
      cout << "random" << endl;
    }
    cout << "  A    = " << A << endl
	 << "  seed = " << seed << endl
	 << "  epistatic interactions:" << endl;
    for (i = 0; i < N; i++)
    {
      cout << "    " << i << "  ";
      for (j = 0; j < K; j++)
      {
	cout << epi_loc[i][j] << " ";
      }
      cout << endl;
    }
  }
  else
  {
    cout << "NK-landscape initialization failed..." << endl;
  }
  cout << endl;
}


/*
** Fitness: Calculate the fitness of a genome.
**
** Parameters:
**   - genome: The genome for which to calculate the fitness.
**
** Returns:
**   If everything went fine: The fitness of the genome (0.0 <= fitness <= 1.0)
**   Otherwise:               -1.
*/

double NK_Landscape::Fitness (int *genome)
{
  int    i, j;
  long   tmp_seed;
  double fitness;

  fitness = 0.0;

  /*
  ** Check the genome content.
  */
  for (i = 0; i < N; i++)
  {
    if ((genome[i] < 0) || (genome[i] >= A))
    {
      fitness = -1;
      cerr << "Invalid gene value in genome" << endl;
      goto End_of_Routine;
    }
  }

  /*
  ** Store the current state of the random number generator.
  */
  rnd->SaveState ();
  
  /*
  ** Calculate and add the fitness contributions of each gene.
  */
  fitness = 0.0;
  for (i = 0; i < N; i++)
  {
    tmp_seed = nseed[i];
    for (j = 0; j < K; j++)
    {
      tmp_seed ^= genome[epi_loc[i][j]] * kseed[epi_loc[i][j]];
    }
    tmp_seed ^= genome[i] * kseed[i];
    rnd->SetSeed (labs (tmp_seed));
    fitness += rnd->Unif ();
  }
  fitness /= N;

  /*
  ** Restore the random number generator state.
  */
  rnd->RestoreState ();

 End_of_Routine:
  /*
  ** Return the fitness.
  */
  return (fitness);
}


/*
** EOF: NK.cpp
*/
