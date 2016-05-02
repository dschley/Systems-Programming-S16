#ifndef BANKSERVER
#define BANKSERVER
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

typedef struct bankAccount_{
  char name[100];
  float bal;
  short busy;
}account;

typedef struct connections_{
  int socket;
  struct connections_* next;
}connections;

connections* list=NULL;

account* bank[20];

pthread_mutex_t lock0, lock1, lock2, lock3, lock4, lock5, lock6, lock7, lock8, lock9, lock10, lock11, lock12, lock13, lock14, lock15, lock16, lock17, lock18, lock19, openlock;

pthread_mutex_t ring[20];

void add(int sock);

void del(int sock);

void handler(int sig);

void inthandler(int sig);

void error(char *msg);

void* newclient(void* arg);

int accountsession(int socket, int accname);

#endif
