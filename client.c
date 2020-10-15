/*
Name: Piotr and Carson 
Date: 26 March 2020
Class: ECE312-Communication Networks
Description: This is client.c. This program contains a chat program where the 
client will connect to the server and the two users will be able to 
talk with each other. 

To compile and run:
make clean
make 
./client 

*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/wait.h>


#define CLADDR_LEN 100   
#define PORT 4567 //default port 


void writeData(int socket);
void readData(int socket);


char *hostname = "192.168.2.244"; //default server IP
char name[256];
int status;
bool quitMessageRecieved = false;
bool isConfigured = false;
int pid, pid1;


void error(char *msg)
{
    perror(msg);
    exit(0);
}

void childSignalHandler(int sig)
{
    quitMessageRecieved = true;
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char clientAddr[CLADDR_LEN];
  char buffer[256];
  bzero(buffer,256);
  bzero(name,256);

  signal(SIGCHLD, childSignalHandler); //determine when a child process exits
  printf("Please enter the host (default or new): ");
  fgets(buffer,255,stdin);

  while(!isConfigured)
  {
    if((buffer[0] == 'd') || (buffer[0] == 'D')) //default uses program declared port
      {
        portno = PORT;
        server = gethostbyname(hostname);
        printf("Please enter your name: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        isConfigured = true;
      }
      else if((buffer[0] == 'n') || buffer[0] == 'N') //new uses user defined port
      {
        printf("Please enter the host port: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        portno = atoi(buffer);
        printf("Please enter the host ip: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        buffer[strlen(buffer) - 1] = '\0';
        hostname = buffer;
        server = gethostbyname(hostname);
        printf("Please enter your name: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        isConfigured = true;
      }
      else //wait until a valid option is selected 
      {
        printf("Invalid Option. Please enter the host (default or new):");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
      }
    }

    printf("Waiting for connection...\n");
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
      error("ERROR opening socket");
   
    if (server == NULL) 
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) //connects to server
      error("ERROR connecting");

    inet_ntop(AF_INET, &(serv_addr.sin_addr), clientAddr, CLADDR_LEN); 

    n = write(sockfd,buffer,strlen(buffer)); //send server users name to client
    if (n < 0) 
      error("ERROR writing to socket");

    n = read(sockfd,name,255); //read clients name 
    if (n < 0) 
         error("ERROR reading from socket");

    name[strlen(name) - 1] = '\0'; //remove \n character
    printf("Connection accepted from %s...\n You are talking to %s\n", clientAddr, name); 

   
  pid = fork(); //start a new process that listens and processes messages from the client 
  if (pid < 0) //incase fork fails
  {
    perror("Fork failed.\n");
    return 1;  // nonzero means a failure result in unix
  }

  if(pid == 0)
  {
    readData(sockfd);
    exit(1);
  }
  else
  {
    pid1 = fork(); //start a new process that takes user input and sends it to the client 
    if (pid1 < 0) 
    {
      perror("Fork failed.\n");
      return 1;  // nonzero means a failure result in unix
    }

    if(pid1 == 0)
    {
      writeData(sockfd);
      exit(1);
    }
  }

  while(!quitMessageRecieved);//Parent waits until quit is recieved
  kill(pid, SIGTERM);//makes sure child process is killed. This removes any zombies
  kill(pid1, SIGTERM);
  printf("\nConnection closed to %s...\n", clientAddr);  
  close(sockfd);//close the connection
  return 0;
}

void readData(int socket) //keep reading data until a quit is recieved 
{
  int n;
  char buffer[256];

  while(!quitMessageRecieved)
  {
    bzero(buffer,256);
    n = read(socket,buffer,255);
    if (n < 0) 
      error("ERROR reading from socket");

    if(n > 0)
    {  
      printf("\n<%s> %s<you> ", name, buffer); 
      fflush( stdout ); 
    } 

    if((buffer[0]=='q') && (buffer[1]=='u') && (buffer[2]=='i') && (buffer[3]=='t'))//if quit is recieved exit program and disconnect users
    {
      quitMessageRecieved = true;
      break;
    }
  }
}

void writeData(int socket)//keep sending data until a quit is sent 
{
  int n;
  char buffer[256];

  while(!quitMessageRecieved)
  {
    printf("<you> ");

    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(socket,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");

    if((buffer[0]=='q') && (buffer[1]=='u') && (buffer[2]=='i') && (buffer[3]=='t')) //if quit is recieved exit program and disconnect users
    {
      quitMessageRecieved = true;
      break;
    }
  }
}
