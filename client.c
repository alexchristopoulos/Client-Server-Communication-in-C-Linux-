#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#define size 52
int ralloccnt = 2;
int command_code(char *x);
int DomainToIP(char *host, char *ip);         //function that converts argv[1] to ip
int com_protocol(char *cmd[], int arg_count); //protocol function for communication
int Protocol_Conversion(char *x);
int argi = 1;
int read_counts = 0;
int param_num(int c);
int sockfd;
struct sockaddr_in server_addr;
struct hostent *hs;
int main(int argc, char **argv)
{
  int bufsize = size * (argc - 3);
  if (argc <= 2)
  {
    return 0;
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  char *host = argv[argi];
  argi++;
  char *ip = malloc(32 * sizeof(char *));
  int status = DomainToIP(host, ip);
  if (status == 1)
  {
    printf("host %s does not exists!\n", host);
    return 0;
  }
  int port = atoi(argv[2]);
  argi++;
  hs = gethostbyname(host);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr = *((struct in_addr *)hs->h_addr);
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
  {
    printf("connect error\n");
  }
  //communication

  char *command;
  command = malloc(bufsize);
  bzero(command, bufsize);
  int k = 0;
  int used = size;
  for (int i = argi; i < argc; i++)
  {
    int code = command_code(argv[i]);
    switch (code)
    {
    case 103:
    {
      int parameters = 1;
      command[k] = 'g';
      k++;
      i++;
      for (int j = 0; j < strlen(argv[i]); j++)
      {
        command[k] = argv[i][j];
        k++;
      }
      command[k] = '\0';
      k++;
      read_counts++;
      break;
    }
    case 112:
    {
      int parameters = 2;
      command[k] = 'p';
      k = k + 1;
      do
      {
        i++;
        for (int j = 0; j < strlen(argv[i]); j++)
        {
          command[k] = argv[i][j];
          k++;
        }
        parameters--;
        command[k] = '\0';
        k++;
      } while (parameters > 0);
      break;
    }
    case 100:
    {
      command[k] = '@';
      command[k + 1] = '\0';
      k = k + 2;
      break;
    }
    }
  }
  write(sockfd, command, bufsize);
  char *buf = malloc(read_counts * 52);
  bzero(buf, read_counts * 52);
  read(sockfd, buf, read_counts * 52);
  int bufi = 0;
  while (read_counts > 0)
  {
    if (buf[bufi] == 'f')
    {
      bufi++;
      while (buf[bufi] != '\0')
      {
        printf("%c", buf[bufi]);
        bufi++;
      }
      printf("\n");
      bufi++;
    }
    else
    {
      printf("\n");
      bufi = bufi + 1;
    }
    read_counts--;
  }
  close(sockfd);
  return 0;
}
//end main

//convert Domain name to IP Adress Start***
int DomainToIP(char *host, char *ip)
{
  struct hostent *h;
  struct in_addr **addr_list;
  if ((h = gethostbyname(host)) == NULL)
  {
    return 1;
  }
  addr_list = (struct in_addr **)h->h_addr_list;
  for (int i = 0; addr_list[i] != NULL; i++)
  {
    strcpy(ip, inet_ntoa(*addr_list[i]));
  }
  return 0;
}
//convert Domain name to IP Adress End***
int command_code(char *x)
{
  if (strcmp(x, "get") == 0)
  {
    return 103;
  }
  else if (strcmp(x, "put") == 0)
  {
    return 112;
  }
  else
  {
    return 100;
  }
}
