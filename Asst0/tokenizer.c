/*
 * tokenizer.c
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
  char* input;
  //integers pointing to the index in input string of the base of current token and current index in the stream
  int bp;
  int cp;
};

typedef struct TokenizerT_ TokenizerT;


char* makeToken(char* type, char* input, int bp, int cp){
  if(type==NULL || input==NULL){
    printf("ERROR: null type or input\n");
    return NULL;
  }
  char* result=(char*)calloc((cp-bp)+1,sizeof(char));

  if(result==NULL){
    printf("ERROR: could not malloc\n");
    return NULL;
  }
  strncpy(result,input+bp,cp-bp);
  printf("%s \"%s\"\n",type,result );
  return result;
}



/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 *
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {

  //make space for the tokenizer
  TokenizerT* tzr= (TokenizerT*)calloc(1,sizeof(TokenizerT));
  if(tzr==NULL){
    printf("ERROR: could not malloc\n");
    return NULL;
  }
  tzr->input=(char *)calloc(strlen(ts)+1,sizeof(char));
  if(tzr->input==NULL){
    printf("ERROR: could not malloc\n");
    return NULL;
  }
  strcpy(tzr->input,ts);
  if(ts==NULL || strlen(ts)<1){
    tzr->bp=-1;
    tzr->cp=-1;
  }else{
    tzr->bp=0;
    tzr->cp=0;
  }


  return tzr;
}



/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
  free(tk->input);
  free(tk);
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {
  char* s=tk->input;
  int bp=tk->bp;
  int cp=tk->cp;
  char* result=NULL;
  //loops through the input sting until a non whitespace character is found
  while(isspace(s[bp])!=0){//if base is whitespace, examine next index. whitespace cannot start a token

      cp++;
      bp++;
  }

  //these statements check the base of new token to determine what type it is which determines where the token ends
    if(isalpha(s[bp])!=0){//next token will be a word

      while(isalnum(s[cp])!=0){//words end when the string ends or when there is whitespace
        cp++;
      }
      result=makeToken("word",s,bp,cp);
      bp=cp;
    }else if(isdigit(s[bp])!=0){//next token will be a decimal/octal/hex/float

      if(s[bp]=='0'){//next token will be octal/hex/float

        if(s[bp+1]!='\0'){//if '0' is not the last character in the string

          if(s[bp+1]=='x' || s[bp+1]=='X'){//next token will be hex
            cp+=2;//index is at the character after 0x
            while(s[cp]!='\0' && isxdigit(s[cp])!=0){//hex number ends when string ends or when a non hex digit occurs
              cp++;
            }
            if(cp-bp==2){//if 0x is followed by non hex digits
              cp--;
              result=makeToken("decimal integer",s,bp,cp);
              bp=cp;
            }else{
              result=makeToken("hexidecimal integer",s,bp,cp);
              bp=cp;
            }

          }else if(s[bp+1]=='.'){//next token will be a float if there are more numbers following the '.'
            cp+=2;
            while(s[cp]!='\0' && isdigit(s[cp])!=0){
              cp++;
            }
            result=makeToken("float",s,bp,cp);
            bp=cp;
          }else if(s[bp+1]>='0' && s[bp+1]<='7'){//next token will be octal
            cp++;
            while(s[cp]>='0' && s[cp]<='7'){
              cp++;
            }
            result=makeToken("octal integer",s,bp,cp);
            bp=cp;
          }else{//this token consists of just the decimal 0
            cp++;
            result=makeToken("decimal integer",s,bp,cp);
            bp=cp;
          }
        }else{//0 was the last character so the token will be "0" and be of type "decimal"
          cp++;
          result=makeToken("decimal integer",s,bp,cp);
          bp=cp;
        }
      }else{//next token will be decimal/float
        cp++;
        while(isdigit(s[cp])!=0){
          cp++;
        }
        if(s[cp]=='.' && isdigit(s[cp+1])!=0){//next token will be float
          cp++;
          while(s[cp]!='\0' && isdigit(s[cp])!=0){
            cp++;
          }
          if(s[cp]=='e'){//possible scientific notation float, may just be a misleading 'e'
            if((s[cp+1]=='-' && isdigit(s[cp+2])!=0) || isdigit(s[cp+1])!=0){//made in 2 separate if statements for slightly better readability
              cp++;
              if(s[cp]=='-'){
                cp++;
              }
              while(isdigit(s[cp])!=0){

                cp++;

              }
            }
          }

          result=makeToken("float",s,bp,cp);
          bp=cp;
        }else{//next token will be a decimal
          result=makeToken("decimal integer",s,bp,cp);
          bp=cp;
        }
      }
    }else if(s[cp]!='\0'){//next token will be a C operator or non-alphanumeric character
      //this is a doozy but its pretty much unavoidable in order to identify each operator properly
      char* op=NULL;
      switch (s[cp]) {
        case '(':{
          op="left parenthesis";
          break;
        }
        case ')':{
          op="right parenthesis";
          break;
        }
        case '[':{
          op="left bracket";
          break;
        }
        case ']':{
          op="right bracket";
          break;
        }
        case '.':{
          op="dot operator";
          break;
        }
        case '-':{
          if(s[cp+1]=='>'){
            op="struct pointer";
            cp++;
          }else if(s[cp+1]=='-'){
            op="decrement";
            cp++;
          }else if(s[cp+1]=='='){
            op="minus equals";
            cp++;
          }else{
            op="minus sign";
          }
          break;
        }
        case '*':{
          if(s[cp+1]=='='){
            op="times equals";
            cp++;
          }else if(s[cp+1]=='/'){
            op="end block comment";
            cp++;
          }else{
            op="multiplication sign";
          }
          break;
        }
        case '&':{
          if(s[cp+1]=='&'){
            op="logical and";
            cp++;
          }else if(s[cp+1]=='='){
            op="bitwise and assignment";
            cp++;
          }else{
            op="bitwise and";
          }
          break;
        }
        case '~':{
          op="1's complement";
          break;
        }
        case '+':{
          if(s[cp+1]=='+'){
            op="increment";
            cp++;
          }else if(s[cp+1]=='='){
            op="plus equals";
            cp++;
          }else{
            op="addition sign";
          }
          break;
        }
        case '/':{
          if(s[cp+1]=='='){
            op="divide equals";
            cp++;
          }else if(s[cp+1]=='/'){
            op="line comment";
            cp++;
          }else if(s[cp+1]=='*'){
            op="open block comment";
            cp++;
          }else{
            op="divide";
          }
          break;
        }
        case '%':{
          if(s[cp+1]=='='){
            op="modulus equals";
            cp++;
          }else{
            op="modulus";
          }
          break;
        }
        case '>':{
          if(s[cp+1]=='>'){
            op="shift right";
            cp++;
            if(s[cp+1]=='='){
              op="shift right assignment";
              cp++;
            }
          }else if(s[cp+1]=='='){
            op="greater or equal";
            cp++;
          }else{
            op="greater than";
          }
          break;
        }
        case '<':{
          if(s[cp+1]=='<'){
            op="shift left";
            cp++;
            if(s[cp+1]=='='){
              op="shift left assignment";
              cp++;
            }
          }else if(s[cp+1]=='='){
            op="less or equal";
            cp++;
          }else{
            op="less than";
          }
          break;
        }
        case '=':{
          if(s[cp+1]=='='){
            op="equals";
            cp++;
          }else{
            op="assignment";
          }
          break;
        }
        case '!':{
          if(s[cp+1]=='='){
            op="not equals";
            cp++;
          }else{
            op="negate";
          }
          break;
        }
        case '^':{
          if(s[cp+1]=='='){
            op="bitwise exclusive or assignment";
            cp++;
          }else{
            op="bitwise exclusive or";
          }
          break;
        }
        case '|':{
          if(s[cp+1]=='='){
            op="bitwise or assignment";
            cp++;
          }else if(s[cp+1]=='|'){
            op="logical or";
            cp++;
          }else{
            op="bitwise or";
          }
          break;
        }
        case '?':{
          op="question mark";
          break;
        }
        case ':':{
          op="colon";
          break;
        }
        case ',':{
          op="comma";
          break;
        }
        case '\'':{
          op="single quote";
          break;
        }
        case '\"':{
          op="double quote";
          break;
        }
        case ';':{
          op="semicolon";
          break;
        }
        case '{':{
          op="open curly bracket";
          break;
        }
        case '}':{
          op="close curly bracket";
          break;
        }

        case '\\':{
          op="escape character";
          break;
        }

        default:{
          op="bad token";
          result=(char*)malloc(sizeof(char)*2);
          result[0]=s[cp];
          result[1]='\0';
          printf("%s [0x%X] %s\n", op,s[cp],result);
          cp++;
          bp=cp;
          tk->bp=bp;
          tk->cp=cp;
          return result;
        }

      }
      cp++;
      result=makeToken(op,s,bp,cp);
      bp=cp;
    }



  tk->bp=bp;
  tk->cp=cp;
  return result;
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
  char* c="";

  if(argc!=2){//makes sure there is only the program name and 1 string as the program's arguments
    printf("ERROR: This program expects a single argument when running the program\n" );
    return -1;
  }
  c=argv[1];
  /*
  * the following commented block (460-470) can be uncommented to make the program read a text file from the command line,
  * convert it to a single string, and tokenized from there
  */
  /*char *contents;
  long fileSize;
  FILE *file = fopen(argv[1], "r");
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  rewind(file);
  contents = malloc((fileSize + 1) * (sizeof(char)));
  fread(contents, sizeof(char), fileSize, file);
  fclose(file);
  contents[fileSize] = 0;
  c=contents;*/

  TokenizerT* t=TKCreate(c);
  char* token;
  while(t->cp>=0 && t->cp<strlen(t->input)){

    token=TKGetNextToken(t);
    free(token);
    token=NULL;
  }

  TKDestroy(t);
  return 0;
}
