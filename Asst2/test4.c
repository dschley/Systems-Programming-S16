#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char** argv){

  char* a=(char*)malloc(5000);
  free(a);

  return 0;
}
