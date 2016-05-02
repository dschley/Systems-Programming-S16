#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char** argv){

  char* a=(char*)malloc(1000);
  char* b=(char*)malloc(1000);
  char* c=(char*)malloc(1000);
  char* d=(char*)malloc(1000);
  char* e=(char*)malloc(1000);
  free(a);
  free(b);
  free(c);
  free(d);
  free(e);
  
return 0;
}
