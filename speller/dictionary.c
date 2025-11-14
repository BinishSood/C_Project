// Implements a dictionary's functionality
// This file contains all the functions needed to create, use, and destroy
// a hash table data structure for spell-checking.

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "dictionary.h"


// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in the hash table (2^16)
const unsigned int N = 65536;

// Hash table - an array of node pointers, initialized to NULL
node *table[N] = {NULL};

// Global word counter for O(1) size() lookup
unsigned int word_count = 0;

// Returns true if 'word' is in the dictionary, else false
// Case-insensitive
bool check(const char *word)
{
    // Get the hash index for the word
    unsigned int index = hash(word);

    // Create a cursor to traverse the linked list at that index
    node *cursor = table[index];

    // Traverse the list
    while (cursor != NULL)
    {
        // Compare the input word with the word in the current node (case-insensitive)
        if (strcasecmp(cursor->word, word) == 0)
        {
            return true; // Word found
        }
        
        // Move to the next node
        cursor = cursor->next;
    }

    // If the loop finishes, the word was not found
    return false;
}

// Hashes 'word' to a number (an index for our hash table)
// Based on the "djb2" algorithm for good distribution
unsigned int hash(const char *word)
{
    // Start with a prime number
    unsigned long hash_value = 5381;
    int c;

    // Iterate over each character in the word (case-insensitively)
    while ((c = tolower(*word++)))
    {
        // The core of the algorithm: hash = (hash * 33) + c
        // Written using bitwise shifts for speed: (hash << 5) + hash = (hash * 32) + hash
        hash_value = ((hash_value << 5) + hash_value) + c;
    }

    // Modulo N to ensure the result is a valid index (0 to N-1)
    return hash_value % N;
}

// Loads the 'dictionary' file into the hash table
// Returns true if successful, else false
bool load(const char *dictionary)
{
    // 1. Open the dictionary file
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        printf("Could not open dictionary %s.\n", dictionary);
        return false;
    }

    // 2. Create a buffer to store each word
    char buffer[LENGTH + 1];

    // 3. Read words from the file one at a time
    while (fscanf(file, "%s", buffer) != EOF)
    {
        // 4. Create a new node for the word
        node *n = malloc(sizeof(node));
        if (n == NULL)
        {
            // Out of memory
            fclose(file);
            return false;
        }

        // 5. Copy the word into the new node
        strcpy(n->word, buffer);

        // 6. Hash the word to find its bucket index
        unsigned int index = hash(buffer);

        // 7. Insert the new node at the beginning of the linked list (prepending)
        n->next = table[index];
        table[index] = n;

        // 8. Increment the global word counter
        word_count++;
    }

    // 9. Close the file and return success
    fclose(file);
    return true;
}

// Returns the number of words in the dictionary
unsigned int size(void)
{
    // We simply return the global 'word_count' variable
    // This is O(1) because we kept track while loading
    return word_count;
}

// Unloads the dictionary from memory, freeing all allocated memory
// Returns true if successful
bool unload(void)
{
    // 1. Iterate over every bucket in the hash table
    for (int i = 0; i < N; i++)
    {
        // 2. Set a cursor to the head of the list for this bucket
        node *cursor = table[i];

        // 3. Loop while the list is not empty
        while (cursor != NULL)
        {
            // 4. Use a 'tmp' pointer to hold the current node
            node *tmp = cursor;
            
            // 5. Move the 'cursor' to the *next* node
            cursor = cursor->next;
            
            // 6. Free the node that 'tmp' is pointing to
            free(tmp);
        }
    }

    // 7. After iterating through all buckets, all memory is freed
    return true;
}