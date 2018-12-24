#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void doprocessing (int sock)
{
  int n;
  char buffer[256] ;
  int done = 0;
  double num1, num2, result = 0 ;
  char operation ;
  bzero(buffer,256);

  while (!done)
  {
      n = read(sock,buffer,255);
      buffer[n] = 0 ;

      if (strcmp(buffer,"quit") == 0)
      {
        done = 1;
        printf("Client closed connection..\n");
      }
      else
      {
        sscanf( buffer, "%lf %c %lf", &num1, &operation, &num2 ) ;
        if( operation == '+' )
          result = num1 + num2 ;
        else if( operation == '-' )
          result = num1 - num2 ;
        else if( operation == '*' )
          result = num1 * num2 ;
        else if( operation == '/' )
          result = num1 / num2 ;
        else
        {
          write( sock, "Wrong operation", 255 ) ;
          continue ;
        }
        sprintf( buffer, "%.2f %c %.2f = %.2f", num1, operation, num2, result ) ;
        write( sock, buffer, 255 ) ;
      }
  }
  close(sock);
}

int main( int argc, char *argv[] )
{
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int  n;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      perror("ERROR opening socket");
      exit(1);
    }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 5001;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
      perror("ERROR on binding");
      exit(1);
    }
  printf("Server started.. waiting for clients ...\n");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while (1)
  {
      newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
      printf("Client connected...\n");
      if (newsockfd < 0)
        {
          perror("ERROR on accept");
          exit(1);
        }
      int pid = fork();
      if (pid < 0)
        {
          perror("ERROR on fork");
          exit(1);
        }
      if (pid == 0)
        {
          close(sockfd);
          doprocessing(newsockfd);
          exit(0);
        }
      else
        {
          close(newsockfd);
        }
    }
}
