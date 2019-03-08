/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Andrew Joseph
 * Date: 2/28/19
 */


#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



//https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshteinDistance(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);

    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    printf("");
    return(column[s1len]);
}


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
            word[length] = c;
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
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map) {
    char* nxtWord = nextWord(file);
    int* value = NULL;

    while (nxtWord != NULL) {
        //get value of nxtWord in hash map
        value = hashMapGet(map, nxtWord);
        
        if (value != NULL) {
            hashMapPut(map, nxtWord, *value + 1);
            //else, value doesn't exist yet so put with count of 1
        } else {
            hashMapPut(map, nxtWord, 1);
        }

        nxtWord = nextWord(file);
    }
    //hashMapPrint(map);
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
    HashMap* map = hashMapNew(1000);
    char closestMatches[5][256]; //array of 5 closest matches
    int levDistance = 0;
    int min = 1000;

    char bestMatch[100]; //get rid of
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    int quit = 0;
    int correctSpelling = 0;
    while (!quit) {

        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        //check for special characters
        /* to do */

        //convert user input to lowercase
        for(int i = 0; i < strlen(inputBuffer); i++) {
            if (inputBuffer[i] >= 'A' && inputBuffer[i] <= 'Z') {
                inputBuffer[i] = tolower(inputBuffer[i]);
            }
        }
        
        // Implement the spell checker code here..

        //check to see if word is in the hash map
        correctSpelling = hashMapContainsKey(map, inputBuffer);

        //if the word is found, print correct
        if (correctSpelling == 1) {
            printf("\nThe inputted word .... is spelled correctly\n\n");

        //else misspelled, find 5 closest words to suggest using levenshtein distance
        } else {
            printf("\nThe inputted word .... is spelled incorrectly\n");
            printf("Did you mean...?\n");

            //find 5 closest words

            /*loop through hash map and compare input to dictionary using levenshtein distance*/
            struct HashLink *cur;

            for (int i = 0; i < hashMapCapacity(map); i++) {
                cur = map->table[i];
                while (cur != NULL) {
                    levDistance = levenshteinDistance(inputBuffer, cur->key);

                    if (levDistance < min) {
                        min = levDistance;
                        strcpy(bestMatch, inputBuffer);
                    }

                    cur = cur->next;
                }
            }
            
            //print best match
            printf("best match:\n\n");
            for (int i = 0; i < strlen(bestMatch); i++) {
                printf("%c", bestMatch[i]);
            }
            printf("\n\n");
        }


        
        if (strcmp(inputBuffer, "quit") == 0) {
            quit = 1;
        }
    }

    printf("Goodbye\n");
    
    hashMapDelete(map);
    return 0;
}