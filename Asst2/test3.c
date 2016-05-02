#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char** argv){

  char* a=(char*)malloc(1000);
  char* b=(char*)malloc(1000);
  char* c=(char*)malloc(1000);
  char* d=(char*)malloc(1000);
  char* e=(char*)malloc(500);
  *a = 'a';
  *b = 'b';
  *c = 'c';
  *d = 'd';
  *e = 'e';
  free(b);
  free(d + 3);
  free(d);
  free(c);
  char* bigone = (char*)malloc(3000);
  *bigone = 'B';
  printf("a = %c, b = %c, c = %c, big = %c\n", *a, *b, *c, *bigone);

  return 0;
}
