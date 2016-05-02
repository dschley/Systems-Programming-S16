#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "tkzr.h"
#include "indexer.h"
#include <unistd.h>


void insert(char* file, char* token){

	if(list==NULL){//first word inserted
		word* w=(word*)malloc(sizeof(word));
		w->string=token;
		w->next=NULL;

		w->occs=(occurrence*)malloc(sizeof(occurrence));

		w->occs->name=file;
		w->occs->freq=1;
		w->occs->next=NULL;
		list=w;
		return;
	}
	word* wcur=list;
	word* wprev=NULL;
	int c;
	while(wcur != NULL){
		c=strcmp(token,wcur->string);
		if(c<0){//new token that comes before this word
			word* w=(word*)malloc(sizeof(word));
			w->string=token;
			w->next=wcur;

			w->occs=(occurrence*)malloc(sizeof(occurrence));

			w->occs->name=file;
			w->occs->freq=1;
			w->occs->next=NULL;

			if(wprev == NULL){
				list = w;
			}else{
				wprev->next=w;
			}
			return;
		}else if(c == 0){//token is in the list, create or increment freq
			occurrence* oc=wcur->occs;
			occurrence* op=NULL;

			while(oc != NULL){
				if(strcmp(file,oc->name) == 0){//found the file in occurrence list
					oc->freq++;
					//remove from list and reinsert so list maintains frequency->alphabetical order

					occurrence* tbi = oc;

					if(op==NULL){
						wcur->occs=wcur->occs->next;

					}else{
						op->next=oc->next;
					}

					oc=wcur->occs;
					op=NULL;

					while(oc!=NULL){

						if(tbi->freq > oc->freq){
							//incremented frequency goes here
							if(op==NULL){
								tbi->next=oc;
								wcur->occs=tbi;
								return;
							}else{
								tbi->next=oc;
								op->next=tbi;
								return;
							}
						}else if(tbi->freq == oc->freq){
							//incremented frequency same as current frequency, insert in alphabetical order
							if(strcmp(tbi->name,oc->name) < 0){
								//incremented occurrence comes before this current one
								if(op==NULL){
									tbi->next=oc;
									wcur->occs=tbi;
									return;
								}else{
									tbi->next=oc;
									op->next=tbi;
									return;
								}
							}
						}
						op=oc;
						oc=oc->next;
					}

					//incremented occurrence was only one in list or is still last in list

					if(op==NULL){

						tbi->next=NULL;
						wcur->occs=tbi;
						return;
					}else{

						tbi->next=NULL;
						op->next=tbi;
						return;
					}


				}
				op=oc;
				oc=oc->next;
			}
			//file wasnt in list of frequencies, make a new one

			occurrence* newo=(occurrence*)malloc(sizeof(occurrence));

			newo->name=file;
			newo->freq=1;
			newo->next=NULL;

			//insert it in alphabetical order
			oc=wcur->occs;
			op=NULL;
			while(oc!=NULL){
				if(oc->freq==1 && strcmp(newo->name,oc->name) < 0){//new occurrance comes before current occurrence
					if(op == NULL){//replace head of list with new occ
						newo->next = oc;
						wcur->occs = newo;
						return;
					}else{
						newo->next = oc;
						op->next = newo;
						return;
					}
				}
				op=oc;
				oc=oc->next;
			}

			//new occurrence goes to end of list
			if(op==NULL){

				newo->next=NULL;
				wcur->occs=newo;
				return;
			}else{

				newo->next=NULL;
				op->next=newo;
				return;
			}


		}
		wprev=wcur;
		wcur=wcur->next;
	}

	//word was not in list, put it in list
	word* w=(word*)malloc(sizeof(word));
	w->string=token;
	w->next=NULL;

	w->occs=(occurrence*)malloc(sizeof(occurrence));

	w->occs->name=file;
	w->occs->freq=1;
	w->occs->next=NULL;
	wprev->next=w;

}



void writelist(char* file)
{

	FILE *fptr;
	fptr = fopen(file, "w");
	word* wptr = list;
	occurrence* optr;
	fprintf(fptr, "{\"list\" : [\n");
	while(wptr != NULL)
	{
		fprintf(fptr, "\t{\"%s\" : [\n", wptr->string);
		optr = wptr->occs;
		while(optr != NULL)
		{
			fprintf(fptr, "\t\t{\"%s\" : %d}", optr->name, optr->freq);
			optr = optr->next;
			if(optr != NULL)
				fprintf(fptr, ",\n");
		}
		fprintf(fptr, "\n\t]}");
		wptr = wptr->next;
		if(wptr != NULL)
			fprintf(fptr, ",\n");
	}
	fprintf(fptr, "\n]}");
}

int main(int argc, char *argv[])
{
	char path[256] = "./";
	char realpath[256] = "";
	strcat(realpath, argv[2]);
	rootDir=realpath;
	if(access(argv[2], F_OK) == -1)
	{
		printf("The directory you entered doesn't exist\n");
		return 0;
	}
	if(access(argv[1], F_OK) != -1)
	{
		char response[2];
		printf("The file you want to write to already exists. Overwrite? (y/n)\n");
		scanf("%s", response);
		if(!strcmp(response, "n"))
			return 0;
		else if(strcmp(response, "y"))
		{
			printf("Not a valid response.\n");
			return 0;
		}
		else
		{
			printf("File will be overwritten\n");
		}
	}
	if(!recursiveDirector(path , realpath))
	{


		tokenizer* tk=build(realpath);
		char* token=NULL;
		token=findNext(tk);
		while(token != NULL){
			insert(realpath,token);
			token=findNext(tk);
		}
		destroytk(tk);

	}

	writelist(argv[1]);

	return 0;
}

int recursiveDirector(char* currPathname, char* realPath)
{
	DIR * dirfd = opendir(realPath);
	if(dirfd == NULL)
		return 0;
	struct dirent * anentry = NULL;
	anentry = readdir(dirfd);

	while( anentry != NULL)
	{
		if(strcmp(anentry->d_name, ".") != 0 && strcmp(anentry->d_name, "..") != 0) //why do I need this? I get junk folders twice for each dir
		{
			if( anentry->d_type == DT_REG )
			{

				char* fullpath=(char*)malloc(strlen(rootDir)+strlen(currPathname)+strlen(anentry->d_name)+1);
				fullpath[0]='\0';
				strcat(fullpath,rootDir);
				strcat(fullpath,currPathname+1);
				strcat(fullpath,anentry->d_name);



				if(access(fullpath, R_OK) != -1)
				{

					tokenizer* tk=build(fullpath);


					char* token=NULL;
					token=findNext(tk);
					char* p=(char*) calloc(strlen(currPathname)+strlen(anentry->d_name)+1,sizeof(char));
					strcat(p,currPathname+2);
					strcat(p,anentry->d_name);

					while(token != NULL){
						insert(anentry->d_name,token);
						//insert(p,token);

						token=findNext(tk);
					}

					destroytk(tk);
					free(fullpath);

				}

			}
			else if( anentry->d_type == DT_DIR )
			{

				char newPath[256] = "";
				strcat(newPath, currPathname);
				strcat(newPath, anentry->d_name);
				strcat(newPath, "/"); //newPath is now concatenated

				char newRealPath[256] = "";
				strcat(newRealPath, realPath);
				strcat(newRealPath, "/");
				strcat(newRealPath, anentry->d_name); //realPath is now concatenated

				recursiveDirector(newPath, newRealPath); //recurse
			}
		}
		anentry = readdir(dirfd);
	}
	return 1;
}
