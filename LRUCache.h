#ifndef LRUCACHE_H
#define LRUCACHE_H

#include "message.h"

#define MAX_CACHE_SIZE 16 // Maximum number of messages in cache

// I implement my LRU cache as a hash table combining with doubly linked list. In double linked list, each node 
//will contain a Message object and 2 pointers: prev and next which pointing to the previous node and the next  
//node in double linked list. I also have 2 dummy nodes called head and tail which are the head and the tail of 
//double linked list. These two nodes will help adding node to the head (most usage) or removing node from tail 
//(least usage). In hash table, I use the message ID as the key and its DLL node as the value.

// The reason why I use the combination of hash map and double linked list is that using hash map will help 
//inserting new node to cache and looking up node in cache in constant time O(1) which is fast. However, in term
//of keep tracking and maintaining the order of data, hash map are unordered data structure which doesn't
//faciliate to do so. Therefore, using double linked list will help updating the position of items in the cache
// in constant time O(1).

//Alternative designs for LRU cache that I did not consider:
//An array or list: 
//The problem with array is that it is not efficient to insert new node to the cache. The reason is that when
//inserting new node to the cache, we need to shift all the nodes after the new node to the right by 1 position
//which takes O(n) time. The problem with list is that it is not efficient to look up node in the cache. The
//reason is that we need to traverse the list from the head to the tail to find the node which takes O(n) time.

//Ordered Dictionary:
//This is a language-specific feature, and implementing an ordered dictionary from scratch would effectively lead 
//back to the hash map and linked list design.

//Self-balanced binary search tree
//The problem with self-balanced binary search tree is that it is not efficient to insert new node to the cache.
//The reason is that when inserting new node to the cache, we need to traverse the tree from the root to the leaf
//to find the position to insert the new node which takes O(logn) time. 

//Single linked list with a hash map
//The problem with single linked list is that it is not efficient to remove node from the tail of the list. The
//reason is that we need to traverse the list from the head to the tail to find the node before the tail which
//takes O(n) time.

//two queues or stacks
//The problem with two queues or stacks is that it is not efficient to remove node from the tail of the queue or
//stack. The reason is that we need to pop all the nodes from the queue or stack to find the node before the tail
//which takes O(n) time.

typedef struct CacheNode {
    Message* message;      // Pointer to the message data.
    struct CacheNode* prev; // Previous node in the doubly linked list.
    struct CacheNode* next; // Next node in the doubly linked list.
} CacheNode;

typedef struct {
    CacheNode* nodes[MAX_CACHE_SIZE]; // Hash table for quick access to nodes.
    CacheNode* head;                  // Head of the doubly linked list for LRU.
    CacheNode* tail;                  // Tail of the doubly linked list for MRU.
    int current_size;                 // Current size of the cache.
    unsigned long hit_count;
    unsigned long miss_count;
} LRUCache;

// Initialize a least recently used cache
void LRUCache_initialize(LRUCache* cache);

// Free all resources used by the cache
void LRUCache_free(LRUCache* cache);

// Insert an item into the cache, returns true if successful and false otherwise
CacheNode* LRUCache_put(LRUCache* cache, Message* message);

// Get an item from the cache if it exists
Message* LRUCache_get(LRUCache* cache, const char* id);

#endif /* LRUCACHE_H */
