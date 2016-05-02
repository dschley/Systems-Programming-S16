#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sock;

void error(char *msg);

void inthandler(int sig);

void *writethread(void *arg);

#endif
