#ifndef MESSAGE_H
#define MESSAGE_H
#define ID_SIZE 20

#include <stdlib.h>

typedef struct Message {
    char id[ID_SIZE]; 
    char* time_sent;
    char* sender;
    char* receiver;
    char* content;
    int delivered;

    struct Message* prev; // Previous message in LRU cache
    struct Message* next; // Next message in LRU cache
} Message;

#include "LRUCache.h"
#include "randomCache.h"

Message* create_msg(const char* sender, const char* receiver, const char* content);
int store_msg(Message* msg);
Message* retrieve_msg(const char* id);
void free_msg(Message* msg);
void clear_message_store();

#endif
