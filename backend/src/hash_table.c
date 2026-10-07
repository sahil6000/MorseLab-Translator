#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static unsigned int hash_character(char character)
{
    return (unsigned int)((unsigned char)character) % HASH_TABLE_SIZE;
}

HashTable *hash_table_create(void)
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));

    if (table == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        table->buckets[i] = NULL;
    }

    return table;
}

int hash_table_insert(HashTable *table, char character, const char *morse_code)
{
    if (table == NULL || morse_code == NULL)
    {
        return 0;
    }

    unsigned int index = hash_character(character);

    HashEntry *current = table->buckets[index];

    while (current != NULL)
    {
        if (current->character == character)
        {
            strncpy(current->morse_code, morse_code, MORSE_CODE_MAX_LENGTH);
            current->morse_code[MORSE_CODE_MAX_LENGTH] = '\0';
            return 1;
        }

        current = current->next;
    }

    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));

    if (new_entry == NULL)
    {
        return 0;
    }

    new_entry->character = character;

    strncpy(new_entry->morse_code, morse_code, MORSE_CODE_MAX_LENGTH);
    new_entry->morse_code[MORSE_CODE_MAX_LENGTH] = '\0';

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;

    return 1;
}

const char *hash_table_search(HashTable *table, char character)
{
    if (table == NULL)
    {
        return NULL;
    }

    unsigned int index = hash_character(character);

    HashEntry *current = table->buckets[index];

    while (current != NULL)
    {
        if (current->character == character)
        {
            return current->morse_code;
        }

        current = current->next;
    }

    return NULL;
}

void hash_table_free(HashTable *table)
{
    if (table == NULL)
    {
        return;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        HashEntry *current = table->buckets[i];

        while (current != NULL)
        {
            HashEntry *next = current->next;
            free(current);
            current = next;
        }
    }

    free(table);
}