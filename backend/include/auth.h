#ifndef AUTH_H
#define AUTH_H

#include "database.h"

#define AUTH_SALT_SIZE 16
#define AUTH_HASH_SIZE 32
#define AUTH_PASSWORD_HASH_STRING_SIZE 128

#define AUTH_TOKEN_SIZE 64

typedef struct AuthResult {
    int success;
    int user_id;
    char name[128];
    char message[256];
    char token[AUTH_TOKEN_SIZE + 1];
} AuthResult;

/* Create a secure password hash using PBKDF2-HMAC-SHA256. */
int auth_hash_password(
    const char *password,
    char *password_hash,
    int password_hash_size
);

/* Verify a password against a stored password hash. */
int auth_verify_password(
    const char *password,
    const char *stored_hash
);

/* Register a new user. */
AuthResult auth_signup(
    Database *database,
    const char *name,
    const char *username,
    const char *email,
    const char *password
);

/* Authenticate an existing user. */
AuthResult auth_login(
    Database *database,
    const char *email,
    const char *password
);

int auth_change_password(
    Database *database,
    int user_id,
    const char *current_password,
    const char *new_password
);

/* Create an authentication session token for a user. */
int auth_create_session(
    int user_id,
    char *token,
    int token_size
);

/* Validate a session token and return the associated user ID. */
int auth_validate_session(
    const char *token,
    int *user_id
);

/* Destroy an authentication session token. */
int auth_destroy_session(
    const char *token
);

/* Destroy every active session belonging to a user. */
void auth_destroy_user_sessions(
    int user_id
);

#endif
