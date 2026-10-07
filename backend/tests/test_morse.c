#include <stdio.h>
#include <stdlib.h>

#include "morse.h"

int main(void)
{
    MorseTranslator *translator = morse_translator_create();

    if (translator == NULL)
    {
        printf("Failed to create Morse translator.\n");
        return 1;
    }

    printf("=== TEXT TO MORSE ===\n");

    char *encoded = morse_encode_text(
        translator,
        "HELLO WORLD"
    );

    if (encoded != NULL)
    {
        printf("HELLO WORLD -> %s\n", encoded);
    }
    else
    {
        printf("Encoding failed.\n");
        morse_translator_free(translator);
        return 1;
    }

    printf("\n=== MORSE TO TEXT ===\n");

    char *decoded = morse_decode_text(
        translator,
        ".... . .-.. .-.. --- / .-- --- .-. .-.. -.."
    );

    if (decoded != NULL)
    {
        printf(
            ".... . .-.. .-.. --- / .-- --- .-. .-.. -.. -> %s\n",
            decoded
        );
    }
    else
    {
        printf("Decoding failed.\n");
        free(encoded);
        morse_translator_free(translator);
        return 1;
    }

    printf("\n=== ADDITIONAL TEST ===\n");

    char *encoded_sos = morse_encode_text(
        translator,
        "SOS"
    );

    if (encoded_sos != NULL)
    {
        printf("SOS -> %s\n", encoded_sos);
    }

    char *decoded_sos = morse_decode_text(
        translator,
        "... --- ..."
    );

    if (decoded_sos != NULL)
    {
        printf("... --- ... -> %s\n", decoded_sos);
    }

    free(encoded);
    free(decoded);
    free(encoded_sos);
    free(decoded_sos);

    morse_translator_free(translator);

    printf("\nMorse translator test completed successfully.\n");

    return 0;
}