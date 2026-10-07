#include <stdio.h>

#include "database.h"
#include "morse.h"
#include "server.h"

#define SERVER_PORT 8080

int main(void)
{
    printf("========================================\n");
    printf("        MORSELAB C BACKEND SERVER\n");
    printf("========================================\n");

    Database *database =
        database_open("backend/database/morselab.db");

    if (database == NULL)
    {
        printf("Failed to open SQLite database.\n");
        return 1;
    }

    printf("SQLite database opened successfully.\n");

    if (!database_initialize_schema(database))
    {
        printf("Failed to initialize database schema.\n");
        database_close(database);
        return 1;
    }

    printf("Database schema initialized successfully.\n");

    MorseTranslator *translator =
        morse_translator_create();

    if (translator == NULL)
    {
        printf("Failed to create Morse translator.\n");
        database_close(database);
        return 1;
    }

    printf("Morse translation engine initialized.\n");

    ServerContext context;

    context.database = database;
    context.translator = translator;

    struct MHD_Daemon *server =
        server_start(
            SERVER_PORT,
            &context
        );

    if (server == NULL)
    {
        printf("Failed to start HTTP server.\n");

        morse_translator_free(translator);
        database_close(database);

        return 1;
    }

    printf("\nServer started successfully.\n");
    printf("Backend URL: http://localhost:%d\n", SERVER_PORT);
    printf("Health:      http://localhost:%d/api/health\n", SERVER_PORT);
    printf("\nPress ENTER to stop the server...\n");

    getchar();

    printf("\nStopping server...\n");

    server_stop(server);
    morse_translator_free(translator);
    database_close(database);

    printf("Server stopped successfully.\n");

    return 0;
}