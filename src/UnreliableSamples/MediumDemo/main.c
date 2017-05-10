#include <stdio.h>
#include <stdlib.h>
#include "foo.h"

//int primes[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
int primetab[100]={0,0,1,1,0,1,0,1,0,0,
		   0,1,0,1,0,0,0,1,0,1,
		   0,0,0,1,0,0,0,0,0,1,
		   0,1,0,0,0,0,0,1,0,0,
		   0,1,0,1,0,0,0,1,0,0,
		   0,0,0,1,0,0,0,0,0,1,
		   0,1,0,0,0,0,0,1,0,0,
		   0,1,0,1,0,0,0,0,0,1,
		   0,0,0,1,0,0,0,0,0,1,
		   0,0,0,0,0,0,0,1,0,0};


int main(int argc, char *argv[]){

  int i;
  int tempfoo;
  int fitness;
  int oddeven;
//  int correct;  // violent version
  int correct[100]; // violent version 2
  FILE *fo;

  fitness=0;
  oddeven=0;
//  correct=0;
  for (i=0;i<100;i++){
    correct[i]=0;
    tempfoo=foo(i);
    switch (primetab[i]){
    case 0 :
      if (tempfoo==0){
        fitness+=2;
        correct[i]=1;
      }
      break;
    case 1 :
      if (tempfoo!=0){
        fitness+=6;
        correct[i]=1;
      }
      break;
    }
    switch (i&1){
    case 0 :
      if (tempfoo==0)
        oddeven++;
      break;
    case 1 :
      if (tempfoo!=0)
        oddeven++;
      break;
    }
  }

  fo=fopen(argv[1],"w");
  if (fo){
    for (i=0;i<100;i++){
/*      if (i<100-correct)
        fprintf(fo,"0 ");
      else
        fprintf(fo,"1 ");*/
      fprintf(fo,"%d ",correct[i]);
    }
    // fprintf(fo,"%d %d %d foo\n",fitness,oddeven,100000-atoi(argv[2]));
    fprintf(fo,"%d %d %d %d\n",fitness,10,100000-atoi(argv[2]),fitness);
    fclose(fo);
  }

  return 0;
}
