#ifndef MORSE_TREE_H
#define MORSE_TREE_H

typedef struct MorseNode {
    char character;
    struct MorseNode *dot;
    struct MorseNode *dash;
} MorseNode;

MorseNode *morse_create_node(char character);

MorseNode *morse_create_tree(void);

void morse_insert(MorseNode *root, const char *code, char character);

char morse_decode(MorseNode *root, const char *code);

void morse_free_tree(MorseNode *root);

#endif