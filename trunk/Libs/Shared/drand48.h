#ifndef HEADER_DRAND48
#define HEADER_DRAND48

#include <string>

typedef unsigned short randbuf[3];

double 	rand_drand48(void);
long 	rand_lrand48(void);
long 	rand_mrand48(void);
void 	rand_srand48(long seed);
const std::string drand48_getStatus();
void drand48_setStatus(const std::string& status);

#endif
