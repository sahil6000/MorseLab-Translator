#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>

#include "database.h"
#include "morse.h"

typedef struct ServerContext
{
    Database *database;
    MorseTranslator *translator;
} ServerContext;

/* Start HTTP server */
struct MHD_Daemon *server_start(
    unsigned short port,
    ServerContext *context
);

/* Stop HTTP server */
void server_stop(
    struct MHD_Daemon *daemon
);

#endif