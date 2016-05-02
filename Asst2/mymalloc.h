#ifndef MEM_H
#define MEM_H
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )
#include <stdlib.h>


static char heap[5000];

typedef struct block_{
  short id;
  struct block_* prev;
  struct block_* next;
  short size;
  short open; //set to 1 if open, this is to make it easy to pass it as it is in an if statement
}block;

void* mymalloc(int size, char* filename, int calledline);

void myfree(void* address, char* filename, int calledline);

#endif
