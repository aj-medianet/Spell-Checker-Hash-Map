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


struct ClosestMatches {
    char word[256];
    int lev_distance;
};




#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

//https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
/**
 * gets the levenshtein distance of two strings
 * @param s1, s2
 * @returns integer distance
 */

int getLevenshteinDistance(char *s1, char *s2) {
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
        free(nxtWord);
        nxtWord = nextWord(file);
    }

    //hashMapPrint(map);
}

/**
* Checks that the input is all letters
* param - inputBuffer
* @return integer, 1 for true 0 for false
*/
int getValidInput(char* inputBuffer) {
    for(int i = 0; i < strlen(inputBuffer); i++) {
        if( inputBuffer[i] < 65 || (inputBuffer[i] > 90 && inputBuffer[i] < 97) || inputBuffer[i] > 122 || isspace(inputBuffer[i] || inputBuffer[i] == '\n') ) {
            printf("\n\n*** Invalid Input. Only 1 word with letters allowed. ***\n\n");
            return 0;
        }

        
    }
    return 1;
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
    struct ClosestMatches cm[5]; //array of 5 closest matches
    int ld = 0;
    int validInput = 0;
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    int quit = 0;
    int correctSpelling = 0;

    char tmpWord[256];
    int tmpLev = 0;
    char tmpWordTwo[256];
    int tmpLevTwo = 0;
    char inputChar = 0;

    int k = 0;
    int charPosition = 0;

    while (!quit) {  
        fflush(stdin);
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        //feeds potential space character into inputBuffer to check for
        inputChar = 0;
        charPosition = strlen(inputBuffer);
        while((inputChar = getchar()) != '\n' && inputChar != EOF) {
            inputBuffer[charPosition] = inputChar;
            inputBuffer[charPosition + 1] = '\0';
            charPosition++;
        }

        //check for special characters & spaces
        validInput = getValidInput(inputBuffer); 

        if (validInput) {
            //initialize lev distances
            ld = 0; //reset ld
            for(int i = 0; i < 5; i++) {
                cm[i].lev_distance = 1000;
                cm[i].word[0] = '\0';
            }

            //convert user input to lowercase
            for(int i = 0; i < strlen(inputBuffer); i++) {
                if (inputBuffer[i] >= 'A' && inputBuffer[i] <= 'Z') {
                    inputBuffer[i] = tolower(inputBuffer[i]);
                }
            }

            //check to see if word is in the hash map
            correctSpelling = hashMapContainsKey(map, inputBuffer);

            //if the word is found, print correct
            if (correctSpelling == 1) {
                printf("\nThe inputted word .... is spelled correctly\n\n");

            //else misspelled, find 5 closest words to suggest using levenshtein distance
            } else {
                printf("\nThe inputted word .... is spelled incorrectly\n");
                printf("Did you mean...?\n\n");

                //find 5 closest words

                /*loop through hash map and compare input to dictionary using levenshtein distance*/
                struct HashLink *cur;

                for (int i = 0; i < hashMapCapacity(map); i++) {
                    cur = map->table[i];
                    while (cur != NULL) {
                        ld = getLevenshteinDistance(inputBuffer, cur->key);

                        //loop through array of ClosestMatches to check lev distances
                        for (int j = 0; j < 5; j++) {
                            //if new lev distance is lower, add it to the array and break out of loop
                            if (ld < cm[j].lev_distance) {
                                //copy current word to tmp variable to check against the rest of array
                                strcpy(tmpWord, cm[j].word);
                                tmpLev = cm[j].lev_distance;

                                //assign new word to spot in array
                                strcpy(cm[j].word, cur->key);
                                cm[j].lev_distance = ld;

                                //check swapped word against other spots
                                k = 0;
                                while(k < 5) {
                                    //if tmp is less than index lev dis, swap and go through array again
                                    if(tmpLev < cm[k].lev_distance) {
                                        //copy word to tmp2
                                        strcpy(tmpWordTwo, cm[k].word);
                                        tmpLevTwo = cm[k].lev_distance;

                                        //cpy tmp to word
                                        strcpy(cm[k].word, tmpWord);
                                        cm[k].lev_distance = tmpLev;

                                        //cpy tmp2 to tmp 
                                        strcpy(tmpWord, tmpWordTwo);
                                        tmpLev = tmpLevTwo;

                                        //set k to 0 to run again
                                        k = 0;
                                    } else {
                                        //if tmp is greater than index lev, move to next index in array
                                        k++;
                                    }
                                    
                                }

                                break;
                            }
                        }

                        cur = cur->next;
                    }
                }
            
                //print best/closest match
                for (int i = 0; i < 5; i++) {
                    printf("%s\n", cm[i].word);
                }
                printf("\n\n");
            }
        }

        if (strcmp(inputBuffer, "quit") == 0) {
            quit = 1;
        }
        
        
    }

    printf("Goodbye\n");
    
    hashMapDelete(map);
    return 0;
}