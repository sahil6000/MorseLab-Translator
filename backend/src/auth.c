#include "auth.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PBKDF2_ITERATIONS 600000

#define HEX_SALT_LENGTH 32
#define HEX_HASH_LENGTH 64

#define STORED_HASH_PREFIX "v1$"

/*
 * Authentication sessions
 *
 * Session tokens are kept in memory while the C backend is running.
 * Restarting the backend invalidates all active sessions.
 */
#define SESSION_TOKEN_BYTES 32
#define SESSION_DURATION_SECONDS (24 * 60 * 60)

typedef struct SessionNode
{
    int user_id;
    char token[AUTH_TOKEN_SIZE + 1];
    time_t created_at;
    time_t expires_at;

    struct SessionNode *next;
} SessionNode;

static SessionNode *session_head = NULL;


/* =========================================================
   HEX UTILITIES
   ========================================================= */

static void bytes_to_hex(
    const unsigned char *bytes,
    int length,
    char *hex
)
{
    static const char hex_chars[] =
        "0123456789abcdef";

    if (bytes == NULL ||
        hex == NULL ||
        length <= 0)
    {
        return;
    }

    for (int i = 0; i < length; i++)
    {
        hex[i * 2] =
            hex_chars[(bytes[i] >> 4) & 0x0F];

        hex[i * 2 + 1] =
            hex_chars[bytes[i] & 0x0F];
    }

    hex[length * 2] = '\0';
}


static int hex_to_bytes(
    const char *hex,
    unsigned char *bytes,
    int expected_length
)
{
    if (hex == NULL ||
        bytes == NULL ||
        expected_length <= 0)
    {
        return 0;
    }

    if ((int)strlen(hex) != expected_length * 2)
    {
        return 0;
    }

    for (int i = 0; i < expected_length; i++)
    {
        unsigned char high;
        unsigned char low;

        char high_char = hex[i * 2];
        char low_char = hex[i * 2 + 1];

        if (high_char >= '0' &&
            high_char <= '9')
        {
            high = (unsigned char)(high_char - '0');
        }
        else if (high_char >= 'a' &&
                 high_char <= 'f')
        {
            high = (unsigned char)(high_char - 'a' + 10);
        }
        else if (high_char >= 'A' &&
                 high_char <= 'F')
        {
            high = (unsigned char)(high_char - 'A' + 10);
        }
        else
        {
            return 0;
        }

        if (low_char >= '0' &&
            low_char <= '9')
        {
            low = (unsigned char)(low_char - '0');
        }
        else if (low_char >= 'a' &&
                 low_char <= 'f')
        {
            low = (unsigned char)(low_char - 'a' + 10);
        }
        else if (low_char >= 'A' &&
                 low_char <= 'F')
        {
            low = (unsigned char)(low_char - 'A' + 10);
        }
        else
        {
            return 0;
        }

        bytes[i] =
            (unsigned char)((high << 4) | low);
    }

    return 1;
}


/* =========================================================
   INPUT VALIDATION
   ========================================================= */

static int valid_password(
    const char *password
)
{
    if (password == NULL)
    {
        return 0;
    }

    size_t length = strlen(password);

    return length >= 8 &&
           length <= 128;
}


static int valid_name(
    const char *name
)
{
    if (name == NULL)
    {
        return 0;
    }

    size_t length = strlen(name);

    return length >= 2 &&
           length < 128;
}

