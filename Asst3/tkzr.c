#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "tkzr.h"

tokenizer* build(char* filepath){
  FILE* file=fopen(filepath, "r");
  tokenizer* t=(tokenizer*)malloc(sizeof(tokenizer));
  t->src=file;
  t->cp=0;
  return t;
}

char* findNext(tokenizer* tk){
  char* result=NULL;
  char c= fgetc(tk->src);
  while(!((c>='A' && c<='Z') || (c>='a' && c<= 'z') )){
    if(EOF==c){
      return NULL;
    }
    c=fgetc(tk->src);
    tk->cp++;
  }
  while((c>='A' && c<='Z') || (c>='a' && c<= 'z') || (c>='0' && c<='9')){
    c=fgetc(tk->src);
    if(EOF==c){
      break;
    }
  }

  int ep=ftell(tk->src);
  if(ep - tk->cp < 1){
    return NULL;
  }
  result = (char*)malloc(sizeof(char) * (ep - tk->cp));
  fseek(tk->src, tk->cp, SEEK_SET);
  result = fgets(result, (ep - tk->cp), tk->src);

  tk->cp = ep-1;

  char tl=(char)*result;
  int i=0;
  while(tl!=0){
    *(result+i)=(char)tolower(*(result+i));
    i++;
    tl=(char)*(result+i);
  }
  return result;
}

void destroytk(tokenizer* tk){
  fclose(tk->src);
  free(tk);
}
/*
int main(int argc, char** argv){
char* s=NULL;
tokenizer* t=build("d1/f2");


s=findNext(t);

while(s!=NULL){

  printf("%s\n", s);
  s=findNext(t);

}





  return 1;
}
*/
