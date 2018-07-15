#include "client.h"


int main(int argc, char const *argv[])
{
	FILE *fp;
    struct timeval current_time, start_time;
    double time_difference;
    double difftime;
    char buffer[50]={0};
    int ret;
    Client cl;
    char *token;
    pid_t pid;
    char clientsdirlog[25] = {"clientsdirlog"};
    char strpid[10] = {0};


    gettimeofday(&start_time, NULL);

    if (argc != 5)
    {
        fprintf(stderr,"USAGE: ./clientApp client priorty homework IP HOST\n");
        fprintf(stderr,"USAGE: ./clientApp Hileci1 C 45 127.0.0.1 5555\n");
        return -1;
    }

    
   // mkdir(clientsdirlog, 0777);

    pid= getpid();
    sprintf(strpid,"/%d", pid);
    strcat(clientsdirlog, strpid);

    fp = fopen(clientsdirlog,"a");
    if (fp == NULL)
    {
    	perror("fopen");
    	exit(-1);
    }


    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);

    user2.sa_handler = sighandler;
    user2.sa_flags = 0;
    signal(SIGINT, sighandler);


    client.pid = pid;
    strcpy(client.name,argv[1]);
    client.priority = argv[2][0];
    client.hw = atoi(argv[3]);

    strcpy(buffer, argv[4]);
    token = strtok(buffer, ":");
    strcpy(addr, token);
    token = strtok(NULL, ":");
    port = atoi(token);

    strcpy(client.addr, addr);
    client.port = port;
    client.socketfdforthread = -1;
    client.reshw = 0;

    connectSocket();


    ret = send(sock, &client, sizeof(Client), 0 );
    if (ret == -1)
    {
        perror("send");
        exit(-1);
    }
    //fcntl(sock, F_GETFL, 0);


    fprintf(stderr,"Client %s is requesting %c %d from server %s:%d\n",client.name, client.priority,
        client.hw, client.addr, client.port);

    fprintf(fp,"Client %s is requesting %c %d from server %s:%d\n",client.name, client.priority,
        client.hw, client.addr, client.port);


    ret = read(sock, &cl, sizeof(Client));


    gettimeofday(&current_time, NULL);
    time_difference = (double) timedifference_milliseconds(start_time, current_time);
    difftime = time_difference/1000.0;

    if (strcmp(cl.message,NOPROVEDER)==0)
    {
        fprintf(stderr,"NOPROVEDER AND EXIT\n");
        fprintf(fp,"NOPROVEDER AND EXIT\n");
        exit(1);
    }
    if (strcmp(cl.message,SHUTDOWN)==0)
    {
        fprintf(stderr,"SERVER SHUTDOWN\n");
        fprintf(fp,"SERVER SHUTDOWN\n");
        exit(1);
    }
    else{
    	if (cl.proprice >10000)
    	{
    		fprintf(stderr,"%s's task is not completed. Sorry!\n", client.name);
    		fprintf(fp,"%s's task is not completed. Sorry!\n", client.name);
    		fclose(fp);
    		return -1;
    	}else{
          fprintf(stderr,"%s’s task completed by %s in %.4f seconds, cos(%d)=%.4f, cost is %dTL, total time spent %.4f seconds.\n", 
        cl.name,cl.proname,cl.solvetime, cl.hw, cl.reshw, cl.proprice,difftime);

          fprintf(fp,"%s’s task completed by %s in %.4f seconds, cos(%d)=%.4f, cost is %dTL, total time spent %.4f seconds.\n", 
        cl.name,cl.proname,cl.solvetime, cl.hw, cl.reshw, cl.proprice,difftime);
       }   

    }

    fclose(fp);

    return 0;

}

int  connectSocket(){

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
      

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, addr , &serv_addr.sin_addr)<=0) 
    {
        perror("inet_pton");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr,"\nConnection Failed. Maybe server is closed\n");
        return -1;
    }
}

double timedifference_milliseconds(struct timeval start, struct timeval finish) {

    return ( (finish.tv_sec - start.tv_sec) * 1000.0f + (finish.tv_usec - start.tv_usec) / 1000.0f);

}

static void sighandler(int signal){
    if (signal == SIGINT)
    {
        fprintf(stderr,"PROCESS CAUGHT SIGINT(CTRL-C) - Hahahha (-_-)\n");
    }

    exit(1);

}