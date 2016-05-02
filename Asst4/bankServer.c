#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include "bankServer.h"



void add(int sock){
  connections* c=(connections*)malloc(sizeof(connections));
  c->socket=sock;
  c->next=list;
  list=c;
}

void del(int sock){
  connections* ptr=list;
  connections* prev=NULL;
  for(ptr=list;ptr!=NULL;ptr=ptr->next){
    if(sock==ptr->socket){
      if(prev==NULL){
        list=list->next;
        free(ptr);
        return;
      }
      prev->next=ptr->next;
      free(ptr);
      return;
    }
    prev=ptr;
  }
}


void handler(int sig){
  printf("bank status:\n");
  int i=0;
  for(i=0;i<20;i++){
    if(bank[i]==NULL){
      printf("account %d: [    Not yet created    ]\n",i );
    }else if(!bank[i]->busy){
      printf("account %d: [Name: %s, Balance: %.2f]\n", i, bank[i]->name, bank[i]->bal);
    }else{
      printf("account %d: [Name: %s, Balance: %.2f] IN SERVICE\n", i, bank[i]->name, bank[i]->bal);
    }
  }
  alarm(20);
}

void inthandler(int sig){
  int n;
  printf("Server down. Bye bye.\n" );
  char* bye="Exiting";
  connections* ptr=list;
  connections* prev=NULL;
  while(ptr!=NULL){
    n = write(ptr->socket,bye,7);

    if (n < 0){
      printf("ERROR writing to socket: %s",strerror(errno) );
    }
    printf("client disconnected\n" );
    prev=ptr;
    ptr=ptr->next;
    free(prev);
  }
  exit(0);
}

void error(char *msg){
    perror(msg);
    exit(1);
}

void* newclient(void* arg){
  char buffer[256]; char accname[100];
  int n=-1; int count = 0; int end = 0;
  account* baccount;
  int socket= (intptr_t) arg;
  add(socket);
  int loop=1;
  n = write(socket,"You are connected",17);
  if (n < 0)
  {
    printf("ERROR writing to socket: %s",strerror(errno));
    return NULL;
  }
  while(loop){
    n = write(socket,"Accepted commands are: open, start, exit",40);
    if (n < 0)
    {
      printf("ERROR writing to socket: %s",strerror(errno));
      return NULL;
    }

    bzero(buffer,256); // zero out the char buffer to receive a client message
    n = read(socket,buffer,255);
    if (n < 0)
  	{
      del(socket);
      printf("Client interrupted, goodbye\n");
      return NULL;
  	}
    if(strcmp(buffer,"Client interrupted, goodbye")==0){ //client drops off
      del(socket);
      printf("Client interrupted, goodbye\n");
      return NULL;
    }
    if(!strncmp(buffer, "exit", 4)) //user exits
    {
      write(socket,"Exiting",7);
      del(socket);
      printf("client disconnected\n" );
      return NULL;
    }

    else if(!strncmp(buffer, "open ", 5))
    {
      pthread_mutex_lock(&openlock);
      bzero(accname, 100);
      strncpy(accname, buffer+5, strlen(buffer+5)-1);
      while(count < 20 && !end)
      {
        baccount = bank[count];
        if(baccount == NULL) //found an open spot
        {
          bank[count] = (account*)malloc(sizeof(account));
          baccount = bank[count];
          bzero(baccount->name, 100);
          strcpy(baccount->name, accname);
          baccount->bal = 0;
          baccount->busy = 0;

          n = write(socket,"Account created",15);
          if (n < 0)
          {
            printf("ERROR writing to socket: %s",strerror(errno));
            pthread_mutex_unlock(&openlock);
            return NULL;
          }
          end = 1;
        }
        else if(!strcmp(accname, baccount->name)) //accname already in system *********
        {
          n = write(socket,"Account already exists",22);
          if (n < 0)
          {
            printf("ERROR writing to socket: %s",strerror(errno));
            pthread_mutex_unlock(&openlock);
            return NULL;
          }
          end = 1;
        }
        else
        {
          count++;
        }
      }
      end = 0;
      if(count == 20) //went thought to the end without finding an open spot
      {
        n = write(socket,"Bank is full",12);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          pthread_mutex_unlock(&openlock);
          return NULL;
        }
      }
      count = 0;
      pthread_mutex_unlock(&openlock);
    }
    else if(!strncmp(buffer, "start ", 6))
    {
      bzero(accname, 100);
      strncpy(accname, buffer+6, strlen(buffer+6)-1);
      count = 0;
      end = 0;
      while(count < 20 && !end)
      {
        baccount = bank[count];
        if(baccount == NULL) //hit the end of the list
        {
          count = 20;
        }
        else if(!strcmp(accname, baccount->name)) //found the account *******
        {
          int e=accountsession(socket, count);
          if(e==0)
          {
            printf("got here\n");
            write(socket,"Exiting",7);
            del(socket);
            printf("client disconnected\n" );
            return NULL;
          }else if(e==-1){
            del(socket);
            printf("client disconnected\n" );
            return NULL;
          }
          end = 1;
        }
        else
        {
          count++;
        }
      }
      end = 0;
      if(count == 20) //went thought to the end without finding an open spot
      {
        n = write(socket,"Account doesn't exist",21);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          return NULL;
        }
      }
      count = 0;
    }
    else
    {
      n = write(socket,"Incorrect command",17);
      if (n < 0)
      {
        printf("ERROR writing to socket: %s",strerror(errno));
        return NULL;
      }
    }
  }
  return NULL;
}

