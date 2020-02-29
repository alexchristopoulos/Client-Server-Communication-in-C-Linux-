
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "keyvalue.h"
#include <stddef.h>
#include <sys/wait.h>
#define bufsize 200
#define keyvaluesize 3000
int sockfd, newsockfd, port;

socklen_t clientlen;
void sig_chld()
{
    while (waitpid(0, NULL, WNOHANG) > 0)
    {
    }
    signal(SIGCHLD, sig_chld);
}
struct sockaddr_in serv_addr, client_addr;
int charTocode(char c);
char **keys;
char **values;
int indx;

int main(int argc, char *argv[])
{
    pid_t pid, wpid;
    int status;

    indx = 0;
    keys = malloc(keyvaluesize);
    values = malloc(keyvaluesize);
    if (argc < 2)
    {
        return 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    port = atoi(argv[1]);
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);
    signal(SIGCHLD, sig_chld);
    while (1)
    {

        clientlen = sizeof(struct sockaddr_in);
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen);
        pid = fork();
        if (pid == 0)
        {
            //close(sockfd);
            char cmd[bufsize];
            bzero(cmd, bufsize);
            int bts_num;
            //connection handler
            int k;
            int terminate = 0;
            while (1) //read per bufsize bytes
            {
                bts_num = read(newsockfd, cmd, bufsize);
                if (bts_num <= 0)
                {
                    break;
                }
                k = 0;
                while (bts_num > 0)
                {
                    int code = charTocode(cmd[k]);
                    k++;
                    bts_num--;
                    switch (code)
                    {
                    case 103:
                    {
                        char *arg = malloc(51);
                        bzero(arg, 51);
                        int j = 0;
                        while (cmd[k] != '\0')
                        {
                            arg[j] = cmd[k];
                            j++;
                            k++;
                            bts_num--;
                        }
                        char x[51];
                        bzero(x, 51);
                        strcpy(x, get(arg));
                        if (strcmp(x, "110") == 0)
                        {
                            write(newsockfd, "n", 1);
                        }
                        else
                        {
                            char result[1024];
                            bzero(result, 1024);
                            result[0] = 'f';
                            int w;
                            for (w = 1; w <= strlen(x); w++)
                            {
                                result[w] = x[w - 1];
                            }
                            result[w + 1] = '\0';
                            write(newsockfd, result, strlen(result) + 1);
                        }
                        k++;
                        bts_num--;
                        break;
                    }
                    case 112:
                    {
                        char *x = malloc(1024);
                        char *y = malloc(1024);
                        bzero(x, 1024);
                        bzero(y, 1024);
                        int j = 0;
                        while (cmd[k] != '\0' && k < bufsize)
                        {
                            x[j] = cmd[k];
                            j++;
                            k++;
                            bts_num--;
                        }
                        k++;
                        bts_num--;
                        j = 0;
                        while (cmd[k] != '\0' && k < bufsize)
                        {
                            y[j] = cmd[k];
                            j++;
                            k++;
                            bts_num--;
                        }
                        k++;
                        bts_num--; //skip next \0
                        put(x, y);
                        break;
                    }
                    case 100:
                    {
                        close(newsockfd);
                        terminate = 1;
                        return 0;
                        break;
                    }
                    } //end switch
                    if (terminate == 1)
                    {
                        break;
                    }
                } //end bts_num==0 while

                if (terminate == 1)
                {
                    break; /* wrong command >> continue to next client*/
                }
            } //end while(1) for read()
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
int charTocode(char c)
{
    if (c == 'p')
    {
        return 112;
    }
    else if (c == 'g')
    {
        return 103;
    }
    else
    {
        return 100;
    }
}

char *get(char *key)
{
    int found;
    int pos;
    found = 0;
    for (int i = 0; i < indx; i++)
    {
        if (strcmp(key, keys[i]) == 0)
        {
            return values[i];
        }
    }
    return "110";
}

void put(char *key, char *value)
{
    int found = 0;
    if (indx < keyvaluesize)
    {
        for (unsigned int i = 0; i < indx; i++)
        {
            if (strcmp(keys[i], key) == 0)
            {
                found = 1;
                size_t vlen = strlen(value + 1);
                values[i] = malloc(vlen);
                strcpy(values[i], value);
            }
        }
        if (found == 0)
        {
            size_t klen = strlen(key + 1);
            keys[indx] = malloc(klen);
            strcpy(keys[indx], key);
            size_t vlen = strlen(value + 1);
            values[indx] = malloc(vlen);
            strcpy(values[indx], value);
            indx++;
        }
    }
}