static int valid_username(
    const char *username
)
{
    if (username == NULL)
    {
        return 0;
    }

    size_t length = strlen(username);

    if (length < 3 || length > 32)
    {
        return 0;
    }

    for (size_t i = 0; i < length; i++)
    {
        char character = username[i];

        if (!(
            (character >= 'a' && character <= 'z') ||
            (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9') ||
            character == '_' ||
            character == '.'
        ))
        {
            return 0;
        }
    }

    return 1;
}

static int valid_email(
    const char *email
)
{
    if (email == NULL)
    {
        return 0;
    }

    size_t length = strlen(email);

    if (length < 5 ||
        length >= 256)
    {
        return 0;
    }

    const char *at =
        strchr(email, '@');

    if (at == NULL ||
        at == email ||
        strchr(at + 1, '@') != NULL)
    {
        return 0;
    }

    const char *dot =
        strchr(at + 1, '.');

    return dot != NULL &&
           dot != at + 1 &&
           dot[1] != '\0';
}


/* =========================================================
   PASSWORD HASHING
   ========================================================= */

int auth_hash_password(
    const char *password,
    char *password_hash,
    int password_hash_size
)
{
    if (!valid_password(password) ||
        password_hash == NULL ||
        password_hash_size < AUTH_PASSWORD_HASH_STRING_SIZE)
    {
        return 0;
    }

    unsigned char salt[AUTH_SALT_SIZE];
    unsigned char derived_key[AUTH_HASH_SIZE];

    if (RAND_bytes(
            salt,
            AUTH_SALT_SIZE) != 1)
    {
        return 0;
    }

    if (PKCS5_PBKDF2_HMAC(
            password,
            (int)strlen(password),
            salt,
            AUTH_SALT_SIZE,
            PBKDF2_ITERATIONS,
            EVP_sha256(),
            AUTH_HASH_SIZE,
            derived_key) != 1)
    {
        return 0;
    }

    char salt_hex[
        HEX_SALT_LENGTH + 1
    ];

    char hash_hex[
        HEX_HASH_LENGTH + 1
    ];

    bytes_to_hex(
        salt,
        AUTH_SALT_SIZE,
        salt_hex
    );

    bytes_to_hex(
        derived_key,
        AUTH_HASH_SIZE,
        hash_hex
    );

    int written =
        snprintf(
            password_hash,
            password_hash_size,
            STORED_HASH_PREFIX "%d$%s$%s",
            PBKDF2_ITERATIONS,
            salt_hex,
            hash_hex
        );

    return written > 0 &&
           written < password_hash_size;
}


/* =========================================================
   PASSWORD VERIFICATION
   ========================================================= */

int auth_verify_password(
    const char *password,
    const char *stored_hash
)
{
    if (!valid_password(password) ||
        stored_hash == NULL)
    {
        return 0;
    }

    /*
     * Expected format:
     *
     * v1$600000$<32 hex salt>$<64 hex hash>
     */

    if (strncmp(
            stored_hash,
            STORED_HASH_PREFIX,
            strlen(STORED_HASH_PREFIX)) != 0)
    {
        return 0;
    }

    const char *iterations_start =
        stored_hash +
        strlen(STORED_HASH_PREFIX);

    char *iterations_end = NULL;

    long iterations =
        strtol(
            iterations_start,
            &iterations_end,
            10
        );

    if (iterations_end == iterations_start ||
        *iterations_end != '$' ||
        iterations < 100000 ||
        iterations > 2000000)
    {
        return 0;
    }

    const char *salt_start =
        iterations_end + 1;

    const char *salt_end =
        strchr(
            salt_start,
            '$'
        );

    if (salt_end == NULL)
    {
        return 0;
    }

    size_t salt_hex_length =
        (size_t)(
            salt_end - salt_start
        );

    if (salt_hex_length != HEX_SALT_LENGTH)
    {
        return 0;
    }

    const char *hash_start =
        salt_end + 1;

    if (strlen(hash_start) !=
        HEX_HASH_LENGTH)
    {
        return 0;
    }

    char salt_hex[
        HEX_SALT_LENGTH + 1
    ];

    memcpy(
        salt_hex,
        salt_start,
        HEX_SALT_LENGTH
    );

    salt_hex[HEX_SALT_LENGTH] =
        '\0';

    unsigned char salt[
        AUTH_SALT_SIZE
    ];

    unsigned char stored_key[
        AUTH_HASH_SIZE
    ];

    unsigned char calculated_key[
        AUTH_HASH_SIZE
    ];

    if (!hex_to_bytes(
            salt_hex,
            salt,
            AUTH_SALT_SIZE))
    {
        return 0;
    }

    if (!hex_to_bytes(
            hash_start,
            stored_key,
            AUTH_HASH_SIZE))
    {
        return 0;
    }

    if (PKCS5_PBKDF2_HMAC(
            password,
            (int)strlen(password),
            salt,
            AUTH_SALT_SIZE,
            (int)iterations,
            EVP_sha256(),
            AUTH_HASH_SIZE,
            calculated_key) != 1)
    {
        return 0;
    }

    /*
     * Constant-time comparison prevents
     * timing-based password verification leaks.
     */
    return CRYPTO_memcmp(
        calculated_key,
        stored_key,
        AUTH_HASH_SIZE
    ) == 0;
}


/* =========================================================
   SESSION TOKEN GENERATION
   ========================================================= */

static int generate_session_token(
    char *token,
    int token_size
)
{
    if (token == NULL ||
        token_size < AUTH_TOKEN_SIZE + 1)
    {
        return 0;
    }

    unsigned char random_bytes[
        SESSION_TOKEN_BYTES
    ];

    if (RAND_bytes(
            random_bytes,
            SESSION_TOKEN_BYTES) != 1)
    {
        return 0;
    }

    bytes_to_hex(
        random_bytes,
        SESSION_TOKEN_BYTES,
        token
    );

    return 1;
}


/* =========================================================
   REMOVE EXPIRED SESSIONS
   ========================================================= */

static void cleanup_expired_sessions(void)
{
    time_t current_time =
        time(NULL);

    SessionNode *current =
        session_head;

    SessionNode *previous =
        NULL;

    while (current != NULL)
    {
        SessionNode *next =
            current->next;

        if (current->expires_at <= current_time)
        {
            if (previous == NULL)
            {
                session_head = next;
            }
            else
            {
                previous->next = next;
            }

            free(current);
        }
        else
        {
            previous = current;
        }

        current = next;
    }
}

/* =========================================================
   CREATE SESSION
   ========================================================= */

int auth_create_session(
    int user_id,
    char *token,
    int token_size
)
{
    if (user_id <= 0 ||
        token == NULL ||
        token_size < AUTH_TOKEN_SIZE + 1)
    {
        return 0;
    }

    cleanup_expired_sessions();

    SessionNode *session =
        (SessionNode *)malloc(
            sizeof(SessionNode)
        );

    if (session == NULL)
    {
        return 0;
    }

    memset(
        session,
        0,
        sizeof(SessionNode)
    );

    if (!generate_session_token(
            session->token,
            sizeof(session->token)))
    {
        free(session);
        return 0;
    }

    session->user_id =
        user_id;

    session->created_at =
        time(NULL);

    session->expires_at =
        session->created_at +
        SESSION_DURATION_SECONDS;

    session->next =
        session_head;

    session_head =
        session;

    snprintf(
        token,
        token_size,
        "%s",
        session->token
    );

    return 1;
}


/* =========================================================
   VALIDATE SESSION
   ========================================================= */

int auth_validate_session(
    const char *token,
    int *user_id
)
{
    if (token == NULL ||
        user_id == NULL ||
        strlen(token) != AUTH_TOKEN_SIZE)
    {
        return 0;
    }

    cleanup_expired_sessions();

    SessionNode *current =
        session_head;

    while (current != NULL)
    {
        if (CRYPTO_memcmp(
                current->token,
                token,
                AUTH_TOKEN_SIZE) == 0)
        {
            *user_id =
                current->user_id;

            return 1;
        }

        current =
            current->next;
    }

    return 0;
}


/* =========================================================
   DESTROY SESSION
   ========================================================= */

int auth_destroy_session(
    const char *token
)
{
    if (token == NULL ||
        strlen(token) != AUTH_TOKEN_SIZE)
    {
        return 0;
    }

    SessionNode *current =
        session_head;

    SessionNode *previous =
        NULL;

    while (current != NULL)
    {
        if (CRYPTO_memcmp(
                current->token,
                token,
                AUTH_TOKEN_SIZE) == 0)
        {
            if (previous == NULL)
            {
                session_head =
                    current->next;
            }
            else
            {
                previous->next =
                    current->next;
            }

            /*
             * Clear the token before freeing
             * the session memory.
             */
            memset(
                current->token,
                0,
                sizeof(current->token)
            );

            free(current);

            return 1;
        }

        previous = current;
        current = current->next;
    }

    return 0;
}

void auth_destroy_user_sessions(int user_id)
{
    if (user_id <= 0)
    {
        return;
    }

    SessionNode *current = session_head;
    SessionNode *previous = NULL;

    while (current != NULL)
    {
        SessionNode *next = current->next;

        if (current->user_id == user_id)
        {
            if (previous == NULL)
            {
                session_head = next;
            }
            else
            {
                previous->next = next;
            }

            memset(current->token, 0, sizeof(current->token));
            free(current);
        }
        else
        {
            previous = current;
        }

        current = next;
    }
}

/* =========================================================
   SIGNUP
   ========================================================= */

AuthResult auth_signup(
    Database *database,
    const char *name,
    const char *username,
    const char *email,
    const char *password
)
{
    AuthResult result = {0};

    if (database == NULL ||
        !valid_name(name) ||
        !valid_username(username) ||
        !valid_email(email) ||
        !valid_password(password))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Invalid registration details."
        );

        return result;
    }

    int existing_user_id = 0;

    char existing_name[128];

    char existing_hash[256];

    if (database_find_user_by_email(
            database,
            email,
            &existing_user_id,
            existing_name,
            sizeof(existing_name),
            existing_hash,
            sizeof(existing_hash)))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "An account with this email already exists."
        );

        return result;
    }

    char password_hash[
        AUTH_PASSWORD_HASH_STRING_SIZE
    ];

    if (!auth_hash_password(
            password,
            password_hash,
            sizeof(password_hash)))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Password hashing failed."
        );

        return result;
    }

    int user_id = 0;

    if (!database_create_user(
            database,
            name,
            username,
            email,
            password_hash,
            &user_id))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Unable to create account."
        );

        return result;
    }

    result.success = 1;

    result.user_id = user_id;

    snprintf(
        result.name,
        sizeof(result.name),
        "%s",
        name
    );

    snprintf(
        result.message,
        sizeof(result.message),
        "Account created successfully."
    );

    return result;
}

