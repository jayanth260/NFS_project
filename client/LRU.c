#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "headers.h"

#define CACHE_SIZE 10


// Node structure for doubly linked list


// Function to create a new node
Node* createNode(const char* key, int port,char ip[]) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->port =port;
    strcpy(newNode->ip,ip);
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize the LRU cache
LRUCache* initCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;

    // Create an array of pointers to nodes for quick access
    cache->cacheMap = (Node**)malloc(sizeof(Node*) * capacity);
    for (int i = 0; i < capacity; i++) {
        cache->cacheMap[i] = NULL;
    }

    return cache;
}

// Function to remove the least recently used node
void removeLRU(LRUCache* cache) {
    Node* lruNode = cache->tail;

    if (lruNode->prev) {
        lruNode->prev->next = NULL;
        cache->tail = lruNode->prev;
    } else {
        cache->head = NULL;
        cache->tail = NULL;
    }

    // Remove the node from the cache map
    int index = hashString(lruNode->key) % cache->capacity;
    cache->cacheMap[index] = NULL;

    freeNode(lruNode);
    cache->size--;
}

// Function to move a node to the front of the cache
void moveToFront(LRUCache* cache, Node* node) {
    // Remove the node from its current position
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        cache->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        cache->tail = node->prev;
    }

    // Move the node to the front
    node->next = cache->head;
    node->prev = NULL;

    if (cache->head) {
        cache->head->prev = node;
    }

    cache->head = node;

    // Update the tail if needed
    if (!cache->tail) {
        cache->tail = node;
    }
}

// Function to hash a string
unsigned int hashString(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        key++;
    }
    return hash;
}

// Function to get a value from the cache
SServer_addr Get(LRUCache* cache, const char* key) {
    // Check if the key is in the cache
    int index = hashString(key) % cache->capacity;
    if (cache->cacheMap[index]) {
        Node* node = cache->cacheMap[index];

        // Move the accessed node to the front
        moveToFront(cache, node);

        // Return the value
        SServer_addr S;
        S.port=node->port;
        strcpy(S.SS_ip,node->ip);
        return S;
    } else {
        // Key not found
        
        SServer_addr S;
        S.port=-1;
        strcpy(S.SS_ip,"nope");
        return S;
    }
}

// Function to put a key-value pair into the cache
void Put(LRUCache* cache, const char* key, int port,char ip[]) {
    // Check if the key is already in the cache
    int index = hashString(key) % cache->capacity;
    if (cache->cacheMap[index]) {
        Node* node = cache->cacheMap[index];

        // Update the value
        // node->value = value;
        node->port=port;
strcpy(node->ip,ip);
        // Move the accessed node to the front
        moveToFront(cache, node);
    } else {
        // Create a new node
        Node* newNode = createNode(key,port,ip);

        // Check if the cache is at capacity
        if (cache->size >= cache->capacity) {
            // Remove the least recently used node
            removeLRU(cache);
        }

        // Insert the new node at the front
        newNode->next = cache->head;

        if (cache->head) {
            cache->head->prev = newNode;
        }

        cache->head = newNode;

        // Update the tail if needed
        if (!cache->tail) {
            cache->tail = newNode;
        }

        // Add the new node to the cache map
        cache->cacheMap[index] = newNode;

        // Update the size
        cache->size++;
    }
}

// Function to print the contents of the cache
// void printCache(LRUCache* cache) {
//     Node* current = cache->head;

//     while (current) {
//         printf("(%s, %d) -> ", current->key, current->value);
//         current = current->next;
//     }

//     printf("NULL\n");
// }

// Function to free the memory used by a node
void freeNode(Node* node) {
    free(node->key);
    free(node);
}

// Function to free the memory used by the cache
void freeCache(LRUCache* cache) {
    Node* current = cache->head;
    Node* next;

    while (current) {
        next = current->next;
        freeNode(current);
        current = next;
    }

    free(cache->cacheMap);
    free(cache);
}

// int main() {
//     LRUCache* cache = initCache(CACHE_SIZE);

//     put(cache, "one", 10);
//     put(cache, "two", 20);
//     put(cache, "three", 30);

//     printf("Initial cache state:\n");
//     printCache(cache);

//     printf("\nValue for key 'two': %d\n", get(cache, "two"));

//     put(cache, "four", 40);

//     printf("\nCache state after adding ('four', 40):\n");
//     printCache(cache);

//     freeCache(cache);

//     return 0;
// }
