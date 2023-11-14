#include "message.h"
#include "LRUCache.h"
#include "randomCache.h"
#include "genRand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//total messages to test each cache
#define TOTAL_MESSAGES 1000
//the length of a message
#define MESSAGE_LENGTH 20
//the max length of an id
#define MAX_ID_LENGTH 10

//helper function to generate random word with provided length
char* generate_random_word(int length) {
    char* word = (char*) malloc(length + 1); // +1 for the null terminator
    if (word == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.?!";
    for (int i = 0; i < length; i++) {
        word[i] = charset[genRand(0, strlen(charset) - 1)];
    }
    word[length] = '\0'; // Null-terminate the string
    return word;
}

// Function to print LRU cache content for debug purposes
void print_LRUCache_content(LRUCache* cache) {
    printf("Cache Content:\n");
    CacheNode* current = cache->head;
    while (current) {
        printf("ID: %s, Content: %s\n", current->message->id, current->message->content);
        current = current->next;
    }
}

// Function to print random cache content for debug purposes
void print_random_cache_content(randomCache* cache) {
    if (cache == NULL) {
        printf("Cache is NULL.\n");
        return;
    }

    printf("Random Cache Content:\n");
    // Iterate over the array of message pointers
    for (int i = 0; i < MAX_CACHE_SIZE; i++) {
        Message* message = cache->random_cache[i];
        if (message != NULL) { // Check if the message pointer is not NULL
            printf("ID: %s, Content: %s\n", message->id, message->content);
        }
    }
}

// Test function for the LRU cache
void test_lru_cache() {
    printf("Testing LRU Cache...\n");

    // Assume MAX_CACHE_SIZE is a small integer like 16 for easy testing
    const int testCacheSize = MAX_CACHE_SIZE;
    LRUCache cache;
    LRUCache_initialize(&cache);

    // Fill the cache to its capacity
    for (int i = 0; i < testCacheSize; ++i) {
        char id[10];
        sprintf(id, "MSG-%d", i); // Create a unique ID for each message
        Message* msg = create_msg("Sender", "Receiver", generate_random_word(20));
        strncpy(msg->id, id, sizeof(msg->id) - 1); // Override the generated ID with a test ID
        msg->id[sizeof(msg->id) - 1] = '\0'; // Ensure null-termination

        LRUCache_put(&cache, msg);
        store_msg(msg); // Storing message to the disk as well
    }

    // Test retrieval - all messages should be cache hits
    for (int i = 0; i < testCacheSize; ++i) {
        char id[10];
        sprintf(id, "MSG-%d", i);
        Message* retrieved = LRUCache_get(&cache, id);
        if (retrieved) {
            printf("Cache hit for %s\n", id);
        } else {
            printf("Cache miss for %s - ERROR!\n", id);
        }
    }

    // add one more message to trigger eviction
    Message* msg_evict = create_msg("Evictor", "Evictee", "This should trigger eviction");
    LRUCache_put(&cache, msg_evict);
    store_msg(msg_evict); // Storing to disk for retrieval test

    // Retrieve the evicted message, should be a miss and then a disk access
    Message* evicted_retrieved = LRUCache_get(&cache, "MSG-0");
    if (evicted_retrieved) {
        printf("Incorrectly retrieved MSG-0 from cache after eviction - ERROR!\n");
    } else {
        printf("Correctly did not find MSG-0 in cache, attempting disk retrieval...\n");
        // attempt to get it from disk, which should succeed and put it into the cache
        evicted_retrieved = retrieve_msg("MSG-0");
        if (evicted_retrieved) {
            printf("Retrieved MSG-0 from disk after eviction\n");
            LRUCache_put(&cache, evicted_retrieved); // should add it back to cache
        } else {
            printf("Failed to retrieve MSG-0 from disk - ERROR!\n");
        }
    }

    // Test that the cache now has the latest message and MSG-0 has been re-cached
    print_LRUCache_content(&cache);

    // Clean up
    clear_message_store(); 
}

// Test function for the Random Cache
void test_random_cache() {
    printf("Testing Random Cache...\n");

    // Initialize the random cache similarly to LRU cache
    randomCache random_cache;
    random_cache_initialize(&random_cache);
    const int testCacheSize = MAX_CACHE_SIZE;

    // Create and store a set of unique messages
    for (int i = 0; i < testCacheSize; ++i) {
        char* content = generate_random_word(20); // Generate message content
        Message* msg = create_msg("RandomSender", "RandomReceiver", content);
        char id[10];
        sprintf(id, "RNDMSG-%d", i); // Generate a unique ID for each message
        strncpy(msg->id, id, sizeof(msg->id) - 1); 
        msg->id[sizeof(msg->id) - 1] = '\0'; // Ensure null-termination
        random_cache_put(&random_cache, msg);
        store_msg(msg);
    }

    // Test retrieval - randomly access messages and check for their presence
    for (int i = 0; i < 1000; ++i) { // Increased the number to test beyond cache size
        int random_index = genRand(0, testCacheSize - 1); // Get a random index within range
        char id[10];
        sprintf(id, "RNDMSG-%d", random_index); // Generate ID for retrieval test
        Message* retrieved = random_cache_get(&random_cache, id);
        if (retrieved) {
            printf("Random Cache hit for %s\n", id);
        } else {
            printf("Random Cache miss for %s\n", id);
        }
    }

    print_random_cache_content(&random_cache); 
}

// Generate a set of 1000 messages
void generate_messages(Message* messages) {
    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        snprintf(messages[i].id, MAX_ID_LENGTH, "MSG-%d", i);
        messages[i].content = generate_random_word(MESSAGE_LENGTH);
    }
}

// Function to access the cache with random message IDs and record hits/misses
void access_caches(LRUCache* lruCache, randomCache* rndCache, Message* messages) {
    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        // Randomly choose a message to access
        int msgIndex = genRand(0, TOTAL_MESSAGES - 1);
        LRUCache_get(lruCache, messages[msgIndex].id); // Access LRU Cache
        random_cache_get(rndCache, messages[msgIndex].id); // Access Random Cache
    }
}

// Main test the cache metric function
void test_cache_performance() {
    // Initialize caches
    LRUCache lruCache;
    LRUCache_initialize(&lruCache);
    randomCache rndCache;
    random_cache_initialize(&rndCache);

    // Generate messages
    Message messages[TOTAL_MESSAGES];
    generate_messages(messages);

    // Put messages into caches
    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        LRUCache_put(&lruCache, &messages[i]);
        random_cache_put(&rndCache, &messages[i]);
    }

    // Access caches
    access_caches(&lruCache, &rndCache, messages);

    // Print cache performance metrics
    printf("LRU Cache Hits: %lu\n", lruCache.hit_count);
    printf("LRU Cache Misses: %lu\n", lruCache.miss_count);
    printf("LRU Cache Hit Ratio: %f\n", (float)lruCache.hit_count / TOTAL_MESSAGES);

    printf("Random Cache Hits: %lu\n", rndCache.hit_count);
    printf("Random Cache Misses: %lu\n", rndCache.miss_count);
    printf("Random Cache Hit Ratio: %f\n", (float)rndCache.hit_count / TOTAL_MESSAGES);

}

int main() {
    //clearing out message store on file first
    clear_message_store();
    test_lru_cache();
    test_random_cache();
    test_cache_performance();
    return 0;
}

