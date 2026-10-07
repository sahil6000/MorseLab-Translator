#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "morse.h"

typedef struct MorseMapping
{
    char character;
    const char *code;
} MorseMapping;

static const MorseMapping MORSE_MAPPINGS[] =
{
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.."},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."},

    {'0', "-----"},
    {'1', ".----"},
    {'2', "..---"},
    {'3', "...--"},
    {'4', "....-"},
    {'5', "....."},
    {'6', "-...."},
    {'7', "--..."},
    {'8', "---.."},
    {'9', "----."}
};

#define MORSE_MAPPING_COUNT \
    (sizeof(MORSE_MAPPINGS) / sizeof(MORSE_MAPPINGS[0]))

static void load_morse_mappings(MorseTranslator *translator)
{
    for (size_t i = 0; i < MORSE_MAPPING_COUNT; i++)
    {
        hash_table_insert(
            translator->encode_table,
            MORSE_MAPPINGS[i].character,
            MORSE_MAPPINGS[i].code
        );

        morse_insert(
            translator->decode_tree,
            MORSE_MAPPINGS[i].code,
            MORSE_MAPPINGS[i].character
        );
    }
}

MorseTranslator *morse_translator_create(void)
{
    MorseTranslator *translator =
        (MorseTranslator *)malloc(sizeof(MorseTranslator));

    if (translator == NULL)
    {
        return NULL;
    }

    translator->encode_table = hash_table_create();

    if (translator->encode_table == NULL)
    {
        free(translator);
        return NULL;
    }

    translator->decode_tree = morse_create_tree();

    if (translator->decode_tree == NULL)
    {
        hash_table_free(translator->encode_table);
        free(translator);
        return NULL;
    }

    load_morse_mappings(translator);

    return translator;
}

void morse_translator_free(MorseTranslator *translator)
{
    if (translator == NULL)
    {
        return;
    }

    hash_table_free(translator->encode_table);
    morse_free_tree(translator->decode_tree);

    free(translator);
}

char *morse_encode_text(
    MorseTranslator *translator,
    const char *text)
{
    if (translator == NULL ||
        translator->encode_table == NULL ||
        text == NULL)
    {
        return NULL;
    }

    size_t input_length = strlen(text);

    /*
     * Maximum output:
     * 5 Morse symbols per character + one separator.
     */
    size_t capacity = (input_length * 6) + 1;

    char *result = (char *)malloc(capacity);

    if (result == NULL)
    {
        return NULL;
    }

    size_t position = 0;

    for (size_t i = 0; i < input_length; i++)
    {
        char character = text[i];

        if (isspace((unsigned char)character))
        {
            if (position > 0 && result[position - 1] != '/')
            {
                result[position++] = '/';
                result[position++] = ' ';
            }

            continue;
        }

        character = (char)toupper((unsigned char)character);

        const char *code =
            hash_table_search(
                translator->encode_table,
                character
            );

        if (code == NULL)
        {
            free(result);
            return NULL;
        }

        if (position > 0 &&
            result[position - 1] != ' ')
        {
            result[position++] = ' ';
        }

        size_t code_length = strlen(code);

        memcpy(
            result + position,
            code,
            code_length
        );

        position += code_length;
    }

    result[position] = '\0';

    return result;
}

char *morse_decode_text(
    MorseTranslator *translator,
    const char *morse)
{
    if (translator == NULL ||
        translator->decode_tree == NULL ||
        morse == NULL)
    {
        return NULL;
    }

    size_t input_length = strlen(morse);

    char *result =
        (char *)malloc(input_length + 1);

    if (result == NULL)
    {
        return NULL;
    }

    size_t result_position = 0;
    size_t i = 0;

    while (i < input_length)
    {
        while (i < input_length &&
               isspace((unsigned char)morse[i]))
        {
            i++;
        }

        if (i >= input_length)
        {
            break;
        }

        if (morse[i] == '/')
        {
            result[result_position++] = ' ';
            i++;
            continue;
        }

        size_t start = i;

        while (i < input_length &&
               morse[i] != ' ' &&
               morse[i] != '/' &&
               !isspace((unsigned char)morse[i]))
        {
            i++;
        }

        size_t code_length = i - start;

        if (code_length == 0)
        {
            continue;
        }

        char *code =
            (char *)malloc(code_length + 1);

        if (code == NULL)
        {
            free(result);
            return NULL;
        }

        memcpy(
            code,
            morse + start,
            code_length
        );

        code[code_length] = '\0';

        char decoded =
            morse_decode(
                translator->decode_tree,
                code
            );

        free(code);

        if (decoded == '\0')
        {
            free(result);
            return NULL;
        }

        result[result_position++] = decoded;
    }

    result[result_position] = '\0';

    return result;
}