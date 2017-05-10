/*
** nkc_walk.cpp: Main routine for performing random walks on an NKC-landscape.
**
** Wim Hordijk   Last modified: 23 August 2010
*/

#include "NKC.h"
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
** C:      The number of between-species interactions.
** A:      The alphabet size.
** epi:    The type of epistatic interactions (adjacent or random). This only
**         hold for the within-species interactions. Between-species
**         interactions are always random.
** seed:   The seed value for the random number generator.
** T:      The number of steps to perform in the random walk.
** s_mut:  The number of steps after which the other species is mutated. A
**         value of 0 indicates no update at all.
** nkc:    A pointer to an NKC-landscape.
** rnd:    A pointer to a random number generator.
*/

int            N, K, C, epi, T, s_mut, A, seed;
Random        *rnd;
NKC_Landscape *nkc;


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
  ** Create an NKC-landscape.
  */
  nkc = new NKC_Landscape (N, K, C, epi, A, seed);
  //nkc->Test ();
  if (!nkc->init_OK)
  {
    status = 1;
    cerr << "Could not create NKC-landscape." << endl;
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
  delete nkc;
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
  C = -1;
  A = 2;
  epi = NKC_Landscape::ADJ;
  seed = -1;
  T = 10000;
  s_mut = 0;

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
    else if (strcmp (argv[i], "-c") == 0)
    {
      if ((sscanf (argv[++i], "%d", &C) != 1) || (C < 0) || (C >= N))
      {
	status = -1;
	cerr << "Invalid value for C: " << argv[i] << endl;
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
	epi = NKC_Landscape::ADJ;
      }
      else if (strcmp (argv[i], "rnd") == 0)
      {
	epi = NKC_Landscape::RND;
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
    else if (strcmp (argv[i], "-m") == 0)
    {
      if ((sscanf (argv[++i], "%d", &s_mut) != 1) || (s_mut < 0))
      {
	status = -1;
	cerr << "Invalid value for s_mut: " << argv[i] << endl;
	goto End_of_Routine;
      }
      i++;
    }
    else if (strcmp (argv[i], "-help") == 0)
    {
      cout << argv[0] << " -n <N> -k <K> -c <C> [-a <A>] [-e <epi>]"
	   << " [-s <seed>] [-t <T>] [-m <s_mut>] [-help]" << endl
	   << endl
	   << "  N:     The genome length." << endl
	   << "  K:     The number of epistatic interactions (0<=K<N)." << endl
	   << "  C:     The number of between-species epistatic interactions "
	   << "(0<=C<=N)." << endl
	   << "  A:     The alphabet size (default=2)." << endl
	   << "  epi:   The type of epistatic interactions ('adj' (default) or"
	   << " 'rnd')." << endl
	   << "  seed:  The seed value for the landscape (default=-1)." << endl
	   << "  T:     The number of steps in the random walk (default=10000)."
	   << endl
	   << "  s_mut: The number of steps after which the other species is "
	   << endl
	   << "         mutated (default=0, no updates)." << endl
	   << "  help:  Print out this message and exit." << endl;
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
  ** Make sure the user has set at least the N, K, and C values.
  */
  if ((N < 0) || (K < 0) || (C < 0))
  {
    status = -1;
    cerr << "Expecting at least the -n, -k, and -c options..." << endl;
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
  int status, *genome, *coev_gen, i, loc, val;

  status = 0;

  /*
  ** Create two random genomes and calculate the fitness of the first one
  ** depending on the second one.
  */
  genome = new int[N];
  coev_gen = new int[N];
  for (i = 0; i < N; i++)
  {
    genome[i] = rnd->Unif (A);
    coev_gen[i] = rnd->Unif (A);
  }
  //for (loc = 0; loc < N; loc++) cout << genome[loc]; cout << "  ";
  //for (loc = 0; loc < N; loc++) cout << coev_gen[loc]; cout << "  ";
  cout << nkc->Fitness (genome, coev_gen) << endl;

  /*
  ** Perform T-1 random steps.
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
    ** See if the other species needs to be mutated.
    */
    if ((s_mut != 0) && (i % s_mut == 0))
    {
      loc = rnd->Unif (N);
      val = coev_gen[loc];
      while (val == coev_gen[loc])
      {
	val = rnd->Unif (A);
      }
      coev_gen[loc] = val;
    }
    /*
    ** Calculate its new fitness.
    */
    //for (loc = 0; loc < N; loc++) cout << genome[loc]; cout << "  ";
    //for (loc = 0; loc < N; loc++) cout << coev_gen[loc]; cout << "  ";
    cout << nkc->Fitness (genome, coev_gen) << endl;
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
