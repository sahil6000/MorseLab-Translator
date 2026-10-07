#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HASH_TABLE_SIZE 53
#define MORSE_CODE_MAX_LENGTH 5

typedef struct HashEntry {
    char character;
    char morse_code[MORSE_CODE_MAX_LENGTH + 1];
    struct HashEntry *next;
} HashEntry;

typedef struct HashTable {
    HashEntry *buckets[HASH_TABLE_SIZE];
} HashTable;

HashTable *hash_table_create(void);

int hash_table_insert(HashTable *table, char character, const char *morse_code);

const char *hash_table_search(HashTable *table, char character);

void hash_table_free(HashTable *table);

#endif