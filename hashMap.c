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
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    printf("in hash link delete\n\n");
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map) {
    assert(map != NULL);

    printf("in clean up function\n");

    struct HashLink *cur;
    //loop through each index in array
    for(int i = 0; i < map->capacity; i++) {
        //set cur to head of ll in index i
        cur = map->table[i];
        //loop through ll & delete links
        while(cur != NULL) {
            struct HashLink *tmp = cur;
            cur = cur->next;
            hashLinkDelete(tmp);
            map->size--;
        }
    }
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key) {
    assert(map != NULL);
    assert(key != NULL);
    
    printf("Get function Key:");
    for (int i = 0; i < strlen(key); i++) {
        printf(" %c", key[i]);
    }
    printf("\n\n");

    //get hash index
    int hashIndex = abs(HASH_FUNCTION(key) % map->capacity);

    //set pointer to head of linked list at hashIndex
    struct HashLink *cur = map->table[hashIndex];

    //search linked list for key & return pointer to value of link
    while(cur != NULL) {
        if(strcmp(key, cur->key) == 0) {
            printf("get: %d\n", cur->value);
            return &cur->value;
        }
        cur = cur->next;
    }
    printf("get: null\n");
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity) {
    assert(map != NULL);
    printf("in resize function\n");

    struct HashMap *newMap = malloc(sizeof(HashMap));
    assert(newMap != NULL);
    hashMapInit(newMap,capacity);

    struct HashLink *cur;

    //loop through old map and hash each key/value to newMap
    for (int i = 0; i < map->capacity; i++) {
        cur = map->table[i];
        while (cur != NULL) {
            hashMapPut(newMap,cur->key,cur->value);
            cur = cur->next;
        }
    }

    hashMapCleanUp(map); //delete indexes of old map

    map->table = newMap->table; //set map table to new map table
    map->capacity = capacity; //update capacity
    map->size = newMap->size; //reset size after using put function
    free(newMap); //free newMap

    printf("after resize capacity is: %d\n", hashMapCapacity(map));
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value) {
    assert(map != NULL);
    assert(key != NULL);
    printf("Put function Key:");
    for (int i = 0; i < strlen(key); i++) {
        printf(" %c", key[i]);
    }
    printf("\n\n");
   
    //get hash index
    int hashIndex = abs(HASH_FUNCTION(key) % map->capacity);

    //check if already exists
    struct HashLink *cur = map->table[hashIndex];
    struct HashLink *last = NULL;

    if (cur != NULL) {
        //loop through linked list to see if key already exists
        while (cur != NULL) {
            //if found, replace value and exit function
            if(strcmp(key, cur->key) == 0) {
              cur->value = value;
              return;
            } 
            last = cur;
            cur = cur->next;
        }
        //if key doesn't already exist, create new link
        HashLink *newLink = hashLinkNew(key, value, NULL);
        assert(newLink); 

        //add to bucket
        last->next = newLink;
    } else {
        //if key doesn't already exist, create new link
        HashLink *newLink = hashLinkNew(key, value, NULL);
        assert(newLink); 

        //add to bucket
        map->table[hashIndex] = newLink; 
    }
    map->size++; 

    //check load
    if (hashMapTableLoad(map) >= MAX_TABLE_LOAD) {
        resizeTable(map, (map->capacity * 2));
    } 
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key) {
    assert(map != NULL);
    printf("in remove function, key: %c\n", *key);
    printf("size before remove: %d\n", map->size);

    //get hash index
    int hashIndex = abs(HASH_FUNCTION(key) % map->capacity);

    //set a link to head of hash table index
    struct HashLink *cur = map->table[hashIndex];
    struct HashLink *last = NULL;

    //loop through linked list in hashed index
    while (cur != NULL) {
        if(strcmp(key, cur->key) == 0) {
            if(last == NULL) {
                map->table[hashIndex] = cur->next;
            } else {
                last->next = cur->next;
            }
            
            hashLinkDelete(cur);
            map->size--;
            return;   
        }
        last = cur;
        cur = cur->next;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key) {
    assert(map != NULL);
    printf("in contains function, key: %c\n", *key);

    //get hash index
    int hashIndex = abs(HASH_FUNCTION(key) % map->capacity);

    //set a link to head of hash table index
    struct HashLink *cur = map->table[hashIndex];

    //loop through linked list in hashed index
    while (cur != NULL) {
        //found a match, return true
        if (strcmp(key, cur->key) == 0) {
            printf("contains: 1\n\n");
            return 1;
        } 
        cur = cur->next;
    }
    //never found a match, return false
    printf("contains: 0\n\n");
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map) {
    printf("size: %d\n",map->size);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map) {
    printf("capacity: %d\n",map->capacity);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map) {
    int count = 0;
    for(int i = 0; i < map->capacity; i++) {
        if(map->table[i] == NULL) {
            count++;
        }
    }
    printf("empty buckets: %d\n", count);

    return count;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map) {
    printf("load: %f\n\n",((map->size / (float) map->capacity)));
    return ((map->size / (float) map->capacity));
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map) {
    assert(map != NULL);

    struct HashLink *cur;

    //loop through old map and hash each key/value to newMap
    for (int i = 0; i < map->capacity; i++) {
        cur = map->table[i];
        while (cur != NULL) {
            printf("%d ", cur->value);
            cur = cur->next;
        } 
    }
    printf("\n\n");
}











