#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>



typedef struct SServer_details{
    char SS_ip[16];
    int SS_Cport;
    char accessible_paths[1024];
    // int no_of_paths;
}SServer_details;

#endif

