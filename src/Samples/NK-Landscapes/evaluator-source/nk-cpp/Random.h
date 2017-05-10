/*
** Random.h: Class definition for the random functions of Terry Jones.
**
** Wim Hordijk   Last modified: 16 November 2004
*/

#ifndef _RANDOM_H_
#define _RANDOM_H_

#define MAX_LONG 2147483647                              /* 2^31 - 1 */


/*
** Random: Class definition for a random number generator.
**
** Members:
**
*/

class Random
{
 private:
  int  inext, inext_saved, inextp, inextp_saved;
  long ma[56], ma_saved[56];

 public:
  long   SetSeed      (long seed);
  void   SaveState    ();
  void   RestoreState ();
  double Unif         ();
  long   Unif         (long n);
};

#endif  /* _RANDOM_H_ */


/*
** EOF: Random.h
*/
