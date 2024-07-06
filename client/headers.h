#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#include "Read.h"


typedef struct SServer_addr{
    int port;
    char SS_ip[16];
}SServer_addr;



typedef struct Node {
    char* key;
    int port;
    char ip[16];
    struct Node* prev;
    struct Node* next;
} Node;

// LRU Cache structure
typedef struct LRUCache {
    int capacity;
    int size;
    Node* head;
    Node* tail;
    Node** cacheMap;
} LRUCache;

void die(const char *message);
int connect_S(char SS_ip[],int SS_port );
int init(SServer_addr address,char buffer[]);

LRUCache* initCache(int capacity);
SServer_addr Get(LRUCache* cache, const char* key);
void Put(LRUCache* cache, const char* key, int port,char ip[]);

#endif