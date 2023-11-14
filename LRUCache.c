#include "LRUCache.h"
#include <stdlib.h>
#include <string.h>

// Forward declaration of private helper functions
static void remove_node(LRUCache* cache, CacheNode* node);
static void add_node_to_front(LRUCache* cache, CacheNode* node);
static int hash(const char* str);

// Initialize a least recently used cache
void LRUCache_initialize(LRUCache* cache) {
    memset(cache->nodes, 0, sizeof(cache->nodes));
    cache->head = NULL;
    cache->tail = NULL;
    cache->current_size = 0;
    cache->hit_count = 0;
    cache->miss_count = 0;
}

// Free all resources used by the cache
void LRUCache_free(LRUCache* cache) {
    CacheNode* current = cache->head;
    while (current) {
        CacheNode* next = current->next;
        free_msg(current->message); // free the message
        free(current); // free the node
        current = next;
    }
    cache->head = cache->tail = NULL;
    cache->current_size = 0;
}

// Insert an item into the cache, returns true if successful and false otherwise
CacheNode* LRUCache_put(LRUCache* cache, Message* message) {
    int index = hash(message->id);

    // If the message is already in cache, update it and move it to the front.
    CacheNode* existing_node = cache->nodes[index];
    while (existing_node && strcmp(existing_node->message->id, message->id) != 0) {
        existing_node = existing_node->next;
    }

    if (existing_node) {
        // Update the message content
        free(existing_node->message->content); // free the old content
        existing_node->message->content = strdup(message->content); // duplicate the new content
        remove_node(cache, existing_node); 
        add_node_to_front(cache, existing_node);
    } else {
        // Create a new cache node for the new message.
        CacheNode* new_node = (CacheNode*)malloc(sizeof(CacheNode));
        if (!new_node) {
            return NULL; // Could not allocate memory for cache node.
        }
        new_node->message = message;

        // If the cache is full, remove the least recently used item.
        if (cache->current_size == MAX_CACHE_SIZE) {
            cache->nodes[hash(cache->tail->message->id)] = NULL;
            remove_node(cache, cache->tail);
        }

        // Add the new node to the front of the list and update the hash map.
        add_node_to_front(cache, new_node);
        cache->nodes[index] = new_node;

        // Increment the current size of the cache.
        if (cache->current_size < MAX_CACHE_SIZE) {
            cache->current_size++;
        }
    }

    return existing_node;
}

// Get an item from the cache, returns NULL if not found
Message* LRUCache_get(LRUCache* cache, const char* id) {
    int index = hash(id);

    // Look for the message in the hash map.
    CacheNode* node = cache->nodes[index];
    while (node && strcmp(node->message->id, id) != 0) {
        node = node->next;
    }

    if (node) {
        // Move the accessed node to the front of the list.
        remove_node(cache, node);
        add_node_to_front(cache, node);
        cache->hit_count++; // Increment hit counter
        return node->message;
    } else {
        cache->miss_count++; // Increment miss counter
    }

    return NULL; // Cache miss
}

// Remove a node from the doubly linked list
static void remove_node(LRUCache* cache, CacheNode* node) {
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
}

// Add a node to the front of the doubly linked list
static void add_node_to_front(LRUCache* cache, CacheNode* node) {
    node->next = cache->head;
    node->prev = NULL;

    if (cache->head) {
        cache->head->prev = node;
    }

    cache->head = node;

    if (cache->tail == NULL) {
        cache->tail = node;
    }
}

// hash function to map a string to an index.
static int hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % MAX_CACHE_SIZE;
}
