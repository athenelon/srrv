#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main( int argc, char *argv[ ])
{
  int sockfd = 0 ;
  struct sockaddr_in serv_addr ;
  int done ;
  char * line = NULL ;
  size_t len = 0 ;
  int nread ;
  if( argc != 2 )
  {
      printf("\n Usage: %s <ip of server> \n",argv[0]);
      return 1;
  }
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5001);
  if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
      printf("\n inet_pton error occured\n");
      return 1;
    }
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      printf("\n Error : Connect Failed \n");
      return 1;
    }
  printf("Connected to server... Send message to server, or type 'quit' to exit\n");
  done = 0;
  while (!done)
  {
    nread=getline(&line, &len, stdin);
    nread-- ;
    line[nread] ='\0';
    if (strcmp(line,"quit") == 0)
    {
      done = 1;
      printf("Closing connection with the server..\n");
    }
    write (sockfd, line, strlen(line));
    read( sockfd, line, 255 ) ;
    printf( ">> %s\n", line ) ;
  }
  close(sockfd);
  return 0;
}
