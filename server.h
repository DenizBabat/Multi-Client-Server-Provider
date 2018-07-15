#ifndef __SERVER__
#define __SERVER__

#include <stdio.h>
#include <sys/socket.h>


#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>



//DEFINES
FILE *fp, *fpWR;
//#define PORT 8080
#define MAX 2
#define Q 'Q'
#define C 'C'
#define T 'T'
#define NOPROVEDER "NO PROVIDER IS AVAILABLE"
#define SHUTDOWN "SERVER SHUTDOWN"

//STRUCTURES
typedef struct Client
{
	int clientid;
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

typedef struct Queue
{
	Client array[MAX];
	int rear;
	int front;
	int itemCount;

}Queue;

typedef struct Provider
{
	int pr_socketfd;
	int index;
	long int pr_tid;
	char name[20];
	int performance;
	int price;
	int duration;
	int remian_dur;
	Queue queue;
	int taskcount;

}Provider;

//VARIABLES
int new_socket;
int server_fd, ret;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);


int NUMOFPROVIDER=0;

Provider *providers;
Queue queue;
pthread_t *pr_tid;
pthread_mutex_t *pr_mutex;
pthread_cond_t *pr_cond;
int *indexarr;

char filename[20], logname[20];
int port;


struct sigaction user1, user2;
sigset_t sigset;
bool flagsignal = false;


//FUNCTIONS
void connectScocket();
void *threadProvider(void* arg);
void initialize();
void readfile(const char *filename);
void create_provider();
void *centralfindproviderthread(void *arg);
bool isEmpty(Provider prov);
bool isFull(Provider prov);
int size(int index);
void insert(int pro_index, Client data);
Client removeData(int pro_index);
void destroyAndFreeFunc();
double timedifference_milliseconds(struct timeval start, struct timeval finish);
double taylorseries(int degree);
void results();
static void sighandler(int signal);



#endif