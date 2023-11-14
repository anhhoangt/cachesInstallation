#include "randomCache.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Initialize a random cache
void random_cache_initialize(randomCache* cache) {
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        cache->random_cache[i] = NULL;
        cache->hit_count = 0;
        cache->miss_count = 0;
    }
    srand((unsigned int)time(NULL)); // Seed the random number generator
}

// Insert an item into the cache, returns true if successful and false otherwise
void random_cache_put(randomCache* cache, Message* message) {
    int index = rand() % MAX_CACHE_SIZE; // Random index for replacement

    // Place the new message at this index
    cache->random_cache[index] = message;
}

// Get an item from the cache, returns NULL if not found
Message* random_cache_get(randomCache* cache, const char* id) {
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        if (cache->random_cache[i] && strcmp(cache->random_cache[i]->id, id) == 0) {
            cache->hit_count++; // Increment hit counter when a message is found
            return cache->random_cache[i]; // Cache hit
        }
    }
    cache->miss_count++; // Increment miss counter when a message is not found
    return NULL; // Cache miss
}

// Free all resources used by the random cache
void random_cache_free(randomCache* cache) {
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        if (cache->random_cache[i]) {
            free_msg(cache->random_cache[i]);
            cache->random_cache[i] = NULL;
        }
    }
}
