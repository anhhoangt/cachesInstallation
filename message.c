#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define MESSAGE_STORE_FILE "messageStore.txt"
#define DELIMITER "|"


// Helper function to get the current time as a string
static char* get_current_time() {
    time_t now = time(NULL);
    char* time_str = ctime(&now);
    if (!time_str) {
        return NULL; // Time retrieval failed
    }
    time_str[strlen(time_str) - 1] = '\0'; // Remove newline
    return strdup(time_str);
}

// Clear the message store file
void clear_message_store() {
    FILE* file = fopen(MESSAGE_STORE_FILE, "w"); // Open in write mode to clear the file
    if (!file) {
        fprintf(stderr, "Error: Unable to clear message store.\n");
        return;
    }
    fclose(file); // Closing the file will truncate it to zero length
}

// Create a new message
Message* create_msg(const char* sender, const char* receiver, const char* content) {
    static int id_counter = 0;
    if (!sender || !receiver || !content) {
        return NULL; // Invalid arguments
    }

    Message* msg = malloc(sizeof(Message));
    if (!msg) {
        return NULL; // Memory allocation failed
    }

    snprintf(msg->id, ID_SIZE, "MSG-%06d", id_counter++); //format and store a series of char in array buffer
    msg->id[ID_SIZE - 1] = '\0'; // Ensure null termination
    msg->time_sent = get_current_time();
    msg->sender = strdup(sender);
    msg->receiver = strdup(receiver);
    msg->content = strdup(content);
    msg->delivered = 0;

    return msg;
}

// Store a message in the message store file
int store_msg(Message* msg) {
    //message is null
    if (!msg) {
        fprintf(stderr, "Error: Message is NULL.\n");
        return -1;
    }

    // Ensure that strings are not NULL before writing.
    if (!msg->time_sent || !msg->sender || !msg->receiver || !msg->content) {
        fprintf(stderr, "Error: One or more message fields are NULL.\n");
        return -1;
    }

    FILE* file = fopen(MESSAGE_STORE_FILE, "a");
    if (!file) {
        perror("Error: Unable to open file for writing");
        return -1;
    }

    int write_count = fprintf(file, "%s%s%s%s%s%s%s%s%s\n",
                              msg->id, DELIMITER,
                              msg->time_sent, DELIMITER,
                              msg->sender, DELIMITER,
                              msg->receiver, DELIMITER,
                              msg->content);
    if (write_count < 0) {
        perror("Error writing to file"); 
        fclose(file); // Always close the file if open.
        return -1;
    }
    fclose(file);
    return 0;
}

// Retrieve a message from the message store file
Message* retrieve_msg(const char* id) {
    //message ID is null
    if (!id) {
        fprintf(stderr, "Error: Message ID is NULL.\n");
        return NULL;
    }

    FILE* file = fopen(MESSAGE_STORE_FILE, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file for reading.\n");
        return NULL;
    }

    char line[1024];
    //reads a limited number of characters from a given file stream source into an array of characters
    while (fgets(line, sizeof(line), file)) { //
        Message* msg = malloc(sizeof(Message));
        if (!msg) {
            fclose(file);
            return NULL; // Memory allocation failed
        }
        // Use a temporary buffer to hold the token for ID comparison
        char temp_id[ID_SIZE]; 
        char* token = strtok(line, DELIMITER);
        if (token && strcmp(token, id) == 0) {
            Message* msg = malloc(sizeof(Message));
            if (!msg) {
                fclose(file);
                return NULL; // Memory allocation failed
            }

            // Use strncpy to copy the ID into the message structure's ID field
            strncpy(msg->id, token, sizeof(msg->id) - 1);
            msg->id[sizeof(msg->id) - 1] = '\0';
            msg->time_sent = strdup(strtok(NULL, DELIMITER));
            msg->sender = strdup(strtok(NULL, DELIMITER));
            msg->receiver = strdup(strtok(NULL, DELIMITER));
            msg->content = strdup(strtok(NULL, DELIMITER));
            msg->delivered = 0; 

            fclose(file);
            return msg;
        }
    }

    fclose(file);
    return NULL; // Message not found
}

// Free all resources used by a message
void free_msg(Message* msg) {
    if (!msg) {
        return;
    }

    free(msg->id);
    free(msg->time_sent);
    free(msg->sender);
    free(msg->receiver);
    free(msg->content);
    free(msg);
}


