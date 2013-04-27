
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//Arguments
	int usage_summary = 0;
	char *server_host ="127.0.0.1";
	int portno = 9090;
//


void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void usage(char *argv)
{
		fprintf(stderr,"\n\nUsage Summary\n");
		fprintf(stderr, "SYNOPSIS: dec_client [−h] [-s server-host] [-p port-number]\n");
        fprintf(stderr, "OPTIONS:\n");
        fprintf(stderr, "−h: Print a usage summary with all options and exit.\n");
        fprintf(stderr,"-s server-host : : Connect to the specified host (by hostname or by IP address). If not provided, connect to the localhost.");
        fprintf(stderr,"-p port-number : Connect to the server at the given port. If not provided, connect to 9090.\n");
        exit(0);
}

void initConfigParams(char *argv[])
{
		int i;
		for(i=0;argv[i]!=NULL;i++)
		{
			if(strcmp(argv[i],"-h")==0)
			{
				//printf("\n %s",argv[i]);
				usage_summary = 1;
			}
			else if(strcmp(argv[i],"-s")==0)
			{
				i=i+1;
				server_host=argv[i];
				//printf("\n %s",argv[i]);
			}
			else if(strcmp(argv[i],"-p")==0)
			{
				i=i+1;
				//printf("\n %s",argv[i]);
				portno=atoi(argv[i]);
			}
		}
		if(argv[i+1]!=NULL)
		{
			//printf("\nargv[i+1]!=NULL");
		}
	}


int main(int argc, char *argv[])
{
    initConfigParams(argv);
    
    if(usage_summary==1)
		usage(argv[0]);

    
    int sockfd,n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    //if (argc < 3)
    //{
       //fprintf(stderr,"usage %s hostname port\n", argv[0]);
       //exit(0);
    //}
    
    //portno = atoi(portno);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    //printf("\nportno:%d server_host=%s",portno,server_host);
    server = gethostbyname(server_host);
    
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    
    printf("Please enter the input: ");
    
    bzero(buffer,256);
    
    fgets(buffer,255,stdin);
    
    n = write(sockfd,buffer,strlen(buffer));
    
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    
    n = read(sockfd,buffer,255);
    
    if (n < 0) 
         error("ERROR reading from socket");
    
    printf("%s\n",buffer);

    close(sockfd);
    return 0;
}
