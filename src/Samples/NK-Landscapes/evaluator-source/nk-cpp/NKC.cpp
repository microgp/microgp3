/*
** NKC.cpp: Implementation of the NKC-landscape model of Stuart Kauffman.
**          For now, the implementation assumes there is only one other
**          species that influences the fitness of the current species,
**          and that the genome length (N) and the alphabet size (A) is
**          the same for both species.
**
**          This implementation uses an idea and some code from Terry Jones
**          to calculate the fitness values. Instead of storing a table
**          with N*(2^K) fitness contributions, a unique (long) integer is
**          deterministically calculated for each gene and every possible
**          neighborhood configuration. This unique integer is then used
**          as the seed for drawing a random number which is the fitness
**          contribution of that gene given that neighborhood configuration.
**
** Wim Hordijk   Last modified: 23 August 2010
*/

#include "NKC.h"
#include <stdlib.h>
#include <iostream>
using namespace std;


/*
** NKC_Landscape: Constructor. Check the parameters and initialize the
**                variables of the model.
**
** Parameters:
**   - n:   The length of the genomes (i.e., number of genes; n > 0).
**   - k:   The number of epistatic dependencies within the species
**          (0 <= k <= n-1).
**   - c:   The number of epistatic dependencies in the other species
**          (0 <= c <= n-1).
**   - nbh: The type of epistatic interactions (optional). Allowed values:
**            ADJ: Adjacent (default).
**            RND: Random.
**   - a:   The alphabet size (optional; default = 2).
**   - s:   Seed for the random number generator (optional).
*/

NKC_Landscape::NKC_Landscape (int n, int k, int c, int nbh, int a, long s)
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
  if ((c < 0) || (c >= n))
  {
    init_OK = false;
    cerr << "Invalid number of between-species epistatic dependencies: "
	 << c << endl;
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
  C = c;
  epi_int = nbh;
  A = a;
  epi_loc1 = new int*[N];
  epi_loc2 = new int*[N];
  for (i = 0; i < N; i++)
  {
    epi_loc1[i] = new int[K];
    epi_loc2[i] = new int[C];
  }
  nseed = new long[N];
  kseed = new long[N];
  cseed = new long[N];

  /*
  ** Create a random number generator.
  */
  rnd = new Random ();
  seed = rnd->SetSeed (s);

  /*
  ** Assign the epistatic interactions within the current species.
  */
  loc = nullptr;
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
	epi_loc1[i][j] = (i+j-(K/2)+N) % N;
      }
      /*
      ** Locations to the right of gene i.
      */
      for (j = K/2; j < K; j++)
      {
	epi_loc1[i][j] = (i+1+j-(K/2)+N) % N;
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
	epi_loc1[i][j] = loc[rnd_loc];
	loc[rnd_loc] = loc[N-2-j];
      }
    }
  }
  /*
  ** Assign the between-species epistatic interactions (assumed to be random).
  */
  if (loc == nullptr)
  {
    loc = new int[N];
  }
  for (i = 0; i < N; i++)
  {
    /*
    ** Initialize available locations.
    */
    for (j = 0; j < N; j++)
    {
      loc[j] = j;
    }
    /*
    ** Choose C locations at random.
    */
    for (j = 0; j < C; j++)
    {
      rnd_loc = rnd->Unif (N-j);
      epi_loc2[i][j] = loc[rnd_loc];
      loc[rnd_loc] = loc[N-1-j];
    }
  }
  delete loc;

  /*
  ** Initialize the seed values for the fitness calculation.
  */
  for (i = 0; i < N; i++)
  {
    nseed[i] = rnd->Unif (MAX_LONG);
    kseed[i] = rnd->Unif (MAX_LONG);
    cseed[i] = rnd->Unif (MAX_LONG);
  }

 End_of_Routine:
  ;
}


/*
** ~NKC_Landscape: Clean up the mess.
*/

NKC_Landscape::~NKC_Landscape ()
{
  int i;

  if (init_OK)
  {
    for (i = 0; i < N; i++)
    {
      delete epi_loc1[i];
      delete epi_loc2[i];
    }
    delete epi_loc1;
    delete epi_loc2;
    delete nseed;
    delete kseed;
    delete cseed;
    delete rnd;
  }
}


/*
** Test: Test the initialization result. If successful, print out variables.
*/

void NKC_Landscape::Test ()
{
  int i, j;

  if (init_OK)
  {
    cout << "NKC-landscape successfully initialized." << endl
	 << "  N    = " << N << endl
	 << "  K    = " << K << endl
	 << "  C    = " << C << endl
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
	 << "  epistatic interactions (within and between species):" << endl;
    for (i = 0; i < N; i++)
    {
      cout << "    " << i << "  ";
      for (j = 0; j < K; j++)
      {
	cout << epi_loc1[i][j] << " ";
      }
      cout << "    ";
      for (j = 0; j < C; j++)
      {
	cout << epi_loc2[i][j] << " ";
      }
      cout << endl;
    }
  }
  else
  {
    cout << "NKC-landscape initialization failed..." << endl;
  }
  cout << endl;
}


/*
** Fitness: Calculate the fitness of a genome.
**
** Parameters:
**   - genome:   The genome for which to calculate the fitness.
**   - coev_gen: The "coevolutionary" genome, i.e., a genome of the other
**               species on which the fitness of the current genome depends.
**
** Returns:
**   If everything went fine: The fitness of the genome (0.0 <= fitness <= 1.0)
**   Otherwise:               -1.
*/

double NKC_Landscape::Fitness (int *genome, int *coev_gen)
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
    if ((genome[i] < 0) || (genome[i] >= A) || (coev_gen[i] < 0) ||
	(coev_gen[i] >= A))
    {
      fitness = -1;
      cerr << "Invalid gene value in one of the genomes" << endl;
      goto End_of_Routine;
    }
  }

  /*
  ** Store current state of random number generator.
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
      tmp_seed ^= genome[epi_loc1[i][j]] * kseed[epi_loc1[i][j]];
    }
    for (j = 0; j < C; j++)
    {
      tmp_seed ^= coev_gen[epi_loc2[i][j]] * cseed[epi_loc2[i][j]];
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
** EOF: NKC.cpp
*/