/* =========================================================
   LOGIN
   ========================================================= */

AuthResult auth_login(
    Database *database,
    const char *email,
    const char *password
)
{
    AuthResult result = {0};

    if (database == NULL ||
        !valid_email(email) ||
        !valid_password(password))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Invalid email or password."
        );

        return result;
    }

    int user_id = 0;

    char name[128];

    char stored_hash[256];

    if (!database_find_user_by_email(
            database,
            email,
            &user_id,
            name,
            sizeof(name),
            stored_hash,
            sizeof(stored_hash)))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Invalid email or password."
        );

        return result;
    }

    if (!auth_verify_password(
            password,
            stored_hash))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Invalid email or password."
        );

        return result;
    }

    /*
     * Password is correct.
     * Create an authenticated session.
     */

    database_update_last_login(
    database,
    user_id
);
    if (!auth_create_session(
            user_id,
            result.token,
            sizeof(result.token)))
    {
        snprintf(
            result.message,
            sizeof(result.message),
            "Unable to create authentication session."
        );

        return result;
    }

    result.success = 1;

    result.user_id =
        user_id;

    snprintf(
        result.name,
        sizeof(result.name),
        "%s",
        name
    );

    snprintf(
        result.message,
        sizeof(result.message),
        "Login successful."
    );

    return result;
}

int auth_change_password(
    Database *database,
    int user_id,
    const char *current_password,
    const char *new_password)
{
    if (database == NULL ||
        current_password == NULL ||
        new_password == NULL ||
        current_password[0] == '\0' ||
        new_password[0] == '\0')
    {
        return 0;
    }

    char stored_hash[AUTH_PASSWORD_HASH_STRING_SIZE];

    memset(
        stored_hash,
        0,
        sizeof(stored_hash)
    );

    if (!database_get_user_password_hash(
            database,
            user_id,
            stored_hash,
            sizeof(stored_hash)))
    {
        return 0;
    }

    if (!auth_verify_password(
            current_password,
            stored_hash))
    {
        return 0;
    }

    char new_password_hash[AUTH_PASSWORD_HASH_STRING_SIZE];

    memset(
        new_password_hash,
        0,
        sizeof(new_password_hash)
    );

    if (!auth_hash_password(
            new_password,
            new_password_hash,
            sizeof(new_password_hash)))
    {
        return 0;
    }

    if (!database_update_user_password(
            database,
            user_id,
            new_password_hash))
    {
        return 0;
    }

    return 1;
}
