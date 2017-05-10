// class to generate random numbers

#ifndef RANDOMT
#define RANDOMT

#include <time.h>

// pseudo random number generator instance
class RandomT
{
// attributes
public:
	/* Variables for Uniform() */
	long int startseed;
	long int aktseed;
	long int aktrand;
	long int *rgrand;
	
	/* Variables for Gauss() */
	short flgstored;
	double hold;

// methods
public:
	long   random_init(long unsigned seed /* 0==clock */);
	void   random_exit();
	double random_Gauss(); /* (0,1)-normally distributed */
	double random_Uniform();
	long   random_Start(long unsigned seed /* 0==1 */);
};

#endif
