// include stuff

#include "TimingsT.h"

void TimingsT::timings_init()
{
	this->totaltotaltime = 0; 
	this->timings_start();
}

void TimingsT::timings_start()
{
	this->totaltime = 0;
	this->tictoctime = 0;
	this->lasttictoctime = 0;
	this->istic = 0;
	this->lastclock = clock();
	this->lasttime = time(nullptr);
	this->lastdiff = 0;
	this->tictoczwischensumme = 0;
	this->isstarted = 1;
}

double TimingsT::timings_update() {
/* returns time between last call of timings_*() and now, 
 *    should better return totaltime or tictoctime? 
 */
  double diffc, difft;
  clock_t lc = this->lastclock; /* measure CPU in 1e-6s */
  time_t lt = this->lasttime;   /* measure time in s */

  if (this->isstarted != 1){}; // TODO: throw exception, motherfucker
    //FATAL("timings_started() must be called before using timings... functions",0,0,0);

  this->lastclock = clock(); /* measures at most 2147 seconds, where 1s = 1e6 CLOCKS_PER_SEC */
  this->lasttime = time(nullptr);

  diffc = (double)(this->lastclock - lc) / CLOCKS_PER_SEC; /* is presumably in [-21??, 21??] */
  difft = difftime(this->lasttime, lt);                    /* is presumably an integer */

  this->lastdiff = difft; /* on the "save" side */

  /* use diffc clock measurement if appropriate */
  if (diffc > 0 && difft < 1000)
    this->lastdiff = diffc;

  if (this->lastdiff < 0){}; // TODO: throw exception
    //FATAL("BUG in time measurement", 0, 0, 0);

  this->totaltime += this->lastdiff;
  this->totaltotaltime += this->lastdiff;

  if (this->istic)
  {
    this->tictoczwischensumme += this->lastdiff;
    this->tictoctime += this->lastdiff;
  }

  return this->lastdiff; 
}

void TimingsT::timings_tic() {
  if (this->istic)
  { /* message not necessary ? */
    //ERRORMESSAGE("Warning: timings_tic called twice without toc",0,0,0); // TODO: exceptioooooons
    return; 
  }
  this->timings_update(); 
  this->istic = 1; 
}

double TimingsT::timings_toc()
{
  if (!this->istic)
  {
    // ERRORMESSAGE("Warning: timings_toc called without tic",0,0,0); // TODO: exception
    return -1; 
  }
  this->timings_update();
  this->lasttictoctime = this->tictoczwischensumme;
  this->tictoczwischensumme = 0;
  this->istic = 0;

  return this->lasttictoctime;
}
