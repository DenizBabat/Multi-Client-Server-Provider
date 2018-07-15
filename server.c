#include "server.h"


int main(int argc, char *argv[])
{

    
    int i = 0, err = 0;
    pthread_t ctid;
    
    if (argc != 4)
    {
        printf("USAGE: $homeworkServer 5555 data.dat log.dat\n");
        return -1;
    }


    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);

    user2.sa_handler = sighandler;
    user2.sa_flags = 0;
    signal(SIGINT, sighandler);


    memset(filename,0,20);
    memset(logname,0,20);
    port = atoi(argv[1]);
    strcpy(filename, argv[2]);
    strcpy(logname, argv[3]);

    readfile(filename);

    fpWR = fopen(logname, "a");
    if (fpWR == NULL)
    {
        perror("fopen");
        exit(-1);
    }



    fprintf(stderr,"Logs kept at %s\n", logname);
    fprintf(stderr,"Name\tPerformance\tPrice\tDuration\n");

    fprintf(fpWR,"Logs kept at %s\n", logname);
    fprintf(fpWR,"Name\tPerformance\tPrice\tDuration\n");
    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        fprintf(stderr,"%s\t%d\t\t%d\t%d\n",
        providers[i].name, providers[i].performance, providers[i].price, 
        providers[i].duration );

        fprintf(fpWR,"%s\t%d\t\t%d\t%d\n",
        providers[i].name, providers[i].performance, providers[i].price, 
        providers[i].duration );

    }

    initialize();

    fprintf(stderr,"%d provider threads created\n", NUMOFPROVIDER);
    fprintf(fpWR,"%d provider threads created\n", NUMOFPROVIDER);
    create_provider();
    

    connectScocket();

    while(1){

        signal(SIGINT, sighandler);

        fprintf(stderr,"Server is waiting for client connections at port 5555\n");
        fprintf(fpWR,"Server is waiting for client connections at port 5555\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        err = pthread_create(&ctid, NULL, centralfindproviderthread, (void*)&new_socket);
        if (err != 0){
            perror("pthread_create");
            exit(-1);
        }

    }

    pthread_join(ctid, NULL);

    destroyAndFreeFunc();

	return 0;
}

void *centralfindproviderthread(void *arg){


    int socket = *(int *) arg;
    Client cl;
    int i = 0;
    int durat; //duration
    int p;//pergormans;
    int c;//cost
    int s;//speed
    int maxper = 0;
    int minprice = 100000;
    int mindur = 100000;
    int ind = 0;
    bool flag = false;


    ret = read(socket, &cl, sizeof(Client));
    if (ret<0)
    {
        perror("read");
        exit(-1);
    }


    cl.socketfdforthread = socket;

    

    while(i<NUMOFPROVIDER) 
    {
        if (cl.priority == Q){

            if (providers[i].index != -1)
            {

                if (isFull(providers[i]) == false)
                {
                    p = providers[i].performance;

                    if (p>maxper)
                    {
                        
                        maxper = p;
                        ind = i;
                        flag = true;
                    }
                    
                }
                
            }
           
        }else if (cl.priority == C)
        {

            if (providers[i].index != -1){

                if (isFull(providers[i]) == false)
                {
                    c = providers[i].price;
  
                    if (c<minprice)
                    {
                        minprice = c;
                        ind = i;
                        flag = true;
                    }
                }
            }
        }
        else if (cl.priority == T)
        {
            if (providers[i].index != -1)
            {
                if (isFull(providers[i]) == false){
                    s = providers[i].remian_dur;
                    if (s<mindur)
                    {
                        mindur = s;
                        ind = i;
                        flag = true;
                    }
                }
            }
        }

        ++i;
    }

    if (flagsignal == true)
    {
        memset(cl.message,0,50);
        strcpy(cl.message,SHUTDOWN );

         if(send(socket, &cl, sizeof(Client),0) == -1){
            perror("send");
            exit(-1);
        }
        return NULL;
    }


    if (flag == true)
    {
        pthread_mutex_lock(&pr_mutex[ind]);

        insert(ind, cl);

        fprintf(stderr,"Client %s (%c %d) connected, forwarded to provider %s\n", cl.name, cl.priority, cl.hw, providers[ind].name);

        fprintf(fpWR,"Client %s (%c %d) connected, forwarded to provider %s\n", cl.name, cl.priority, cl.hw, providers[ind].name);

        pthread_cond_signal(&pr_cond[ind]);
        pthread_mutex_unlock(&pr_mutex[ind]);
    }else{
        memset(cl.message,0,50);
        strcpy(cl.message,NOPROVEDER );

         if(send(socket, &cl, sizeof(Client),0) == -1){
            perror("send");
            exit(-1);
        }

    }



    return NULL;
}


