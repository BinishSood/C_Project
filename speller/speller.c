// Implements a spell-checker

#include <ctype.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include<string.h>

#include "dictionary.h"

// Undefine any definitions
#undef calculate
#undef getrusage

// Default dictionary
#define DICTIONARY "dictionaries/large"

// Prototype
double calculate(const struct rusage *b, const struct rusage *a);

int main(int argc, char *argv[])
{
    // Check for correct number of args
    if (argc != 2 && argc != 3)
    {
        printf("Usage: ./speller [DICTIONARY] text\n");
        return 1;
    }

    // Structures for timing data
    struct rusage before, after;

    // Benchmarks
    double time_load = 0.0, time_check = 0.0, time_size = 0.0, time_unload = 0.0;

    // Determine dictionary to use
    char *dictionary = (argc == 3) ? argv[1] : DICTIONARY;

    // Load dictionary
    getrusage(RUSAGE_SELF, &before);
    bool loaded = load(dictionary);
    getrusage(RUSAGE_SELF, &after);

    // Exit if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }

    // Calculate time to load dictionary
    time_load = calculate(&before, &after);

    // Try to open text
    char *text = (argc == 3) ? argv[2] : argv[1];
    FILE *file = fopen(text, "r");
    FILE *temp = fopen("temp.txt", "w+");
    if (file == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }

    // Prepare to report misspellings
    printf("\nCORRECTED WORDS\n\n");

    // Prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    bool int_in_word = false, special_char_in_word = false;
    char word[LENGTH + 1];

    // Spell-check each word in text
    char c;
    char punctuation[2];
    punctuation[1] = '\0';
    while (fread(&c, sizeof(char), 1, file))
    {
        bool punctuation_break = ispunct((unsigned char) c);
        bool end_of_word = isspace((unsigned char) c) || punctuation_break || c=='\n';

        // Append character to word when we are still building it
        if(!end_of_word)
        {
            word[index] = c;
            index++;
            if (isdigit(c))
            {
                int_in_word = true;
            }
            else if(!isalpha(c))
            {
                special_char_in_word = true;
            }
        }

        // If we hit a delimiter without having accumulated a word, just mirror the delimiter
        if (end_of_word && index == 0)
        {
            fputc(c, temp);
            continue;
        }
        // Ignore alphabetical strings too long to be words
        if (index > LENGTH)
        {
            // Consume remainder of alphabetical string
            while (fread(&c, sizeof(char), 1, file) && isalpha(c));

            // Prepare for new word
            index = 0;
            int_in_word = false;
            special_char_in_word = false;
        }

        // We must have found a whole word
        else if (end_of_word)
        {
            // Terminate current word
            word[index] = '\0';
            punctuation[0] = c;
            // Update counter
            words++;
            
            // Check word's spelling
            getrusage(RUSAGE_SELF, &before);
            bool misspelled = !check(word);
            getrusage(RUSAGE_SELF, &after);
            if (int_in_word || special_char_in_word)
                misspelled = false;

            // Update benchmark
            time_check += calculate(&before, &after);

            // Print word if misspelled
            if (misspelled)
            {
                char **dict = getAllDictionaryWords();
                char *correct_word = best_match(word, dict, size());
                printf("%s -> %s\n", word, correct_word);
                fputs(correct_word, temp);
                fputs(punctuation, temp);
                misspellings++;
            }
            else
            {
                fputs(word, temp);
                fputs(punctuation, temp);
            }

            // Prepare for next word
            index = 0;
            int_in_word = false;
            special_char_in_word = false;
        }
    }

    // Check whether there was an error
    if (ferror(file))
    {
        fclose(file);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }

    // Close text
    fclose(file);
    fclose(temp);
    remove(text);
    rename("temp.txt", text);

    // Determine dictionary's size
    getrusage(RUSAGE_SELF, &before);
    unsigned int n = size();
    getrusage(RUSAGE_SELF, &after);

    // Calculate time to determine dictionary's size
    time_size = calculate(&before, &after);

    // Unload dictionary
    getrusage(RUSAGE_SELF, &before);
    bool unloaded = unload();
    getrusage(RUSAGE_SELF, &after);

    // Abort if dictionary not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s.\n", dictionary);
        return 1;
    }

    // Calculate time to unload dictionary
    time_unload = calculate(&before, &after);

    // Report benchmarks
    printf("\nWORDS MISSPELLED:     %d\n", misspellings);
    printf("WORDS IN DICTIONARY:  %d\n", n);
    printf("WORDS IN TEXT:        %d\n", words);
    printf("TIME IN load:         %.2f\n", time_load);
    printf("TIME IN check:        %.2f\n", time_check);
    printf("TIME IN size:         %.2f\n", time_size);
    printf("TIME IN unload:       %.2f\n", time_unload);
    printf("TIME IN TOTAL:        %.2f\n\n",
           time_load + time_check + time_size + time_unload);

    // Success
    return 0;
}

// Returns number of seconds between b and a
double calculate(const struct rusage *b, const struct rusage *a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                  (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                 ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                  (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
    }
}
