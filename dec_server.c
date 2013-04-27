#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>

//Arguments
	int usage_summary = 0;
	char *log_file=NULL;
	int portno = 9090;
//

typedef struct vertex{
	char element;
	struct vertex *connVertices;
}vertex;

typedef struct graph{
	char element;
	struct graph *next;
	struct vertex *vertices;
}graph;

//The above structures represents linked lists
//Linked List 1: Graph: Each node is a vertex in graph linked by next pointer. List of connected vertices is linked by vertices pointer 
//Linked List 2: Vertex: Each vertex is connected to above node. List is linked by connVertices pointer

void initConfigParams(char *argv[])
{
		int i;
		for(i=0;argv[i]!=NULL;i++)
		{
			
			//printf("\n%s",argv[i]);
			if(strcmp(argv[i],"-h")==0)
			{
				//printf("\n%s",argv[i]);
				usage_summary = 1;
			}
			if(strcmp(argv[i],"-p")==0)
			{
				i=i+1;
				portno=atoi(argv[i]);
				//printf("\n%s",argv[i]);
			}
			if(strcmp(argv[i],"-l")==0)
			{
				i=i+1;
				log_file=argv[i];
				//printf("\n%s",log_file);
			}
		}
		
		
		if(argv[i+1]!=NULL)
		{
			printf("\n");
		}
		
	}


int checkPath(graph **g,char source, char dest)
{
	if(source==dest)
		return 1;
	else
	{
		graph *temp=*g;
		int sum=0;
		while(temp!=NULL)
		{
			if(temp->element==source)
			{
				vertex *v=temp->vertices;
				while(v!=NULL)
				{
					sum=sum+checkPath(g,v->element,dest);
					v=v->connVertices;
				}
			}
			temp=temp->next;
		}
		if(sum>=1)
		return 1;
		else
		return 0;
	}
}

int undoInsert(graph **g,char event1,char event2)//Remove the last inserted node/vertex
{
	graph *temp=*g;
	if(temp==NULL)
		return 0;
	else
	{
		while(temp!=NULL)
		{
			if(temp->element==event1)
			{
				vertex *v=temp->vertices;
				vertex *prev=NULL;
				while(v!=NULL && v->connVertices!=NULL)
				{
					prev=v;
					v=v->connVertices;
				}
				if(v!=NULL && v->element==event2)
				{
					if(prev!=NULL)
						prev->connVertices=NULL;
					else 
						temp->vertices=NULL;
					free(v);
					return 1;
				}
			}
			temp=temp->next;
		}
		return 0;
	}
}

int removeUnRefNodes(graph **g)
{
	graph *temp1=*g;
	graph *toBeFree=NULL;
	if(temp1==NULL)
		return 0;
	else 
	{
		int referred;
		graph *prev=NULL;
		while(temp1!=NULL)
		{
			graph *temp2=*g;
			referred=0;
			//printf("\ntemp1->element:%c",temp1->element);
			while(temp2!=NULL)
			{
				//printf("\ntemp2->element:%c",temp2->element);
				if(temp1->element!=temp2->element)
				{
					vertex *v=temp2->vertices;
					
					while(v!=NULL)
					{
						if(v->element==temp1->element)
						{
							referred=1;
							break;
						}
						v=v->connVertices;
					}
				}
				temp2=temp2->next;
			}
			if(referred==0 && temp1->vertices==NULL)
			{
				if(prev!=NULL)
				{
					prev->next=temp1->next;
				}
				else
				{
					*g=temp1->next;
				}
				toBeFree=temp1;
			}
			prev=temp1;
			temp1=temp1->next;
			if(referred==0)
			{
				prev=NULL;
				free(toBeFree);
			}
		}
		
		return 1;
	}
}

