#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;

#include "drand48.h"

void 			 rand_calc_next(randbuf buf);
unsigned short*  rand_seed48(randbuf buf);
long 			 rand_jrand48(randbuf buf);
long 			 rand_nrand48(randbuf buf);
double 			 rand_erand48(randbuf buf);
static const int rand_A_0 = 0xE66D;
static const int rand_A_1 = 0xDEEC;
static const int rand_A_2 = 0x5;
static const int rand_C = 0xB;

randbuf rand_a = { rand_A_0, rand_A_1, rand_A_2 };
randbuf rand_rand48buf;

double rand_drand48(void) 
{
	return rand_erand48(rand_rand48buf);
}

long rand_lrand48(void) 
{
	return rand_nrand48(rand_rand48buf);
}

long rand_mrand48(void) 
{
	return rand_jrand48(rand_rand48buf);
}

void drand48_setStatus(const string& status)
{
	istringstream stream(status);
	stream >> rand_rand48buf[0]; 
	stream >> rand_rand48buf[1]; 
	stream >> rand_rand48buf[2];
	stream >> rand_a[0];
	stream >> rand_a[1];
	stream >> rand_a[2];
	
	if(stream.fail()) throw runtime_error("Could not restore random generator state");
}

const string drand48_getStatus()
{
	ostringstream status;
	status 
		<< rand_rand48buf[0] << ' ' 
		<< rand_rand48buf[1] << ' ' 
		<< rand_rand48buf[2] << ' '
		<< rand_a[0] << ' '
		<< rand_a[1] << ' '
		<< rand_a[2];
		
	return status.str();
}
void rand_calc_next(randbuf buf)
{
	randbuf tmp;
	long t;
	t = buf[0] * rand_a[0] + rand_C;
	tmp[0] = t & 0xffff;
	tmp[1] = (t >> 16) & 0xffff;
	t = buf[1] * rand_a[0] + buf[0] * rand_a[1] + tmp[1];
	tmp[1] = t & 0xffff;
	tmp[2] = (t >> 16) & 0xffff;
	t = buf[2] * rand_a[0] + buf[1] * rand_a[1] + buf[0] * rand_a[2] + tmp[2];
	tmp[2] = t & 0xffff;
	buf[0] = tmp[0];
	buf[1] = tmp[1];
	buf[2] = tmp[2];
}

void rand_srand48(long seed) 
{
	rand_rand48buf[1] = (seed >> 16) & 0xffff;
	rand_rand48buf[2] = seed & 0xffff;
	rand_rand48buf[0] = 0x330e;
	rand_a[0] = rand_A_0;
	rand_a[1] = rand_A_1;
	rand_a[2] = rand_A_2;
}

unsigned short* rand_seed48(randbuf buf) 
{
	static randbuf oldx;
	int i;
	for (i = 0; i < 3; i++) {
		oldx[i] = rand_rand48buf[i];
		rand_rand48buf[i] = buf[i];
	}
	rand_a[0] = rand_A_0;
	rand_a[1] = rand_A_1;
	rand_a[2] = rand_A_2;

	return (unsigned short *)&oldx;
}


long rand_jrand48(randbuf buf) 
{
	long ret;
	ret = buf[2] << 16 | buf[1];
	rand_calc_next(buf);
	return ret;
}

long rand_nrand48(randbuf buf) 
{
	return rand_jrand48(buf) & 0x7FFFFFFFL;
}

double rand_erand48(randbuf buf) 
{
	double ret;
	ret = ((buf[0] / 65536.0 + buf[1]) / 65536.0 + buf[2]) / 65536.0;
	rand_calc_next(buf);
	return ret;
}
