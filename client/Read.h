#ifndef READ_H_
#define READ_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>



// typedef struct SServer_addr{
//     int port;
//     char SS_ip[16];
// }SServer_addr;

typedef struct get_info{
    char perm[20];
    int size;
    char time[20];
}get_info;

int Request_Read(char file[],int sock);
int Request_Write(char file[],int sock);
int Request_Create_delete(char file[],int sock);



#endif

