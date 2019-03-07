/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Andrew Joseph
 * Date: 2/28/19
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
//Fix me: Do the necessary change to make the implementation //case-insensitive  
   while (1) 
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')  
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            // .............
            word[length] = tolower(c);
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    const char* fileName = "input1.txt";
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);
    FILE *inFile = fopen(fileName,"r");

    if(inFile != NULL) {
        clock_t timer = clock();
    
        HashMap* map = hashMapNew(10);
    
        // --- Concordance code begins here ---

        //set nextWord to first word in file, declare value to NULL to use in loop
        char* nxtWord = "nextWord(inFile)";
        int* value = NULL;

        while (nxtWord != NULL) {
            //get value of nxtWord in hash map
            value = hashMapGet(map, nxtWord);

            //if value already exists in map, add 1 to current total
            if (value != NULL) {
                hashMapPut(map, nxtWord, *value + 1);
            //else, value doesn't exist yet so put with count of 1
            } else {
                hashMapPut(map, nxtWord, 1);
            }
            // Be sure to free the word after you are done with it here.
            free(nxtWord);
            nxtWord = nextWord(inFile);
        }
        printf("Closing file: %s\n", fileName);
        fclose(inFile);

        // --- Concordance code ends here ---
    
        timer = clock() - timer;
        printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
        printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
        printf("Number of links: %d\n", hashMapSize(map));
        printf("Number of buckets: %d\n", hashMapCapacity(map));
        printf("Table load: %f\n", hashMapTableLoad(map));
    
        hashMapDelete(map);


    } else {
        printf("\n\n**FILE INPUT ERROR**\n\n");
    }

    return 0;
}