#include "RandomT.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// utility function to allocate memory (blaaagh)
static void * new_void(int n, size_t size)
{
  static char s[70];
  void *p = calloc((unsigned) n, size);
  if (p == nullptr) {
    sprintf(s, "new_void(): calloc(%ld,%ld) failed",(long)n,(long)size); // TODO: write on LOG_DEBUG
    //FATAL(s,0,0,0); // TODO: throw exceptions, motherfucker
  }
  return p;
}

// probably initialization
long RandomT::random_init(long unsigned inseed)
{
	clock_t cloc = clock();

	this->flgstored = 0;
	this->rgrand = (long *) new_void(32, sizeof(long));
	
	if (inseed < 1)
	{
		while ((long) (cloc - clock()) == 0); /* TODO: remove this for time critical applications? */
		inseed = (long unsigned)abs(100*time(nullptr)+clock());
	}

	return this->random_Start(inseed);
}

// to be probably executed before destructor
void RandomT::random_exit()
{
	free( this->rgrand );
}

// executed inside random_init...
long RandomT::random_Start(long unsigned inseed)
{
	long tmp;
	int i;

	this->flgstored = 0;
	this->startseed = inseed;

	if (inseed < 1) inseed = 1; 

	this->aktseed = inseed;
	for(i = 39; i >= 0; --i)
	{
		tmp = this->aktseed/127773;
		this->aktseed = 16807 * (this->aktseed - tmp * 127773) - 2836 * tmp;
		if (this->aktseed < 0) this->aktseed += 2147483647;
		if (i < 32)
		this->rgrand[i] = this->aktseed;
	}
	this->aktrand = this->rgrand[0];

	return inseed;
}

// number taken from a Gaussian distribution (?)
double RandomT::random_Gauss()
{
	double x1, x2, rquad, fac;

	if (this->flgstored)
	{    
		this->flgstored = 0;
		return this->hold;
	}

	do 
	{
		x1 = 2.0 * this->random_Uniform() - 1.0;
		x2 = 2.0 * this->random_Uniform() - 1.0;
		rquad = x1*x1 + x2*x2;
	}
	while(rquad >= 1 || rquad <= 0);

	fac = sqrt(-2.0*log(rquad)/rquad);
	this->flgstored = 1;
	this->hold = fac * x1;

	return fac * x2;
}

// number taken from a Uniform distribution (?)
double RandomT::random_Uniform()
{
	long tmp;

	tmp = this->aktseed/127773;
	this->aktseed = 16807 * (this->aktseed - tmp * 127773) - 2836 * tmp;

	if (this->aktseed < 0) this->aktseed += 2147483647;
	tmp = this->aktrand / 67108865;

	this->aktrand = this->rgrand[tmp];
	this->rgrand[tmp] = this->aktseed;

	return (double)(this->aktrand)/(2.147483647e9);
}