int accountsession(int socket, int baccount)
{
  int n = 0; float num = 0.0;
  account* theaccount;
  char buffer[256]; char amount[20]; char balance[30];
  pthread_mutex_t* lock = &ring[baccount];
  theaccount = bank[baccount];
  while(pthread_mutex_trylock(lock) != 0)
  {

    n = write(socket,"Waiting to begin session. One moment...",39);
    if (n < 0)
    {
      printf("ERROR writing to socket: %s",strerror(errno));
      return 0;
    }

    sleep(2);
  }
  theaccount->busy = 1;
  n = write(socket,"Connected",9);
  if (n < 0)
  {
    printf("ERROR writing to socket: %s",strerror(errno));
    return 0;
  }
  while(1)
  {
    n = write(socket,"Accepted commands are: credit, debit, balance, finish, exit",59);
    if (n < 0)
    {
      printf("ERROR writing to socket: %s",strerror(errno));
      return 0;
    }

    bzero(buffer,256); // zero out the char buffer to receive a client message
    n = read(socket,buffer,255);
    if (n < 0)
    {

      printf("Client interrupted, goodbye\n");
      theaccount->busy =0;
      pthread_mutex_unlock(lock);
      return -1;
    }
    if(!strncmp(buffer, "exit", 4)) //user exits
    {
      theaccount->busy = 0;
      pthread_mutex_unlock(lock);
      return 0;
    }
    else if(!strncmp(buffer, "finish", 6)) //user finishes session
    {
      n = write(socket,"Finished session",16);
      if (n < 0)
      {
        printf("ERROR writing to socket: %s",strerror(errno));
        return 0;
      }
      theaccount->busy = 0;
      pthread_mutex_unlock(lock);
      return 1;
    }

    else if(!strncmp(buffer, "balance", 7)) //user wants to see their balance
    {
      bzero(balance, 30);
      snprintf(balance,30,"%.2f",theaccount->bal);
      n = write(socket,balance,strlen(balance));
      if (n < 0)
      {
        printf("ERROR writing to socket: %s",strerror(errno));
        return 0;
      }
    }

    else if(!strncmp(buffer, "credit ", 7))
    {
      bzero(amount, 20);
      strncpy(amount, buffer+7, strlen(buffer+7));
      num = atof(amount);
      if(num == 0 || num < 0)
      {
        n = write(socket,"Bad amount",10);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          return 0;
        }
      }
      else
      {
        theaccount->bal = theaccount->bal + num;
        n = write(socket,"Account credited",16);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          return 0;
        }
      }
    }
    else if(!strncmp(buffer, "debit ", 6))
    {
      bzero(amount, 20);
      strncpy(amount, buffer+6, strlen(buffer+6));
      num = atof(amount);
      if(num == 0 || num > theaccount->bal || num < 0)
      {
        n = write(socket,"Bad amount",10);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          return 0;
        }
      }
      else
      {
        theaccount->bal = theaccount->bal - num;
        n = write(socket,"Account debited",15);
        if (n < 0)
        {
          printf("ERROR writing to socket: %s",strerror(errno));
          return 0;
        }
      }
    }
    else
    {
      n = write(socket,"Incorrect command",17);
      if (n < 0)
      {
        printf("ERROR writing to socket: %s",strerror(errno));
        return 0;
      }
    }
  }
  return 0;
}

int main(int argc, char** argv){

    ring[0]=lock0;
    ring[1]=lock1;
    ring[2]=lock2;
    ring[3]=lock3;
    ring[4]=lock4;
    ring[5]=lock5;
    ring[6]=lock6;
    ring[7]=lock7;
    ring[8]=lock8;
    ring[9]=lock9;
    ring[10]=lock10;
    ring[11]=lock11;
    ring[12]=lock12;
    ring[13]=lock13;
    ring[14]=lock14;
    ring[15]=lock15;
    ring[16]=lock16;
    ring[17]=lock17;
    ring[18]=lock18;
    ring[19]=lock19;

    int b;
    for(b=0;b<20;b++){
      bank[b]=NULL;
    }

  	int sockfd = -1;
  	int newsockfd = -1;
  	int portno = -1;
  	socklen_t clilen = -1;

    pthread_t pth;

  	struct sockaddr_in serverAddressInfo;
  	struct sockaddr_in clientAddressInfo;


  	portno =12321; // atoi(argv[1]);


  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0)
  	{
         error("ERROR opening socket");
  	}


  	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));

  	serverAddressInfo.sin_port = htons(portno);

      serverAddressInfo.sin_family = AF_INET;

      serverAddressInfo.sin_addr.s_addr = INADDR_ANY;

      if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
  	{
  		error("ERROR on binding");
  	}

      listen(sockfd,5);

      clilen = sizeof(clientAddressInfo);

    signal(SIGINT,inthandler);
    signal(SIGALRM,handler);
    alarm(20);

    while(1) {

      newsockfd = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clilen);

      pthread_create(&pth, NULL, newclient, (void *) (intptr_t) newsockfd);
      printf("new client connected\n" );
    }


      if (newsockfd < 0)
  	{
          error("ERROR on accept");
  	}
  	return 0;
}
