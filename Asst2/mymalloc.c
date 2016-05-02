#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"




void* mymalloc(int size, char* filename, int calledline){
  if(size<=0){
    printf("Sorry! Cannot allocate anything less than or equal to 0 bytes. Error occurred on line %d in the file %s\n",calledline,filename );
    return NULL;
  }
  void* tbr=NULL;
  block* head=(block*)heap;
  long space=0;//current amount of space between blocks/end of heap
  long l=0; long r=0; long blks=0; long szs=0;


  if(head->id!=123){//no block is at the beginning of heap
    if(size+sizeof(block)>5000){
      printf("Sorry! On line %d in the file %s malloc was unable to give you memory of that size\n",calledline,filename);
      return NULL;
    }
    head->id=123;
    head->prev=NULL;
    head->next=NULL;
    head->size=size;
    head->open=0;
    tbr=(char*)((block*)&heap+1);
    return tbr;
  }
  block* ptr=head;
  block* newblock=NULL;
  for(ptr=head;ptr!=NULL;ptr=ptr->next){//look through the blocks for place to put new block/replace free block
    if(ptr->open){//current block is free
      if(ptr->next!=NULL){//current block has a next block
        l=(long)(ptr->next);
        r=(long)(ptr+1);
        space=l-r;//check space between blocks
        if(space >= size){//have enough room, new block replaces this free one

          ptr->open=0;
          ptr->size=size;
          tbr=(char*)((block*)ptr+1);
          return tbr;
        }
      }else{//no next block
        blks=(long)(ptr+1);
        szs=(long)(size+ptr->size);
        r=(long)(&heap+1);
        if(blks+szs <= r){//check if there is room on the heap for the new block/data after the last block and its data

          ptr->size=size;
          ptr->open=1;
          tbr=(char*)((block*)ptr+1);
          return tbr;
        }else{
          printf("Sorry! On line %d in the file %s malloc was unable to give you memory of that size\n",calledline,filename);
          return NULL;
        }
      }
    }else{
    //ptr is not open
      if(ptr->next!=NULL){
        l=(long)(ptr->next);
        r=(long)(ptr+1);
        space=l-r;
        r=(long)(ptr->size);
        space=space-r;
        if(space > 0 && space >= (sizeof(block)+size)){//enough free room between two blocks

          newblock=ptr+1;
          newblock=(block*)((char*)newblock+ptr->size);
          newblock->id=123;
          newblock->prev=ptr;
          newblock->next=ptr->next;
          newblock->prev->next=newblock;
          newblock->next->prev=newblock;
          newblock->size=size;
          newblock->open=0;
          tbr=(char*)((block*)newblock+1);
          return tbr;
        }
      }else{//current block is the last in the list and is not free
        blks=(long)(ptr+2);
        szs=(long)(size + ptr->size);
        r=(long)(&heap+1);
        if(blks+szs <= r){
          newblock=ptr+1;
          newblock=(block*)((char*)newblock+ptr->size);
          newblock->id=123;
          newblock->prev=ptr;
          newblock->prev->next=newblock;
          newblock->next=NULL;
          newblock->size=size;
          newblock->open=0;
          tbr=(char*)((block*)newblock+1);
          return tbr;
        }else{
          printf("Sorry! On line %d in the file %s malloc was unable to give you memory of that size\n",calledline,filename);
          return NULL;
        }
      }
    }
  }
  //got here means that no block was free to be replaced by a new block
  return NULL;
}

void myfree(void* address, char* filename, int calledline){

  if((char*)address >= (char*)&heap+sizeof(block) && (char*)address <= (char*)&heap+5000)
  {
    block* pointer = (block*)address-1;
    if(pointer->id == 123)
    {
      if(pointer->open) //address is pointing to an already freed segment
      {
        printf("In file %s, line %d, you tried to free an already freed pointer\n", filename, calledline);
        return;
      }else //if everything checks out
      {
        pointer->open = 1;
        block* previous = pointer->prev;
        block* nextone = pointer->next;
        if(previous != NULL) //combine prev and pointer if they are both free
        {
          if(previous->open)
          {
            previous->next = nextone;
            if(nextone != NULL)
              nextone->prev = previous;
            previous->size = previous->size + pointer->size + sizeof(block);
            pointer = previous;
          }
        }
        if(nextone != NULL) //combine next and pointer if they are both free
        {
          if(nextone->open)
          {
            pointer->next = nextone->next;
            pointer->size = pointer->size + nextone->size + sizeof(block);
            nextone = nextone->next;
            if(nextone != NULL)
              nextone->prev = pointer;
          }
        }
      }
    }else //address isn't a pointer that we have given
    {
      printf("In file %s, line %d, you tried to free an offset of a pointer, which is bad\n", filename, calledline);
      return;
    }
  }
  else //address is outside of heap
  {
    printf("In file %s, line %d, you tried to free a pointer that you never allocated\n", filename, calledline);
    return;
  }
}