int addVertex1(graph **g, char event1, char event2)//Make this function return message: 0->Failed 1->Conflict 2->Insert Done
{
	if(event1==event2)
		return 0;
	
	if((*g)==NULL)
	{
		*g=(graph*)malloc(sizeof(graph));
		(*g)->next=NULL;
	}
	
	if((*g)->next==NULL)//If graph is empty
	{
		(*g)->element=event1;
		(*g)->next=NULL;
		
		vertex *temp=(vertex*)malloc(sizeof(vertex));//Adding vertex
		temp->element=event2;
		temp->connVertices=NULL;
		(*g)->vertices=temp;
		
		graph *newNode=(graph*)malloc(sizeof(graph));
		newNode->element=event2;
		newNode->vertices=NULL;
		newNode->next=NULL;
		
		(*g)->next=newNode;
		
		return 2;
	}
	else if(g!=NULL)//Graph already present
	{
		
		graph *temp=(*g);
		graph *prev=NULL;
		graph *newNode=NULL;
		int event1Found=0, event2Found=0;//To identify presence of event nodes
		int alreadyAdded=0;//If Event1->Event2 already added
		
		while(temp!=NULL)
		{
			if(temp->element==event1)
			{
				event1Found=1;
				vertex *v=temp->vertices;
				while(v!=NULL)
				{
					if(v->element==event2)
						alreadyAdded=1;
					v=v->connVertices;
				}
			}
			if(temp->element==event2)
			{
				event2Found=1;
			}
			prev=temp;
			temp=temp->next;
		}
		
		if(alreadyAdded==1)
		return 2;
		
		int cycleDetected=0;
		if(event1Found==1 && event2Found==1)//Check for cycle if both of event nodes are present
		{
			cycleDetected = checkPath(g,event2,event1);
		}
		
		if(cycleDetected==1)//If cycle is detected then return "Conflict Detected Message"
		{	
			return 1;
		}
		
		if(event1Found==0)//If event1 node was not found then add it to graph and corresponding event2 node
		{
			newNode=(graph*)malloc(sizeof(graph));
			newNode->element=event1;
			newNode->next=NULL;
			
			prev->next=newNode;
			
			vertex *v=(vertex*)malloc(sizeof(vertex));
			v->element=event2;
			v->connVertices=NULL;
			
			newNode->vertices=v;
		}
		else// If event1 node was found then add event2 node to vertices of event1 node
		{
			graph *t=*g;
			while(t!=NULL)
			{
				if(t->element==event1)
				{
					vertex *vtemp=t->vertices;
					while(vtemp!=NULL && vtemp->connVertices!=NULL)
						vtemp=vtemp->connVertices;
					
					vertex *newVertex=(vertex*)malloc(sizeof(vertex));
					newVertex->element=event2;
					newVertex->connVertices=NULL;
					if(vtemp!=NULL)
					{
						vtemp->connVertices=newVertex;
					}
					else
					{
						t->vertices=newVertex;
					}	
					break;
				}
				t=t->next;
			}
		}
		
		if(event2Found==0)
		{
			graph *newNode1=(graph*)malloc(sizeof(graph));
			newNode1->element=event2;
			newNode1->next=NULL;
			newNode1->vertices=NULL;
			
			graph *tempNode=*g;
			while(tempNode->next!=NULL)
			{
				tempNode=tempNode->next;
			}
			tempNode->next=newNode1;
		}
	}
	return 2;
}

char * query(graph **g,char event1, char event2)
{
	
	if(event1==event2)
		return "Check Query";
		
	//First check for presence of both events
	graph *temp=*g;
	char a[2];
	a[0]=event1;
	a[1]='\0';
	
	char b[2];
	b[0]=event2;
	b[1]='\0';
	
	char *msg=(char*)malloc(sizeof(char));
	
	if(temp==NULL)//Graph is empty
	{
		strcpy(msg,"Event not found: ");
		strcat(msg,a);
		strcat(msg," ");
		strcat(msg,b);
		return msg;
	}
	
	int event1Found=0, event2Found=0;
	
	while(temp!=NULL)
	{
		if(temp->element==event1)
		event1Found=1;
		
		if(temp->element==event2)
		event2Found=1;
		
		temp=temp->next;
	}
	
	if(event1Found==0 || event2Found==0)//If one of the events is not present
	{
		strcpy(msg,"Event not found: ");
		if(event1Found==0)
		{ 
		 strcat(msg,a);
		 strcat(msg," ");
		}
		
		if(event2Found==0)
		 strcat(msg,b);
		 
		 return msg;
	}
	
	int pathExists=checkPath(g,event1,event2);//event1->event2
	if(pathExists==1)
	{
		strcpy(msg,a);
		strcat(msg," happened before ");
		strcat(msg,b);
		return msg;
	}
	
	pathExists=checkPath(g,event2,event1);//event2->event1
	if(pathExists==1)
	{
		strcpy(msg,b);
		strcat(msg," happened before ");
		strcat(msg,a);
		return msg;
	}
	
	//If no path exists between event1 and event2 then they are concurrent
	strcpy(msg,a);
	strcat(msg," and ");
	strcat(msg,b);
	strcat(msg," are concurrent");
	return msg;
}

