/*
** NKC.h: Class definition for the NKC-landscape model.
**
** Wim Hordijk   Last modified: 17 November 2004
*/

#ifndef _NKC_LANDSCAPE_
#define _NKC_LANDSCAPE_

#include "Random.h"


/*
** NKC_Landscape: The class implementing the NKC-landscape model.
**
** Member variables:
**   - N:        The length of the genomes.
**   - K:        The number of epistatic interactions within the same species
**               (K <= N-1).
**   - C:        The number of epistatic interactions with the other species.
**   - A:        The alphabet size.
**   - epi_int:  The type of epistatic interactions (adjacent or random).
**   - epi_loc1: A 2-D array holding the epistatic locations within the same
**               species for each gene.
**   - epi_loc2: A 2-D array holding the epistatic locations from the other
**               species for each gene.
**   - seed:     Seed for the random number generator.
**   - nseed,
**     kseed,
**     cseed:    Seed values for fitness calculation (after Terry Jones).
**   - rnd:      Pointer to a random number generator.
*/

class NKC_Landscape
{
private:
  int     N, K, C, A, epi_int, **epi_loc1, **epi_loc2;
  long    seed, *nseed, *kseed, *cseed;
  Random *rnd;

public:
  static const int ADJ=1, RND=2;
  bool             init_OK;

  NKC_Landscape (int n, int k, int c, int nbh = ADJ, int a = 2, long s = -1);
  ~NKC_Landscape ();

  void   Test    ();
  int    GetN    () { return (N); }
  int    GetK    () { return (K); }
  int    GetC    () { return (C); }
  int    GetA    () { return (A); }
  int    GetNbh  () { return (epi_int); }
  long   GetSeed () { return (seed); }
  double Fitness (int *genome, int *coev_gen);
};

#endif  /* _NKC_LANDSCAPE_ */


/*
** EOF: NKC.h
*/
