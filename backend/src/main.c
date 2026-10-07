#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "database.h"
#include "morse.h"
#include "server.h"

#define DEFAULT_SERVER_PORT 8080

static volatile sig_atomic_t shutdown_requested = 0;

static void request_shutdown(int signal_number)
{
    (void)signal_number;
    shutdown_requested = 1;
}

static int configured_server_port(unsigned short *server_port)
{
    if (server_port == NULL)
    {
        return 0;
    }

    *server_port = DEFAULT_SERVER_PORT;

    const char *port_text = getenv("PORT");

    if (port_text == NULL || port_text[0] == '\0')
    {
        return 0;
    }

    char *end = NULL;
    unsigned long port = strtoul(port_text, &end, 10);

    if (end == port_text ||
        *end != '\0' ||
        port == 0 ||
        port > 65535UL)
    {
        return 0;
    }

    *server_port = (unsigned short)port;
    return 1;
}

static void wait_for_shutdown_signal(void)
{
    signal(SIGINT, request_shutdown);
    signal(SIGTERM, request_shutdown);

    while (!shutdown_requested)
    {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
}

int main(void)
{
    unsigned short server_port = DEFAULT_SERVER_PORT;
    const int port_was_configured =
        configured_server_port(&server_port);

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
            server_port,
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
    printf("Backend port: %u\n", (unsigned int)server_port);
    printf("Health:       http://localhost:%u/api/health\n",
           (unsigned int)server_port);

    if (port_was_configured)
    {
        printf("Waiting for SIGINT or SIGTERM to stop the server...\n");
        wait_for_shutdown_signal();
    }
    else
    {
        printf("\nPress ENTER to stop the server...\n");
        getchar();
    }

    printf("\nStopping server...\n");

    server_stop(server);
    morse_translator_free(translator);
    database_close(database);

    printf("Server stopped successfully.\n");

    return 0;
}