void reset(graph **g)//Resets the graph by deallocating the memory
{
	*g=NULL;
}

char * parseInput(char *input, graph **g)
{
	int cmdFound=0;
	int i;
	char *command;
	command=(char*)malloc(sizeof(char));
	
	int j;
	char event1,event2;
	int event1Found=0,event2Found=0, dashFound=0, arrowFound=0;
	int k=0;
	
	char *insert_msg=(char*)malloc(sizeof(char));
	char *query_msg=(char*)malloc(sizeof(char));
	char *final_msg=(char*)malloc(sizeof(char));
	
	for(i=0;input[i];i++)
	{
		if(input[i]!=' ' && cmdFound==0 && input[i]!=';')
		{
			command[k++]=input[i];
		}
		
		if((input[i]==' '||input[i]==';')&& command!=NULL && strcmp(command,"")!=0)
		{
			cmdFound=1;
			
			if(strcmp(command,"insert")==0)
			{
				char *event1Arr=(char*)malloc(sizeof(char));
				char *event2Arr=(char*)malloc(sizeof(char));
				int index=0;
				int conflictFound=0;
				
				for(j=i+1;input[j]!=';';j++)
				{
					if(input[j]!='-' && input[j]!='>' && input[j]!=' ' && dashFound==0 && arrowFound==0)
					{
						
						if(isalpha(input[j])>0)
						{
							event1=input[j];
							event1Found=1;
						}
						else
						{
							strcpy(insert_msg,"\nInvalid Input: Non Alphabetic Characters Present in insert");
							break;
						}
					}
					else if(input[j]=='-')
						dashFound=1;
					else if(dashFound==1 && input[j]=='>')
						arrowFound=1;
					else if(event1Found==1 && dashFound==1 && arrowFound==1 && input[j]!='-' && input[j]!='>')
					{
						if(isalpha(input[j])>0)
						{
							event2=input[j];
							event2Found=1;
						}
						else
						{
							strcpy(insert_msg,"\nInvalid Input: Non Alphabetic Characters Present in insert");
							break;
						}
					}
					if(event1Found && dashFound && arrowFound && event2Found && conflictFound==0)
					{
						//printf("\nCommand:%s Event1:%c Event2:%c",command,event1,event2);
						event1Found=0;
						dashFound=0;
						arrowFound=0;
						event2Found=0;
						event1Arr[index]=event1;
						event2Arr[index++]=event2;
						int result = addVertex1(g,event1,event2);
						if(result==1)
						{
							char a[2];
							a[0]=event1;
							a[1]='\0';
							char b[2];
							b[0]=event2;
							b[1]='\0';
							strcpy(insert_msg,"response from server: CONFLICT DETECTED");
							strcat(insert_msg,". INSERT FAILED for:");
							strcat(insert_msg,a);
							strcat(insert_msg,"->");
							strcat(insert_msg,b);
							
							
							conflictFound=1;
							int event_index;
							
							for(event_index=index-1;event_index>=0;event_index--)
								undoInsert(g,event1Arr[event_index],event2Arr[event_index]);
							
							removeUnRefNodes(g);
						}
						
						if(result ==0)
						{	
							strcpy(insert_msg,"response from server: INSERT FAILED");
						}
						if(result==2)
						{
							strcpy(insert_msg,"response from server: INSERT DONE");
						}		
					}
				}
				i=j;
				command=(char*)malloc(sizeof(char));
				k=0;
				cmdFound=0;
				strcat(final_msg,insert_msg);
				strcat(final_msg,"\n");
			}
			else if(strcmp(command,"query")==0)
			{
				for(j=i;input[j]!=';';j++)
				{
					if(input[j]!=' ')
					{
						if(isalpha(input[j])>0 && event1Found==0)
						{
							event1=input[j];
							event1Found=1;
						}
						else if(event1Found && isalpha(input[j])>0)
						{
							event2=input[j];
							event2Found=1;
						}
						if(event1Found && event2Found)
						{
							event1Found=0;
							event2Found=0;
							query_msg=query(g,event1,event2);
						}
					}			
				}
				i=j;
				
				command=(char*)malloc(sizeof(char));
				k=0;
				cmdFound=0;
				strcat(final_msg,"response from server:");
				strcat(final_msg,query_msg);
				strcat(final_msg,"\n");
			}
			else if(strcmp(command,"reset")==0)
			{
				reset(g);
				strcat(final_msg,"respone from server: RESET DONE");
				strcat(final_msg,"\n");
			}
		}
		else if(input[i]==';')
		{
			//semCFound=1;
			//i++;
		}
	}
	return final_msg;
}

