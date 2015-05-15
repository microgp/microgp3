/*
** Random.cpp: Implementation of a class for generating uniformly
**             distributed random numbers.
**             Uses Terry Jones' code for generating UNIF[0,1) numbers.
**
** Wim Hordijk   Last modified: 16 November 2004
*/

#include "Random.h"
#include <time.h>

#define MBIG  2147483647     /* 2^31 - 1 */
#define FAC   (1.0 / MBIG)


/*
** SetSeed: Initialize the random number generator. Code taken from
**          Terry Jones' random.h.
**
** Parameters:
**   - seed: The seed value to use. If seed<0, it will be set according to
**           the current time.
**
** Returns:
**   The seed value used.
*/

long Random::SetSeed (long seed)
{
  register int i;
  register int k;
  long         mj, mk;

  /*
  ** Check the seed value.
  */
  if (seed < 0)
  {
    seed = (long)time (nullptr);
  }

  /*
  ** Initialize the random number generator.
  */
  ma[55] = mj = seed;
  mk = 1;
  for (i = 1; i <= 54; i++)
  {
    register int ii = (21 * i) % 55;
    ma[ii] = mk;
    mk = mj - mk;
    if (mk < 0)
    {
      mk += MBIG;
    }
    mj = ma[ii];
  }
  
  for (k = 0; k < 4; k++)
  {
    for (i = 1; i <= 55; i++)
    {
      ma[i] -= ma[1 + (i + 30) % 55];
      if (ma[i] < 0)
      {
	ma[i] += MBIG;
      }
    }
  }
  
  inext = 0;
  inextp = 31;

  /*
  ** Return the seed value used.
  */
  return (seed);
}


/*
** SaveState: Save the current state of the random number generator.
*/

void Random::SaveState ()
{
  int i;
  
  /*
  ** Copy the relevant variables.
  */
  inext_saved = inext;
  inextp_saved = inextp;
  for (i = 1; i < 56; i++)
  {
    ma_saved[i] = ma[i];
  }
}


/*
** RestoreState: Restore the state of the random number generator from the
**               last time it was saved.
*/

void Random::RestoreState ()
{
  int i;

  /*
  ** Copy the relevant variables back.
  */
  inext = inext_saved;
  inextp = inextp_saved;
  for (i = 1; i < 56; i++)
  {
    ma[i] = ma_saved[i];
  }
}


/*
** Unif: Generate a random number from the UNIF[0,1) distribution.
**       Code taken from Terry Jones' random.h.
**
** Returns:
**   A random number (double) from UNIF[0,1).
*/

double Random::Unif ()
{
  long mj;
    
  if (++inext == 56)
  {
    inext = 1;
  }
    
  if (++inextp == 56)
  {
    inextp = 1;
  }
  
  mj = ma[inext] - ma[inextp];
    
  if (mj < 0)
  {
    mj += MBIG;
  }

  ma[inext] = mj;
  return mj * FAC;
}


/*
** Unif: Generate a random number between 0 and n-1 (inclusive) with
**       uniform probability.
**
** Parameters:
**   - n: The upper limit of the range to draw from + 1.
**
** Returns:
**   A random number (long) from the UNIF[0,n-1] distribution.
*/

long Random::Unif (long n)
{
  return ((long)(Unif () * n));
}


/*
** EOF: Random.cpp
*/
