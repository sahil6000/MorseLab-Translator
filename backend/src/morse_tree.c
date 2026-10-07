#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "morse_tree.h"

MorseNode *morse_create_node(char character)
{
    MorseNode *node = (MorseNode *)malloc(sizeof(MorseNode));

    if (node == NULL)
    {
        return NULL;
    }

    node->character = character;
    node->dot = NULL;
    node->dash = NULL;

    return node;
}

MorseNode *morse_create_tree(void)
{
    return morse_create_node('\0');
}

void morse_insert(MorseNode *root, const char *code, char character)
{
    if (root == NULL || code == NULL)
    {
        return;
    }

    MorseNode *current = root;

    for (size_t i = 0; code[i] != '\0'; i++)
    {
        MorseNode **next = NULL;

        if (code[i] == '.')
        {
            next = &current->dot;
        }
        else if (code[i] == '-')
        {
            next = &current->dash;
        }
        else
        {
            return;
        }

        if (*next == NULL)
        {
            *next = morse_create_node('\0');

            if (*next == NULL)
            {
                return;
            }
        }

        current = *next;
    }

    current->character = character;
}

char morse_decode(MorseNode *root, const char *code)
{
    if (root == NULL || code == NULL || code[0] == '\0')
    {
        return '\0';
    }

    MorseNode *current = root;

    for (size_t i = 0; code[i] != '\0'; i++)
    {
        if (code[i] == '.')
        {
            current = current->dot;
        }
        else if (code[i] == '-')
        {
            current = current->dash;
        }
        else
        {
            return '\0';
        }

        if (current == NULL)
        {
            return '\0';
        }
    }

    return current->character;
}

void morse_free_tree(MorseNode *root)
{
    if (root == NULL)
    {
        return;
    }

    morse_free_tree(root->dot);
    morse_free_tree(root->dash);

    free(root);
}