void *threadProvider(void* arg){

    int clientpid = 0, ret =0;
    Provider pro = *(Provider *)arg;
    int pro_index = pro.index;
    long int tid  = pthread_self();
    Client nextClient;
    struct timeval current_time, start_time, nexttime;
    int time_difference;
    double difftime;
    int random;
    int task=0;

    gettimeofday(&start_time, NULL);

    while(1){

      ///  printf("time_difference: %d duration: %d\n",time_difference, (pro.duration*1000));
        if (time_difference>(pro.duration*1000) && isEmpty(providers[pro_index]) == true)
        {
            providers[pro_index].index = -1;
            break;
        }

        while(isEmpty(providers[pro_index]) == true)
        {

            if (flagsignal == true)
            {
                    
                if (isEmpty(providers[pro_index])== true && providers[pro_index].taskcount > 0)
                {
                    memset(nextClient.message,0,50);
                    strcpy(nextClient.message,SHUTDOWN);
                  //  printf("size1: %d\n",size(pro_index) );
                    if(send(nextClient.socketfdforthread, &nextClient, sizeof(Client),0) == -1){
                        perror("send");
                        exit(-1);
                    }
                     return NULL;
                }
                else {
                     return NULL;
                }
                
            }

            fprintf(stderr,"Provider %s is waiting for a task\n", pro.name);
            fprintf(fpWR,"Provider %s is waiting for a task\n", pro.name);
            pthread_cond_wait(&pr_cond[pro_index], &pr_mutex[pro_index]);
            if (flagsignal == true)
            {
                    
                if (isEmpty(providers[pro_index])== true && providers[pro_index].taskcount > 0)
                {
                    memset(nextClient.message,0,50);
                    strcpy(nextClient.message,SHUTDOWN);
                    if(send(nextClient.socketfdforthread, &nextClient, sizeof(Client),0) == -1){
                        perror("send");
                        exit(-1);
                    }
                     return NULL;
                }
                else {
                     return NULL;
                }
                
            }

        }

        gettimeofday(&current_time, NULL);
        time_difference = (int) timedifference_milliseconds(start_time, current_time);



        ret = pthread_mutex_trylock(&pr_mutex[pro_index]);
        if (ret == -1)
        {
            fprintf(stderr,"ERROR MUTEX LOCK EXIT(-1)\n");
            fprintf(fpWR,"ERROR MUTEX LOCK EXIT(-1)\n");
            exit(-1);
        }
   
        nextClient = removeData(pro_index);

        ++providers[pro_index].taskcount;
        task = providers[pro_index].taskcount;

        fprintf(stderr,"Provider %s is processing task number %d: %d\n",pro.name, task, nextClient.hw );
        fprintf(fpWR,"Provider %s is processing task number %d: %d\n",pro.name, task, nextClient.hw );
        
        pthread_mutex_unlock(&pr_mutex[pro_index]);

       // nextClient.reshw = cos(nextClient.hw);

        nextClient.reshw = taylorseries(nextClient.hw);

        random = (rand() % 16)+5;
        sleep(random);

        providers[pro_index].remian_dur = providers[pro_index].duration - random;
        strcpy(nextClient.proname,pro.name);

        gettimeofday(&nexttime, NULL);
        difftime = (double) timedifference_milliseconds(current_time, nexttime);
        nextClient.solvetime = difftime/1000.0;
        nextClient.proprice = pro.price;
        strcpy(nextClient.proname,pro.name);

        if (isEmpty(providers[pro_index])== true && providers[pro_index].taskcount > 0 && providers[pro_index].remian_dur <=0)
        {

            if (flagsignal == true)
            {
                memset(nextClient.message,0,50);
                strcpy(nextClient.message,SHUTDOWN);
                if(send(nextClient.socketfdforthread, &nextClient, sizeof(Client),0) == -1){
                    perror("send");
                    exit(-1);
                }
                 return NULL;
            
            }
            else {
                if(send(nextClient.socketfdforthread, &nextClient, sizeof(Client),0) == -1){
                    perror("send");
                    exit(-1);
                }
                fprintf(stderr,"Provider %s completed task number %d: cos(%d)=%.4f in %.4f seconds.\n", 
                    pro.name, task, nextClient.hw, nextClient.reshw, nextClient.solvetime);
                fprintf(fpWR,"Provider %s completed task number %d: cos(%d)=%.4f in %.4f seconds.\n", 
                    pro.name, task, nextClient.hw, nextClient.reshw, nextClient.solvetime);

                fprintf(stderr,"Provider %s is out\n", pro.name );
                fprintf(fpWR,"Provider %s is out\n", pro.name );

                return NULL;
            }
            
        }
        if (flagsignal == true)
        {
            memset(nextClient.message,0,50);
            strcpy(nextClient.message,SHUTDOWN);
        }


        if(send(nextClient.socketfdforthread, &nextClient, sizeof(Client),0) == -1){
            perror("send");
            exit(-1);
        }

        fprintf(stderr,"Provider %s completed task number %d: cos(%d)=%.4f in %.4f seconds.\n", 
            pro.name, task, nextClient.hw, nextClient.reshw, nextClient.solvetime);
        fprintf(fpWR,"Provider %s completed task number %d: cos(%d)=%.4f in %.4f seconds.\n", 
            pro.name, task, nextClient.hw, nextClient.reshw, nextClient.solvetime);

    }

    fprintf(stderr,"Provider %s is out\n", pro.name );
    fprintf(fpWR,"Provider %s is out\n", pro.name );

}



