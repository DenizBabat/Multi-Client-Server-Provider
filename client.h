#ifndef __CLIENTHEADER__
#define __CLIENTHEADER__ 


#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

//DEFINES
FILE *fp;
//#define PORT 8080
#define NOPROVEDER "NO PROVIDER IS AVAILABLE"
#define SHUTDOWN "SERVER SHUTDOWN"


//STRUCTURES 
typedef struct Client
{
	int pid;
	char name[20];
	char priority;
	int hw;
	double reshw;
	double solvetime;
	char addr[16];
	int port;
	int socketfdforthread;
	char proname[20];
	char message[50];
	int proprice;

}Client;


//VARIABLES
struct sockaddr_in address;
struct sockaddr_in serv_addr;
int sock = 0;
int valread;

Client client;

char addr[15];
int port;


struct sigaction user1, user2;
sigset_t sigset;


//FUNCTIONS
int  connectSocket();
double timedifference_milliseconds(struct timeval start, struct timeval finish);

void *clientThreads(void *arg);

static void sighandler(int signal);

#endif//__CLIENTHEADER__