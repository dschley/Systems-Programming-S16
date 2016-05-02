#ifndef INDEXER_C
#define INDEXER_C
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

typedef struct word_{
	char* string;
	struct word_* next;
	struct occurrence_* occs;
}word;

typedef struct occurrence_{
	char* name;
	int freq;
	struct occurrence_* next;
}occurrence;

word* list=NULL;

char* rootDir=NULL;

void insert(char* file, char* token);

int recursiveDirector(char* currPathname, char* realPath);

void writelist(char* file);

#endif
