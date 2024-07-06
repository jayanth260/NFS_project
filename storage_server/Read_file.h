#ifndef Read_file_H_
#define Read_file_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct write_path{
    char path[128];
    int write_access;
}write_path;

typedef struct set_writepaths{
int no_of_writepaths;
int size;
write_path *WP;
}set_writepaths;

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

typedef struct get_info{
    char perm[20];
    int size;
    char time[20];
}get_info;

void* Read(void* arg);
void* Write(void* arg);
int create_delete(int C_conn);
int add_path(char path[]);
int remove_path(char path[]);

void* getinfo(void* arg);


int copy_init(int C_conn);

#endif