void printGraph(graph *g)
{
	graph *temp=g;
	while(temp!=NULL)
	{
		printf("\nNode:%c->",temp->element);
		vertex *v=temp->vertices;
		while(v!=NULL)
		{
			printf("%c",v->element);
			printf(" ");
			v=v->connVertices;
		}
		temp=temp->next;
	}
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void usage(char *argv)
{
		fprintf(stderr,"\n\nUsage Summary\n");
		fprintf(stderr, "SYNOPSIS: dec_server [−h] [-p port-number] [-l file]\n");
        fprintf(stderr, "OPTIONS:\n");
        fprintf(stderr, "−h: Print a usage summary with all options and exit.\n");
        fprintf(stderr,"-p port-number : Listen on the given port. If not provided, dec_server will listen on port 9090.\n");
        fprintf(stderr,"−l file	: Log all requests and responses to the given file. If not, print all to stdout.\n");
        exit(0);
}

int main(int argc, char *argv[])
{
     initConfigParams(argv);
     
     if(usage_summary==1)
		usage(argv[0]);
     
     FILE *fp;
     char *client_ip_addr=(char*)malloc(sizeof(char));
   
     if(log_file!=NULL)
     {
		 fp=fopen(log_file,"a+");
		 fprintf(fp,"\nLog File");
		 fclose(fp);
	 }
     
     
     int sockfd, newsockfd;
     //int portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     

     graph *g=(graph*)malloc(sizeof(graph));
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
     
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     while(1)
     {
		 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		 if (newsockfd < 0)
		   error("ERROR on accept");
		 bzero(buffer,256);
		 n = read(newsockfd,buffer,255); 
		 if (n < 0) 
			error("ERROR reading from socket");
		
		client_ip_addr = inet_ntoa(cli_addr.sin_addr);
		 if(log_file==NULL)
			printf("\nInput from client %s : %s",client_ip_addr,buffer);
		 else
		 {
			fp=fopen(log_file,"a+");
			fprintf(fp,"\nInput from client %s : %s",client_ip_addr,buffer);
			fclose(fp);
		 }
		 char *result=parseInput(buffer,&g);
		 
		 if(log_file==NULL)
			printf("\n Result for client %s : %s",client_ip_addr,result);
		 else
		 {
			fp=fopen(log_file,"a+");
			fprintf(fp,"\nResult for client %s : %s",client_ip_addr,result);
			fclose(fp);
		 }
		 
		 int i=0;
		 for(i=0;result[i];i++){}
		
		 n = write(newsockfd,result,i);
		 if (n < 0) error("ERROR writing to socket");
		 
	}
     close(newsockfd);
     close(sockfd);
     if(fp!=NULL)
		fclose(fp);
     return 0; 
}