void create_provider(){

    int i = 0, err=0;

    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        providers[i].index = i;
        err = pthread_create(&pr_tid[i], NULL, threadProvider, (void*)&providers[i]);
        if (err != 0){
            perror("pthread_create");
            exit(-1);
        }
    
    }

}


void initialize(){
    int i = 0;
    int retval;

    pr_tid = (pthread_t *) calloc(NUMOFPROVIDER, sizeof(pthread_t));
    if (pr_tid == NULL){
        perror("phtread_t");
        exit(-1);
    }

    pr_mutex = (pthread_mutex_t *) calloc(NUMOFPROVIDER, sizeof(pthread_mutex_t));
    if (pr_mutex == NULL)
    {
        perror("pthread_mutex_t");
        exit(-1);
    }
    pr_cond = (pthread_cond_t *) calloc(NUMOFPROVIDER, sizeof(pthread_cond_t));

    i=0;
    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        providers[i].queue.itemCount = 0;
        providers[i].taskcount = 0;
        providers[i].remian_dur = providers[i].duration;
    }

    indexarr = (int *) calloc(NUMOFPROVIDER, sizeof(int));

    i = 0;
    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        if (pthread_mutex_init(&pr_mutex[i], NULL) != 0)
        {
            printf("\n mutex init failed\n");
            exit(1);
        }

        if (pthread_cond_init(&pr_cond[i], NULL) != 0) {                                    
            perror("pthread_cond_init() error");                                        
            exit(-1);
        } 
        indexarr[1] = 0;
    }


}

void destroyAndFreeFunc(){

    int i =0;

    free(indexarr);
    free(providers);

    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        pthread_cond_signal(&pr_cond[i]);
    }
    fprintf(stderr,"Terminating all clients\n");
    fprintf(stderr,"Terminating all provider\n");
    fprintf(fpWR,"Terminating all clients\n");
    fprintf(fpWR,"Terminating all provider\n");
    i = 0;
    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        
        pthread_join(pr_tid[i], NULL);
        pthread_mutex_destroy(&pr_mutex[i]);
        pthread_cond_destroy(&pr_cond[i]);

    }
}


//create socket
void connectScocket(){

   // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

}
//end of create socket

// QUEUE METHODS

int size(int index){
    return providers[index].queue.itemCount;
}

