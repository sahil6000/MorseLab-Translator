#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

static char *duplicate_string(const char *source)
{
    if (source == NULL)
    {
        return NULL;
    }

    size_t length = strlen(source);

    char *copy = (char *)malloc(length + 1);

    if (copy == NULL)
    {
        return NULL;
    }

    strcpy(copy, source);

    return copy;
}

TranslationList *linked_list_create(void)
{
    TranslationList *list = (TranslationList *)malloc(sizeof(TranslationList));

    if (list == NULL)
    {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

int linked_list_append(
    TranslationList *list,
    int id,
    const char *input_text,
    const char *output_text,
    const char *input_type)
{
    if (list == NULL ||
        input_text == NULL ||
        output_text == NULL ||
        input_type == NULL)
    {
        return 0;
    }

    TranslationNode *node =
        (TranslationNode *)malloc(sizeof(TranslationNode));

    if (node == NULL)
    {
        return 0;
    }

    node->input_text = duplicate_string(input_text);
    node->output_text = duplicate_string(output_text);
    node->input_type = duplicate_string(input_type);

    if (node->input_text == NULL ||
        node->output_text == NULL ||
        node->input_type == NULL)
    {
        free(node->input_text);
        free(node->output_text);
        free(node->input_type);
        free(node);

        return 0;
    }

    node->id = id;
    node->next = NULL;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;

    return 1;
}

TranslationNode *linked_list_find(
    TranslationList *list,
    int id)
{
    if (list == NULL)
    {
        return NULL;
    }

    TranslationNode *current = list->head;

    while (current != NULL)
    {
        if (current->id == id)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

int linked_list_delete(
    TranslationList *list,
    int id)
{
    if (list == NULL || list->head == NULL)
    {
        return 0;
    }

    TranslationNode *current = list->head;
    TranslationNode *previous = NULL;

    while (current != NULL)
    {
        if (current->id == id)
        {
            if (previous == NULL)
            {
                list->head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            if (current == list->tail)
            {
                list->tail = previous;
            }

            free(current->input_text);
            free(current->output_text);
            free(current->input_type);
            free(current);

            list->size--;

            if (list->size == 0)
            {
                list->head = NULL;
                list->tail = NULL;
            }

            return 1;
        }

        previous = current;
        current = current->next;
    }

    return 0;
}

void linked_list_print(
    const TranslationList *list)
{
    if (list == NULL)
    {
        return;
    }

    const TranslationNode *current = list->head;

    while (current != NULL)
    {
        printf(
            "ID: %d | Type: %s | Input: %s | Output: %s\n",
            current->id,
            current->input_type,
            current->input_text,
            current->output_text
        );

        current = current->next;
    }
}

void linked_list_free(
    TranslationList *list)
{
    if (list == NULL)
    {
        return;
    }

    TranslationNode *current = list->head;

    while (current != NULL)
    {
        TranslationNode *next = current->next;

        free(current->input_text);
        free(current->output_text);
        free(current->input_type);
        free(current);

        current = next;
    }

    free(list);
}