#!/bin/sh

rm foo
cp $1 bar.s
gcc -c -o foo.o bar.s
gcc -o foo main.o foo.o
#echo "1 1 1 bar" > fitness.output
#rm fitness.output
./foo fitness.output `wc -l bar.s`
