#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
typedef struct SS_recv
{
    char SS_ip[16];
    int SS_Cport;
    char accessible_paths[1024];
    // int no_of_paths;
} SS_recv;

typedef struct acc_path{
    char path[128];
    int SS_num;

}acc_path;

typedef struct SServer_details
{
    char SS_ip[16];
    int SS_Cport;
    acc_path* paths;
    int no_of_paths;
    int active;
    int S1_idx;
    int S1_set;
    int S2_set;
    int S2_idx;
    
} SServer_details;

typedef struct Server_set
{
    SServer_details *SS;
    int no_of_SS;
    int size;
} Server_set;

typedef struct SServer_addr
{
    int port;
    char SS_ip[16];
    int index;
} SServer_addr;

<<<<<<< HEAD
typedef struct Client_details
{
    int port;
    char C_ip[16];
} Client_details;

int book_conn(int code, int id);
int book_cmd(char cmd[], int C_id, int SS_id, int success);
int check_connection(int C_conn,int SS_id);
int first_time(char ip[16],int S_port);
SServer_addr get_Sserver(char file_path[],char op[]);

int duplicate_S(int index,int now);
int update_R(int RS1_index,int RS2_index,int S_index);
=======
// LRU caching +++++++++++++++++++++++++++
struct LRU_Caching{
  int SS_No;
  char path[1024];
  int flag;
  int Client_no;
  char operation[1024];
};
int book();
>>>>>>> origin/main

#endif
