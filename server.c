// // gcc tcpserver.c -o server -pthread
// // ./server

//   #include <stdio.h>
//   #include <stdlib.h>
//   #include <string.h>
//   #include <sys/types.h> 
//   #include <sys/socket.h>
//   #include <netinet/in.h>
//   #include <pthread.h>

//   #define PORT 8080//4567
//   #define BUF_SIZE 256
//   #define CLADDR_LEN 100

//     void error(char *msg)
//     {
//         perror(msg);
//         exit(1);
//     }

//     void * receiveMessage(void * socket) {
//         int sockfd, ret;
//         char buffer[BUF_SIZE];
//         sockfd = (int) socket;

//         memset(buffer, 0, BUF_SIZE);
//         if (write(sockfd,"I'm waiting for message",23) < 0)
//             error("ERROR writing to socket");

//         while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
//             printf("client: %s", buffer);
//         }
//         if (ret < 0) 
//             printf("Error receiving data!\n");
//         else
//             printf("Closing connection\n");
//         close(sockfd);
//     }


//     int main(int argc, char *argv[])
//     {
//         int sockfd, newsockfd, portno, clilen;
//         char buffer[BUF_SIZE];
//         struct sockaddr_in serv_addr, cli_addr;
//         pid_t childpid;
//         char clientAddr[CLADDR_LEN];
//         pthread_t rThread;
//         int ret, n;

//         sockfd = socket(AF_INET, SOCK_STREAM, 0);
//         if (sockfd < 0) 
//            error("ERROR opening socket");
//         printf("Socket created...\n");

//         memset(&serv_addr, 0, sizeof(serv_addr));
//         serv_addr.sin_family = AF_INET;
//         serv_addr.sin_port = htons(PORT);
//         serv_addr.sin_addr.s_addr = INADDR_ANY;

//         if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
//             error("ERROR on binding");
//         printf("Binding done...\n");

//         printf("Waiting for a connection...\n");
//         listen(sockfd,5);
//         clilen = sizeof(cli_addr);
//         while (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) {
//             inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
//             printf("Connection accepted from %s...\n", clientAddr);

//             //creating a new thread for receiving messages from the client
//             if (ret = pthread_create(&rThread, NULL, receiveMessage, (void *) newsockfd)) {
//                 printf("ERROR: Return Code from pthread_create() is %d\n", ret);
//                 error("ERROR creating thread");
//             }
//         }

//         if (newsockfd < 0) 
//             error("ERROR on accept");
// //        close(newsockfd);
//         close(sockfd);

//         pthread_exit(NULL);
//         return 0; 
//     }

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define CLADDR_LEN 100  
#define PORT 4567
//char *hostname = "192.168.2.244";
char name[256];
int status;
bool quitMessageRecieved = false;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char clientAddr[CLADDR_LEN];
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

    printf("Please enter the host (default or new): ");
    fgets(buffer,255,stdin);
    if((buffer[0] == 'd') || (buffer[0] == 'D'))
    {
      portno = PORT;
     // server = gethostbyname(hostname);
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
      // printf("Please enter the host ip: ");
      // bzero(buffer,256);
      // fgets(buffer,255,stdin);
      // server = gethostbyname(buffer);
    }
    else 
    {
      printf("Invalid Option ");
      return 0;
    }
     // if (argc < 2) {
     //     fprintf(stderr,"ERROR, no port provided\n");
     //     exit(1);
     // }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     //bzero(buffer,256);

     inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
     printf("Connection accepted from %s...\n", clientAddr);  

    n = write(newsockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    n = read(newsockfd,name,strlen(name));
    if (n < 0) 
         error("ERROR reading from socket");

     // n = read(newsockfd,buffer,255);
     // if (n < 0) error("ERROR reading from socket");
     // printf("Here is the message: %s\n",buffer);
     // n = write(newsockfd,"I got your message",18);
     // if (n < 0) error("ERROR writing to socket");
    int pid = fork();
   if (pid < 0) 
   {
      perror("Fork failed.\n");
      return 1;  // nonzero means a failure result in unix
   }

   if(pid == 0)
   {
     readData(newsockfd);
     exit(1);
   }
   else
   {
     writeData(newsockfd);
   }
     wait(&status);
     close(newsockfd);
    close(sockfd);
     return 0; 
}

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
    if(n > 0) printf("<%s> %s\n", name, buffer);
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
    if(n > 0) printf("<you> ");
    bzero(buffer,256);
    n = write(socket,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
    if(n > 0) printf("%s\n", buffer);
    if((buffer[0]=='q') && (buffer[1]=='u') && (buffer[2]=='i') && (buffer[3]=='z') && (buffer[4]=='\n'))
    {
      quitMessageRecieved = true;
      break;
    }
  }
}