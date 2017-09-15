
/* expecting rankno, noofprocessors, filename containing hostname,port

format in file
hostname1 my_server_port1
hostname2 my_server_port2

*/

#include"my_mpi.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <netdb.h>

int rank;
int comm_size;
/*can support upto 16 processors*/
char *hostname[16];
int port[16];
int sockServer;


int tokeniser(char *st,char delimiter,char *temp,int start){
	int i;
	for(i=start;st[i]!=0 && st[i]!=delimiter;i++){
		temp[i-start]=st[i];
	}
	temp[i-start]=0;
	return i;
}


int MPI_Init(int *argc, char ***argv){
	if(*argc==4){
		rank=atoi((*argv)[1]);
		comm_size=atoi((*argv)[2]);
		char *filename=(*argv)[3];
		FILE *file=fopen(filename,"r");
		char line[32];

		int i=0;
		while(fgets(line,sizeof(line),file)){
			//printf("%s\n",line);
			char *temp=(char *)malloc(sizeof(char)*32);
			int end=tokeniser(line,' ',temp,0);
			hostname[i]=temp;
			//printf("%s\n",hostname[i]);
			temp=(char *)malloc(sizeof(char)*32);
			tokeniser(line,' ',temp,end+1);
			//printf("%s\n",temp);
			port[i]=atoi(temp);
			//printf("port no %d\n",port[i]);
			i++;
		}
		fclose(file);



	int my_server_port;
	struct sockaddr_in s_serv_addr;


     sockServer = socket(AF_INET, SOCK_STREAM, 0);
     if (sockServer < 0)
    	 printf("ERROR opening socket");
     bzero((char *) &s_serv_addr, sizeof(s_serv_addr));
     my_server_port = port[rank];
     s_serv_addr.sin_family = AF_INET;
     s_serv_addr.sin_addr.s_addr = INADDR_ANY;
     s_serv_addr.sin_port = htons(my_server_port);
     if (bind(sockServer, (struct sockaddr *) &s_serv_addr,
              sizeof(s_serv_addr)) < 0)
    	 printf("ERROR on binding");
     listen(sockServer,5);


		return MPI_SUCCESS;
	}else{
		return MPI_ERR;
	}
}

int MPI_Comm_size ( int comm, int *size_ptr )
{
	*size_ptr=comm_size;
	return MPI_SUCCESS;
}
int MPI_Comm_rank ( int comm, int *rank_ptr ){
	*rank_ptr=rank;
	return MPI_SUCCESS;
}

int MPI_Send( void *buf, int count, int datatype, int dest,
                     int tag, int comm ){
	int sockSend, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    portno = port[dest];
    printf("trying to send to %d %s\n",port[dest],hostname[dest]);
    sockSend = socket(AF_INET, SOCK_STREAM, 0);
    if (sockSend < 0)
    	printf("ERROR opening socket");
    server = gethostbyname(hostname[dest]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    while (connect(sockSend,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
    	printf("waiting for connection\n");
    sleep(1);
    }
    //fgets(buffer,count,stdin);

    write(sockSend,(char *)buf,count);

    printf("sent %s\n",(char *)buf);
    close(sockSend);
    return MPI_SUCCESS;



}

int MPI_Recv( void *buf, int count, int datatype, int source,
                     int tag, int comm, MPI_Status *status ){

/*s_clilen = sizeof(s_cli_addr);
	newsockfd = accept(sockServer,
                 (struct sockaddr *) &s_cli_addr,
                 &clilen);
	char *ip = inet_ntoa(their_addr.sin_addr);
	*/

	struct sockaddr_in s_cli_addr;

	socklen_t s_clilen;

	int newsockfd, n;

	newsockfd = accept(sockServer,
                 (struct sockaddr *) &s_cli_addr,
                 &s_clilen);
     if (newsockfd < 0)
    	 printf("ERROR on accept");
     bzero((char *)buf,count+1);
     n = read(newsockfd,(char *)buf,count);
     if (n < 0) printf("ERROR reading from socket");
     printf("Here is the message: %s\n",(char *)buf);
     close(newsockfd);

     return MPI_SUCCESS;

}

int MPI_Finalize( void ){


	int dest;
	if(rank==comm_size-1){
		dest=0;
	}else{
		dest=rank+1;
	}
	int source;
	if(rank==0){
		source=comm_size-1;
	}else{
		source=rank-1;
	}
	if(rank==0){
		int tag=1;
		int Stat;
		char inmsg[6];
		MPI_Send("READY", 6, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		MPI_Recv(inmsg, 6, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
		printf(" %s %d \n",inmsg,rank);
		MPI_Send("DONE", 5, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}else if(rank==comm_size-1){
		int tag=1;
		int Stat;
		char inmsg[6];
		MPI_Recv(inmsg, 6, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
		printf(" %s %d \n",inmsg,rank);
		MPI_Send("READY", 6, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		MPI_Recv(inmsg, 5, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
		printf(" %s %d \n",inmsg,rank);
	}else{
		int tag=1;
		int Stat;
		char inmsg[6];
		MPI_Recv(inmsg, 6, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
		MPI_Send("READY", 6, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		printf(" %s %d \n",inmsg,rank);
		MPI_Recv(inmsg, 5, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
		MPI_Send("DONE", 5, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		printf(" %s %d \n",inmsg,rank);

	}
    return MPI_SUCCESS;
}



