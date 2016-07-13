/***********************************************************************\
|                                                                       |
| Node.cc                                                               |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2016 Giovanni Squillero                            |
|                                                                       |
|-----------------------------------------------------------------------|
|                                                                       |
| This program is free software; you can redistribute it and/or modify  |
| it under the terms of the GNU General Public License as published by  |
| the Free Software Foundation, either version 3 of the License, or (at |
| your option) any later version.                                       |
|                                                                       |
| This program is distributed in the hope that it will be useful, but   |
| WITHOUT ANY WARRANTY; without even the implied warranty of            |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      |
| General Public License for more details                               |
|                                                                       |
|***********************************************************************'
| $Revision: 643 $
| $Date: 2015-02-23 14:49:36 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>

/* PROTO */
unsigned long int evolved_function(void);

/* FILE VARs */
static jmp_buf wakeup_place;
static struct itimerval new;
static void timeout(int);

int main(int argc, char *argv[])
{
    unsigned long int val;
    unsigned long int m;
    int fitness;
    
    /* Set up watchdog */
    signal(SIGVTALRM, timeout);
    new.it_interval.tv_sec = 0;
    new.it_interval.tv_usec = 0;
    new.it_value.tv_sec = 0;
    new.it_value.tv_usec = 1000;
    setitimer(ITIMER_VIRTUAL, &new, NULL);

    if(setjmp(wakeup_place)) {
	/* if the evolved_function hung, the longjmp will bring back us here */
	printf("0 XX\n");
    } else {
	/* "normal" flow */
	val = evolved_function(); 	
	fitness = 0;
	for(m = 0x1; m; m <<= 1) {
	    if(val & m) 
		++fitness;
	}
	printf("%d %lx\n", fitness, val);
    }

    return 0;
}

static void timeout(int foo)
{
    longjmp(wakeup_place, 1);
}
