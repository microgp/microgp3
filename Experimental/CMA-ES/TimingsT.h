// class to collect timings (?), collecting everything that had to do with timings struct
// in the original cmaes.c file

#ifndef TIMINGST
#define TIMINGST

#include <time.h> 

class TimingsT
{
// attributes
private:
	/* local fields */
	clock_t lastclock;
	time_t lasttime;
	clock_t ticclock;
	time_t tictime;
	short istic;
	short isstarted; 

	double lastdiff;
	double tictoczwischensumme;

public:
	/* for outside use */
	double totaltime; /* zeroed by calling re-calling timings_start */
	double totaltotaltime;
	double tictoctime; 
	double lasttictoctime;
	
// methods
public:
	void   timings_init();
	void   timings_start(); /* fields totaltime and tictoctime */
	double timings_update();
	void   timings_tic();
	double timings_toc();
};

#endif
