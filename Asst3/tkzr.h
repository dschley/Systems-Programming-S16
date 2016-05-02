#ifndef TKNZR_H
#define TKNZR_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct tokenizer_{
  FILE* src;
  int cp;
}tokenizer;

tokenizer* build(char* filepath);

char* findNext(tokenizer* tk);

void destroytk(tokenizer* tk);

#endif
