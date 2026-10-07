#include <stdio.h>

#include "hash_table.h"

int main(void)
{
    HashTable *table = hash_table_create();

    if (table == NULL)
    {
        printf("Failed to create hash table.\n");
        return 1;
    }

    hash_table_insert(table, 'A', ".-");
    hash_table_insert(table, 'B', "-...");
    hash_table_insert(table, 'C', "-.-.");
    hash_table_insert(table, 'H', "....");
    hash_table_insert(table, 'L', ".-..");
    hash_table_insert(table, 'O', "---");
    hash_table_insert(table, 'Z', "--..");

    const char *a = hash_table_search(table, 'A');
    const char *h = hash_table_search(table, 'H');
    const char *l = hash_table_search(table, 'L');
    const char *o = hash_table_search(table, 'O');
    const char *z = hash_table_search(table, 'Z');

    printf("A: %s\n", a != NULL ? a : "NOT FOUND");
    printf("H: %s\n", h != NULL ? h : "NOT FOUND");
    printf("L: %s\n", l != NULL ? l : "NOT FOUND");
    printf("O: %s\n", o != NULL ? o : "NOT FOUND");
    printf("Z: %s\n", z != NULL ? z : "NOT FOUND");

    const char *unknown = hash_table_search(table, 'X');

    printf("X: %s\n", unknown != NULL ? unknown : "NOT FOUND");

    hash_table_free(table);

    return 0;
}