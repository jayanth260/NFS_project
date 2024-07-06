#ifndef CREATE_DELETE_H_
#define CREATE_DELETE_H_

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

typedef struct create_delete_details{
    char op[8];
    char type[8];
    char file_path[256];
    int add;
}create_delete_details;

typedef struct copy_details{
    char type[8];
    char filepath[256];
    char S_D;
}copy_details;

int Request_Create_delete(char buffer[], int sock, int index, int add);
int Request_copy(char buffer[], int source_index, int dest_index);
int remove_path(int index, char path[]);


#endif

