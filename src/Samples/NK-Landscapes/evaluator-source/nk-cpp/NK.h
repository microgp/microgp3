/*
** NK.h: Class definition for the NK-landscape model.
**
** Wim Hordijk   Last modified: 16 November 2004
*/

#ifndef _NK_LANDSCAPE_
#define _NK_LANDSCAPE_

#include "Random.h"


/*
** NK_Landscape: The class implementing the NK-landscape model.
**
** Member variables:
**   - N:       The length of the genomes.
**   - K:       The number of epistatic interactions (K <= N-1).
**   - A:       The alphabet size.
**   - epi_int: The type of epistatic interactions (adjacent or random).
**   - epi_loc: An 2-D array holding the epistatic locations for each gene.
**   - seed:    Seed for the random number generator.
**   - nseed,
**     kseed:   Seed values for fitness calculation (after Terry Jones).
**   - rnd:     Pointer to a random number generator.
*/

class NK_Landscape
{
private:
  int     N, K, A, epi_int, **epi_loc;
  long    seed, *nseed, *kseed;
  Random *rnd;

public:
  static const int ADJ=1, RND=2;
  bool             init_OK;

   NK_Landscape (int n, int k, int nbh = ADJ, int a = 2, long s = -1);
  ~NK_Landscape ();

  void   Test    ();
  int    GetN    () { return (N); }
  int    GetK    () { return (K); }
  int    GetA    () { return (A); }
  int    GetNbh  () { return (epi_int); }
  long   GetSeed () { return (seed); }
  double Fitness (int *genome);
};

#endif  /* _NK_LANDSCAPE_ */


/*
** EOF: NK.h
*/
