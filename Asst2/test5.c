#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char** argv){
  int i;
  for(i = 0; i < 20; i++)
  {
  	malloc(250);
  }

  return 0;
}
