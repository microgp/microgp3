#
# Makefile: Makefile for the NK-landscape implementation.
#
# Wim Hordijk   Last modified: 23 August 2010
#

CC     = g++
CFLAGS = -Wno-deprecated -c

nk: nk_walk.cpp Random.cpp NK.cpp Random.h NK.h
	$(CC) $(CFLAGS) nk_walk.cpp Random.cpp NK.cpp
	$(CC) -o nk_walk nk_walk.o Random.o NK.o

nkc: nkc_walk.cpp Random.cpp NKC.cpp Random.h NKC.h
	$(CC) $(CFLAGS) nkc_walk.cpp Random.cpp NKC.cpp
	$(CC) -o nkc_walk nkc_walk.o Random.o NKC.o

clean:
	rm -f *~ *.o

#
# EOF: Makefile
#
