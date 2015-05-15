/*
** nk_walk.cpp: Main routine for performing random walks on an NK-landscape.
**
** Wim Hordijk   Last modified: 23 August 2010
*/

#include "NK.h"
#include "Random.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;

/*
** Global vars.
**
** N:      The length of the genomes.
** K:      The number of epistatic interactions.
** A:      The alphabet size.
** epi:    The type of epistatic interactions (adjacent or random).
** seed:   The seed value for the random number generator.
** T:      The number of steps to perform in the random walk.
** nk:     A pointer to an NK-landscape.
** rnd:    A pointer to a random number generator.
*/

int           N, K, epi, T, A, seed;
Random       *rnd;
NK_Landscape *nk;


/*
** Function prototypes.
*/

int GetArguments (int argc, char **argv);
int RandomWalk   ();


/*
** main: Main routine of the program.
**
** Parameters:
**   - argc: The number of arguments to the program.
**   - argv: A pointer to the list of arguments.
**
** Returns:
**   If everything went fine: 0.
**   Otherwise:               1.
*/

int main (int argc, char **argv)
{
  int status;

  status = 0;

  /*
  ** Get and check the arguments.
  */
  if (GetArguments (argc, argv) == -1)
  {
    status = 1;
    goto End_of_Routine;
  }

  /*
  ** Create an NK-landscape.
  */
  nk = new NK_Landscape (N, K, epi, A, seed);
  //nk->Test ();
  if (!nk->init_OK)
  {
    status = 1;
    cerr << "Could not create NK-landscape." << endl;
    goto End_of_Routine;
  }

  /*
  ** Create a random number generator for the walk.
  */
  rnd = new Random ();
  rnd->SetSeed (-1);

  /*
  ** Perform a random walk.
  */
  if (RandomWalk () == -1)
  {
    status = 1;
    goto End_of_Routine;
  }

  /*
  ** Clean up after use...
  */
  delete nk;
  delete rnd;

 End_of_Routine:
  /*
  ** return the status.
  */
  return (status);
}


/*
** GetArguments: Get and check the command line arguments.
**
** Parameters:
**   - argc: The number of arguments to the program.
**   - argv: A pointer to the list of arguments.
** 
** Returns:
**   If everything went fine:  0.
**   Otherwise:               -1.
*/

int GetArguments (int argc, char **argv)
{
  int status, i;

  status = 0;

  /*
  ** Set defaults.
  */
  N = -1;
  K = -1;
  A = 2;
  epi = NK_Landscape::ADJ;
  seed = -1;
  T = 10000;

  /*
  ** Get and check all arguments.
  */
  i = 1;
  while (i < argc)
  {
    if (strcmp (argv[i], "-n") == 0)
    {
      if ((sscanf (argv[++i], "%d", &N) != 1) || (N < 2))
      {
	status = -1;
	cerr << "Invalid value for N: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-k") == 0)
    {
      if ((sscanf (argv[++i], "%d", &K) != 1) || (K < 0) || (K >= N))
      {
	status = -1;
	cerr << "Invalid value for K: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-a") == 0)
    {
      if ((sscanf (argv[++i], "%d", &A) != 1) || (A < 2))
      {
	status = -1;
	cerr << "Invalid value for A: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-e") == 0)
    {
      if (strcmp (argv[++i], "adj") == 0)
      {
	epi = NK_Landscape::ADJ;
      }
      else if (strcmp (argv[i], "rnd") == 0)
      {
	epi = NK_Landscape::RND;
      }
      else
      {
	status = -1;
	cerr << "Unknown type of epistatic interactions: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-s") == 0)
    {
      if (sscanf (argv[++i], "%d", &seed) != 1)
      {
	status = -1;
	cerr << "Invalid value for seed: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-t") == 0)
    {
      if ((sscanf (argv[++i], "%d", &T) != 1) || (T < 1))
      {
	status = -1;
	cerr << "Invalid value for T: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-help") == 0)
    {
      cout << argv[0] << " -n <N> -k <K> [-a <A>] [-e <epi>] [-s <seed>] "
	   << "[-t <T>] [-help]" << endl
	   << endl
	   << "  N:    The genome length." << endl
	   << "  K:    The number of epistatic interactions (0<=K<N)." << endl
	   << "  A:    The alphabet size (default=2)." << endl
	   << "  epi:  The type of epistatic interactions ('adj' (default)"
	   << " or 'rnd')." << endl
	   << "  seed: The seed value for the landscape (default=-1)." << endl
	   << "  T:    The number of steps in the random walk (default=10000)."
	   << endl
	   << "  help: Print out this message and exit." << endl;
      status = -1;
      goto End_of_Routine;
    }
    else
    {
      status = -1;
      cerr << "Unknow option " << argv[i] << endl;
      goto End_of_Routine;
    }
  }

  /*
  ** Make sure the user has set at least the N and K values.
  */
  if ((N < 0) || (K < 0))
  {
    status = -1;
    cerr << "Expecting at least the -n and -k options..." << endl;
    goto End_of_Routine;
  }

 End_of_Routine:
  /*
  ** Return the status.
  */
  return (status);
}


/*
** RandomWalk: Perform a random walk on the landscape and print out the
**             generated time series of fitness values.
**
** Returns:
**   If everything went fine:  0.
**   Otherwise:               -1.
*/

int RandomWalk ()
{
  int status, *genome, i, loc, val;

  status = 0;

  /*
  ** Create a random genome and calculate its fitness.
  */
  genome = new int[N];
  for (i = 0; i < N; i++)
  {
    genome[i] = rnd->Unif (A);
  }
  //for (loc = 0; loc < N; loc++) cout << genome[loc]; cout << "  ";
  cout << nk->Fitness (genome) << endl;

  /*
  ** Perform T-1 steps.
  */
  for (i = 1; i < T; i++)
  {
    /*
    ** Mutate the genome.
    */
    loc = rnd->Unif (N);
    val = genome[loc];
    while (val == genome[loc])
    {
      val = rnd->Unif (A);
    }
    genome[loc] = val;
    /*
    ** Calculate its new fitness.
    */
    //for (loc = 0; loc < N; loc++) cout << genome[loc]; cout << "  ";
    cout << nk->Fitness (genome) << endl;
  }

 End_of_Routine:
  /*
  ** Return the status.
  */
  return (status);
}


/*
** EOF: nk_walk.cpp
*/
