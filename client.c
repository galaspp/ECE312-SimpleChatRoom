// /* A simple server in the internet domain using TCP
//    The port number is passed as an argument 
//    This version runs forever, forking off a separate 
//    process for each connection
//    gcc server2.c 
// */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h> 
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h> 

// char* hostname = "192.168.2.244"; 

// void dostuff(int); /* function prototype */
// void error(char *msg)
// {
//     perror(msg);
//     exit(1);
// }

// int main(int argc, char *argv[])
// {
//      int sockfd, newsockfd, portno, clilen, pid;
//      struct sockaddr_in serv_addr, cli_addr;
//      struct hostent *server;

//      if (argc < 2) {
//          fprintf(stderr,"ERROR, no port provided\n");
//          exit(1);
//      }
//      sockfd = socket(AF_INET, SOCK_STREAM, 0);
//      //setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&serv_addr,sizeof(serv_addr));
//      if (sockfd < 0) 
//         error("ERROR opening socket");

//     server = gethostbyname(hostname);
//     if (server == NULL) {
//         fprintf(stderr,"ERROR, no such host\n");
//         exit(0);
//     }

//      memset(&serv_addr, 0, sizeof(serv_addr));
//      portno = 4567;//atoi(argv[1]);
//      serv_addr.sin_family = AF_INET;
//      serv_addr.sin_port = htons(portno);
//      //serv_addr.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY;//inet_addr("192.168.2.244");
//      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
//      printf("yeet");
//      if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
//               error("ERROR on binding");
//      //int yeet = listen(sockfd,5);
//      //printf("%d", yeet);
//      clilen = sizeof(cli_addr);

//      while (1) {
//         newsockfd = sockfd;
//          //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
//          if (newsockfd < 0) 
//              error("ERROR on accept");
//          pid = fork();
//          if (pid < 0)
//              error("ERROR on fork");
//          if (pid == 0)  {
//             // close(sockfd);
//              dostuff(newsockfd);
//              exit(0);
//          }
//          else close(newsockfd);
//      } /* end of while */
//      return 0; /* we never get here */
// }

// /******** DOSTUFF() *********************
//  There is a separate instance of this function 
//  for each connection.  It handles all communication
//  once a connnection has been established.
//  *****************************************/
// void dostuff (int sock)
// {
//    int n;
//    char buffer[256];
      
//    memset(buffer, 0, 256);
//    n = read(sock,buffer,255);
//    if (n < 0) error("ERROR reading from socket");
//    printf("Here is the message: %s\n",buffer);
//    n = write(sock,"I got your message",18);
//    if (n < 0) error("ERROR writing to socket");
// }
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void writeData(int socket);
void readData(int socket);

#define CLADDR_LEN 100   
#define PORT 4567
char *hostname = "192.168.2.244";
char name[256];
int status;
bool quitMessageRecieved = false;

void error(char *msg)
{
    perror(msg);
    exit(0);
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

    printf("Please enter the host (default or new): ");
    fgets(buffer,255,stdin);
    if((buffer[0] == 'd') || (buffer[0] == 'D'))
    {
      portno = PORT;
      server = gethostbyname(hostname);
      printf("Please enter your name: ");
      bzero(buffer,256);
      fgets(buffer,255,stdin);
    }
    else if((buffer[0] == 'n') || buffer[0] == 'N')
    {
      printf("Please enter the host port: ");
      bzero(buffer,256);
      fgets(buffer,255,stdin);
      portno = atoi(buffer);
      printf("Please enter the host ip: ");
      bzero(buffer,256);
      fgets(buffer,255,stdin);
      server = gethostbyname(buffer);
    }
    else 
    {
      printf("Invalid Option ");
      return 0;
    }

    // if (argc < 3) {
    //    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    //    exit(0);
    // }
    // portno = atoi(argv[2]);
    //portno = PORT;//atoi(PORT);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    // server = gethostbyname(argv[1]);
    //server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    inet_ntop(AF_INET, &(serv_addr.sin_addr), clientAddr, CLADDR_LEN);
    printf("Connection accepted from %s...\n", clientAddr);  

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    n = read(sockfd,name,strlen(name));
    if (n < 0) 
         error("ERROR reading from socket");

    // bzero(buffer,256);
    // fgets(buffer,255,stdin);
    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // bzero(buffer,256);
    // while((n = read(sockfd,buffer,255)) <= 0)
    //   printf("yeet");
    // if (n < 0) 
    //      error("ERROR reading from socket");
    // printf("%s\n",buffer);
   int pid = fork();
   if (pid < 0) 
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
     writeData(sockfd);
   }
     wait(&status);
    close(sockfd);
    return 0;
}

/*
 Ideas right now:
 fork reading to child, once a quit signal is recieved set a flag and do a graceful exit and disconnect. 
 if a quit is being set a flag for child to exit.
 I do not think zombies matter because I will not have more then 1 child at a time.
*/
void readData(int socket) 
{
  int n;
  char buffer[256];
  while(!quitMessageRecieved)
  {
    bzero(buffer,256);
    n = read(socket,buffer,255);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("<%s> %s\n", name, buffer);
    if((buffer[0]=='q') && (buffer[1]=='u') && (buffer[2]=='i') && (buffer[3]=='z') && (buffer[4]=='\n'))
    {
      quitMessageRecieved = true;
      break;
    }
  }
}

void writeData(int socket)
{
  int n;
  char buffer[256];
  while(!quitMessageRecieved)
  {
    printf("<you> ");
    bzero(buffer,256);
    n = write(socket,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
    printf("%s\n", buffer);
    if((buffer[0]=='q') && (buffer[1]=='u') && (buffer[2]=='i') && (buffer[3]=='z') && (buffer[4]=='\n'))
    {
      quitMessageRecieved = true;
      break;
    }
  }
}