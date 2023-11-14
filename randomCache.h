#ifndef RANDOMCACHE_H
#define RANDOMCACHE_H

#include "message.h"

#define MAX_CACHE_SIZE 16 // Maximum number of messages in cache

//I implement my random cache simply using an array because I can add/remove items from anywhere in they array.
//For a random cache, where the order of elements doesn't matter, an array provides an easy way to store items.
//Arrays provide O(1) time complexity for access to any element by index, which is useful for randomly choosing 
//an element to replace. When you need to replace an item randomly, you can generate a random index and directly 
//access and overwrite the element in the array without any additional overhead. Arrays use a contiguous block of 
//memory, which can be beneficial for performance due to spatial locality and fewer cache misses.

//Alternative designs for random cache that i did not consider:
//Linked Lists (Single or Double): 
//While they are good for caches where order or quick removal/insertion is important, they don't provide the
//same O(1) access time as arrays. Choosing a random element would require traversing the list, which is O(n) 
//on average.

//Hash Map:
//Hash maps are good for quick access to elements, but they don't provide the same O(1) access time as arrays.
//Choosing a random element would require traversing the hash map, which is O(n) on average.

//Self-balanced binary search tree:
//While they are good for caches where order or quick removal/insertion is important, they don't provide the
//same O(1) access time as arrays. Choosing a random element would require traversing the tree, which is O(logn)
//on average.


typedef struct {
    Message* random_cache[MAX_CACHE_SIZE]; // Array with fixed size defined by global variable
    unsigned long hit_count;
    unsigned long miss_count;
} randomCache;

// Initialize the random cache 
void random_cache_initialize(randomCache* cache);

// Put a message into the cache with random replacement policy
void random_cache_put(randomCache* cache, Message* message);

// Get a message from the cache if it exists
Message* random_cache_get(randomCache* cache, const char* id);

// Free all resources used by the random cache
void random_cache_free(randomCache* cache);

#endif // RANDOMCACHE_H
