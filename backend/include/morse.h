#ifndef MORSE_H
#define MORSE_H

#include "hash_table.h"
#include "morse_tree.h"

typedef struct MorseTranslator
{
    HashTable *encode_table;
    MorseNode *decode_tree;
} MorseTranslator;

/* Translator lifecycle */
MorseTranslator *morse_translator_create(void);

void morse_translator_free(
    MorseTranslator *translator
);

/* Translation functions */
char *morse_encode_text(
    MorseTranslator *translator,
    const char *text
);

char *morse_decode_text(
    MorseTranslator *translator,
    const char *morse
);

#endif