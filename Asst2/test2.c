#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char** argv){

  char* a=(char*)malloc(5);
  char* b=(char*)malloc(5);
  char* c=(char*)malloc(5);
  *a = 'a';
  *b = 'b';
  *c = 'c';
  printf("a = %c, b = %c, c = %c\n", *a, *b, *c);
  free(a);
  free(a);
  char* t=(char*)malloc(5);
  *t = 't';
  printf("a = %c, b = %c, c = %c, t = %c\n", *a, *b, *c, *t);

  return 0;
}
