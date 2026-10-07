#include <stdio.h>

#include "morse_tree.h"

int main(void)
{
    MorseNode *root = morse_create_tree();

    if (root == NULL)
    {
        printf("Failed to create Morse tree.\n");
        return 1;
    }

    morse_insert(root, ".-", 'A');
    morse_insert(root, "-...", 'B');
    morse_insert(root, "-.-.", 'C');
    morse_insert(root, "-..", 'D');
    morse_insert(root, ".", 'E');
    morse_insert(root, "..-.", 'F');
    morse_insert(root, "--.", 'G');
    morse_insert(root, "....", 'H');
    morse_insert(root, "..", 'I');
    morse_insert(root, ".---", 'J');
    morse_insert(root, "-.-", 'K');
    morse_insert(root, ".-..", 'L');
    morse_insert(root, "--", 'M');
    morse_insert(root, "-.", 'N');
    morse_insert(root, "---", 'O');
    morse_insert(root, ".--.", 'P');
    morse_insert(root, "--.-", 'Q');
    morse_insert(root, ".-.", 'R');
    morse_insert(root, "...", 'S');
    morse_insert(root, "-", 'T');
    morse_insert(root, "..-", 'U');
    morse_insert(root, "...-", 'V');
    morse_insert(root, ".--", 'W');
    morse_insert(root, "-..-", 'X');
    morse_insert(root, "-.--", 'Y');
    morse_insert(root, "--..", 'Z');

    printf("A: %c\n", morse_decode(root, ".-"));
    printf("B: %c\n", morse_decode(root, "-..."));
    printf("C: %c\n", morse_decode(root, "-.-."));
    printf("H: %c\n", morse_decode(root, "...."));
    printf("L: %c\n", morse_decode(root, ".-.."));
    printf("O: %c\n", morse_decode(root, "---"));
    printf("Z: %c\n", morse_decode(root, "--.."));

    morse_free_tree(root);

    return 0;
}