bool isEmpty(Provider prov) {

    return prov.queue.itemCount == 0;

}
bool isFull(Provider prov) {

    return prov.queue.itemCount == 2;

}
void insert(int pro_index, Client data) {

    int index=0;

    if(!isFull(providers[pro_index])) {

        if(providers[pro_index].queue.rear == MAX) {

            providers[pro_index].queue.rear = 0;            
        }   

        index = providers[pro_index].queue.rear++;

        providers[pro_index].queue.array[index] = data;

        ++providers[pro_index].queue.itemCount;

    }
}

Client removeData(int pro_index) {
   Client data;
    int index = 0;
    
    data = providers[pro_index].queue.array[providers[pro_index].queue.front++];
    
    if(providers[pro_index].queue.front == MAX) {
        providers[pro_index].queue.front = 0;
    }
    index = providers[pro_index].queue.itemCount--;

    return data;
    
}
//QUEUE END

//read file
void readfile(const char *filename){
    int LENGHT = 50;
    char line[LENGHT];
    int count=0 ,index = 0;
    char *token;
    char *name;
    int performance = 0, price = 0, duration = 0;

    fp = fopen(filename,"r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(-1);
    }

    memset(line,0,0);
    while(fgets(line, LENGHT, fp)!=NULL)
    {
        ++count;
    }

    fclose(fp);
    
    NUMOFPROVIDER = count-1;
 
    providers = (Provider*) calloc(NUMOFPROVIDER, sizeof(Provider));
    if (providers == NULL)
    {
        perror("calloc");
        exit(-1);
    }


    fp = fopen(filename,"r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(-1);
    }

    memset(line,0,LENGHT);
    count = 0;
    if(fgets(line, LENGHT, fp)==NULL){
        perror("fgets");
        exit(-1);
    }

    index = 0;
    while(fgets(line, LENGHT, fp)!=NULL )
    {
       
       if (line == NULL)
       {
            break;
       }

        
        token = strtok(line, " \n");
        strcpy(providers[index].name, token);

        token = strtok(NULL, " \n");
        providers[index].performance = atoi(token);

        token = strtok(NULL, " \n");
        providers[index].price = atoi(token);

        token = strtok(NULL, " \n");
        providers[index].duration = atoi(token);

        memset(line, 0, LENGHT);

        ++index;


    }

    fclose(fp);

}

double timedifference_milliseconds(struct timeval start, struct timeval finish) {
    return ( (finish.tv_sec - start.tv_sec) * 1000.0f + (finish.tv_usec - start.tv_usec) / 1000.0f);
}

double taylorseries(int degree){

    int n, x1, i, j;
    float x, sign, cosx, fact;

    x = degree;
    n = 10;
    x1 = x;
    /*  Degrees to radians */
    x = x * (3.142 / 180.0);
    cosx = 1;
    sign = -1;
    for (i = 2; i <= n; i = i + 2)
    {
        fact = 1;
        for (j = 1; j <= i; j++)
        {
            fact = fact * j;
        }
        cosx = cosx + (pow(x, i) / fact) * sign;
        sign = sign * (-1);
    }

    return cosx;
}


static void sighandler(int signal){
    if (signal == SIGINT)
    {
        fprintf(stderr,"CHILD CAUGHT SIGINT(CTRL-C) - Hahahha (-_-)\n");
        fprintf(fpWR,"CHILD CAUGHT SIGINT(CTRL-C) - Hahahha (-_-)\n");
    }


    flagsignal = true;
    results();
    destroyAndFreeFunc();
    
    fprintf(stderr,"Goodbye.(^_^)\n");
    fprintf(fpWR,"Goodbye.(^_^)\n");
    fclose(fpWR);
    exit(1);
}

void results(){

    int i = 0;

    fprintf(stderr,"\nStatistics\n");
    fprintf(stderr,"Name\tNumber of clients served\n");

    fprintf(fpWR,"\nStatistics\n");
    fprintf(fpWR,"Name\tNumber of clients served\n");
    for (i = 0; i < NUMOFPROVIDER; ++i)
    {
        fprintf(stderr,"%s\t%d\n",providers[i].name, providers[i].taskcount );
        fprintf(fpWR,"%s\t%d\n",providers[i].name, providers[i].taskcount );
    }

}
