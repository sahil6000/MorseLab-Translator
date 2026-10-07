#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct TranslationNode {
    int id;
    char *input_text;
    char *output_text;
    char *input_type;
    struct TranslationNode *next;
} TranslationNode;

typedef struct TranslationList {
    TranslationNode *head;
    TranslationNode *tail;
    int size;
} TranslationList;

TranslationList *linked_list_create(void);

int linked_list_append(
    TranslationList *list,
    int id,
    const char *input_text,
    const char *output_text,
    const char *input_type
);

TranslationNode *linked_list_find(
    TranslationList *list,
    int id
);

int linked_list_delete(
    TranslationList *list,
    int id
);

void linked_list_print(
    const TranslationList *list
);

void linked_list_free(
    TranslationList *list
);

#endif