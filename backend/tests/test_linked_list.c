#include <stdio.h>

#include "linked_list.h"

int main(void)
{
    TranslationList *list = linked_list_create();

    if (list == NULL)
    {
        printf("Failed to create linked list.\n");
        return 1;
    }

    linked_list_append(
        list,
        1,
        "HELLO",
        ".... . .-.. .-.. ---",
        "TEXT_TO_MORSE"
    );

    linked_list_append(
        list,
        2,
        ".... . .-.. .-.. ---",
        "HELLO",
        "MORSE_TO_TEXT"
    );

    linked_list_append(
        list,
        3,
        "WORLD",
        ".-- --- .-. .-.. -..",
        "TEXT_TO_MORSE"
    );

    printf("Translation History:\n");
    linked_list_print(list);

    TranslationNode *found = linked_list_find(list, 2);

    if (found != NULL)
    {
        printf("\nFound ID 2: %s -> %s\n",
               found->input_text,
               found->output_text);
    }
    else
    {
        printf("\nID 2 not found.\n");
    }

    printf("\nDeleting ID 2...\n");

    if (linked_list_delete(list, 2))
    {
        printf("ID 2 deleted successfully.\n");
    }
    else
    {
        printf("ID 2 could not be deleted.\n");
    }

    printf("\nHistory after deletion:\n");
    linked_list_print(list);

    linked_list_free(list);

    return 0;
}