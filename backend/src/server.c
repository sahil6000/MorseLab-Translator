#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <microhttpd.h>

#include <limits.h>

#include "server.h"
#include "auth.h"

#define MAX_REQUEST_BODY 8192

typedef struct RequestData
{
    char body[MAX_REQUEST_BODY];
    size_t body_length;
} RequestData;


/* =========================================================
   JSON RESPONSE
   ========================================================= */

static enum MHD_Result send_json_response(
    struct MHD_Connection *connection,
    unsigned int status_code,
    const char *json)
{
    size_t length = strlen(json);

    struct MHD_Response *response =
        MHD_create_response_from_buffer(
            length,
            (void *)json,
            MHD_RESPMEM_MUST_COPY
        );

    if (response == NULL)
    {
        return MHD_NO;
    }

    MHD_add_response_header(
        response,
        "Content-Type",
        "application/json"
    );

    MHD_add_response_header(
        response,
        "Access-Control-Allow-Origin",
        "*"
    );

    MHD_add_response_header(
        response,
        "Access-Control-Allow-Headers",
        "Content-Type"
    );

    MHD_add_response_header(
        response,
        "Access-Control-Allow-Methods",
        "GET, POST, DELETE, OPTIONS"
    );

    enum MHD_Result result =
        MHD_queue_response(
            connection,
            status_code,
            response
        );

    MHD_destroy_response(response);

    return result;
}


/* =========================================================
   HEALTH CHECK
   ========================================================= */

static enum MHD_Result handle_health(
    struct MHD_Connection *connection)
{
    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"status\":\"ok\",\"service\":\"MorseLab C Backend\"}"
    );
}


/* =========================================================
   SIMPLE JSON TEXT EXTRACTION
   ========================================================= */

static int extract_json_text(
    const char *body,
    char *output,
    size_t output_size)
{
    if (body == NULL ||
        output == NULL ||
        output_size == 0)
    {
        return 0;
    }

    const char *key = "\"text\"";

    const char *key_position =
        strstr(body, key);

    if (key_position == NULL)
    {
        return 0;
    }

    const char *colon =
        strchr(
            key_position + strlen(key),
            ':'
        );

    if (colon == NULL)
    {
        return 0;
    }

    const char *quote =
        strchr(colon + 1, '"');

    if (quote == NULL)
    {
        return 0;
    }

    quote++;

    size_t position = 0;

    while (*quote != '\0' &&
           *quote != '"' &&
           position < output_size - 1)
    {
        if (*quote == '\\' &&
            *(quote + 1) != '\0')
        {
            quote++;

            if (*quote == 'n')
            {
                output[position++] = '\n';
            }
            else if (*quote == 'r')
            {
                output[position++] = '\r';
            }
            else if (*quote == 't')
            {
                output[position++] = '\t';
            }
            else
            {
                output[position++] = *quote;
            }
        }
        else
        {
            output[position++] = *quote;
        }

        quote++;
    }

    if (*quote != '"')
    {
        return 0;
    }

    output[position] = '\0';

    return 1;
}

/* =========================================================
   GENERIC JSON STRING EXTRACTION
   ========================================================= */

static int extract_json_string(
    const char *body,
    const char *key,
    char *output,
    size_t output_size)
{
    if (body == NULL ||
        key == NULL ||
        output == NULL ||
        output_size == 0)
    {
        return 0;
    }

    char search_key[128];

    int written = snprintf(
        search_key,
        sizeof(search_key),
        "\"%s\"",
        key
    );

    if (written < 0 ||
        (size_t)written >= sizeof(search_key))
    {
        return 0;
    }

    const char *key_position =
        strstr(body, search_key);

    if (key_position == NULL)
    {
        return 0;
    }

    const char *colon =
        strchr(key_position + written, ':');

    if (colon == NULL)
    {
        return 0;
    }

    const char *value_start =
        colon + 1;

    while (*value_start != '\0' &&
           isspace((unsigned char)*value_start))
    {
        value_start++;
    }

    if (*value_start != '"')
    {
        return 0;
    }

    value_start++;

    size_t position = 0;

    while (*value_start != '\0')
{
    if (*value_start == '"')
    {
        output[position] = '\0';
        return 1;
    }

    if (position >= output_size - 1)
    {
        return 0;
    }

    if (*value_start == '\\' &&
        value_start[1] != '\0')
    {
        value_start++;

        switch (*value_start)
        {
            case '"':
                output[position++] = '"';
                break;

            case '\\':
                output[position++] = '\\';
                break;

            case 'n':
                output[position++] = '\n';
                break;

            case 'r':
                output[position++] = '\r';
                break;

            case 't':
                output[position++] = '\t';
                break;

            default:
                output[position++] = *value_start;
                break;
        }

        value_start++;
    }
    else
    {
        output[position++] = *value_start;
        value_start++;
    }
}

    output[position] = '\0';

    return 0;
}

/* =========================================================
   GENERIC JSON INTEGER EXTRACTION
   ========================================================= */

static int extract_json_int(
    const char *body,
    const char *key,
    int *output)
{
    if (body == NULL ||
        key == NULL ||
        output == NULL)
    {
        return 0;
    }

    char search_key[128];

    int written = snprintf(
        search_key,
        sizeof(search_key),
        "\"%s\"",
        key
    );

    if (written < 0 ||
        (size_t)written >= sizeof(search_key))
    {
        return 0;
    }

    const char *key_position =
        strstr(body, search_key);

    if (key_position == NULL)
    {
        return 0;
    }

    const char *colon =
        strchr(key_position + written, ':');

    if (colon == NULL)
    {
        return 0;
    }

    const char *value_start =
        colon + 1;

    while (*value_start != '\0' &&
           isspace((unsigned char)*value_start))
    {
        value_start++;
    }

    char *end_pointer = NULL;

    long value =
        strtol(
            value_start,
            &end_pointer,
            10
        );

    if (value_start == end_pointer)
    {
        return 0;
    }

    while (*end_pointer != '\0' &&
           isspace((unsigned char)*end_pointer))
    {
        end_pointer++;
    }

    if (*end_pointer != '\0' &&
        *end_pointer != ',' &&
        *end_pointer != '}')
    {
        return 0;
    }

    if (value <= 0 ||
        value > INT_MAX)
    {
        return 0;
    }

    *output = (int)value;

    return 1;
}

static int extract_json_int_zero_or_one(
    const char *body,
    const char *key,
    int *output)
{
    if (body == NULL || key == NULL || output == NULL)
    {
        return 0;
    }

    char search_key[128];
    int written = snprintf(search_key, sizeof(search_key), "\"%s\"", key);
    if (written < 0 || (size_t)written >= sizeof(search_key))
    {
        return 0;
    }

    const char *key_position = strstr(body, search_key);
    if (key_position == NULL)
    {
        return 0;
    }

    const char *colon = strchr(key_position + written, ':');
    if (colon == NULL)
    {
        return 0;
    }

    const char *value_start = colon + 1;
    while (*value_start != '\0' && isspace((unsigned char)*value_start))
    {
        value_start++;
    }

    char *end_pointer = NULL;
    long value = strtol(value_start, &end_pointer, 10);
    if (value_start == end_pointer || (value != 0 && value != 1))
    {
        return 0;
    }

    while (*end_pointer != '\0' && isspace((unsigned char)*end_pointer))
    {
        end_pointer++;
    }

    if (*end_pointer != ',' && *end_pointer != '}')
    {
        return 0;
    }

    *output = (int)value;
    return 1;
}

/* =========================================================
   JSON ESCAPE
   ========================================================= */

static void json_escape(
    const char *input,
    char *output,
    size_t output_size)
{
    size_t position = 0;

    if (input == NULL ||
        output == NULL ||
        output_size == 0)
    {
        return;
    }

    for (size_t i = 0;
         input[i] != '\0' &&
         position < output_size - 1;
         i++)
    {
        char character = input[i];

        if (character == '"' ||
            character == '\\')
        {
            if (position + 2 >= output_size)
            {
                break;
            }

            output[position++] = '\\';
            output[position++] = character;
        }
        else if (character == '\n')
        {
            if (position + 2 >= output_size)
            {
                break;
            }

            output[position++] = '\\';
            output[position++] = 'n';
        }
        else if (character == '\r')
        {
            if (position + 2 >= output_size)
            {
                break;
            }

            output[position++] = '\\';
            output[position++] = 'r';
        }
        else if (character == '\t')
        {
            if (position + 2 >= output_size)
            {
                break;
            }

            output[position++] = '\\';
            output[position++] = 't';
        }
        else
        {
            output[position++] = character;
        }
    }

    output[position] = '\0';
}


/* =========================================================
   AUTHENTICATION HANDLER
   ========================================================= */

static enum MHD_Result handle_authentication(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *url,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"error\":\"Authentication service unavailable\"}"
        );
    }

    char name[128];
    char email[256];
    char password[129];

    memset(name, 0, sizeof(name));
    memset(email, 0, sizeof(email));
    memset(password, 0, sizeof(password));

    /* =====================================================
       SIGNUP
       ===================================================== */

    if (strcmp(url, "/api/auth/signup") == 0)
    {
        if (!extract_json_string(
                body,
                "name",
                name,
                sizeof(name)) ||
            !extract_json_string(
                body,
                "email",
                email,
                sizeof(email)) ||
            !extract_json_string(
                body,
                "password",
                password,
                sizeof(password)))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"error\":\"Invalid request. Expected name, email and password.\"}"
            );
        }

        char username[128];

        memset(username, 0, sizeof(username));

        if (!extract_json_string(
                body,
                "username",
                username,
                sizeof(username)))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"error\":\"Invalid request. Expected name, username, email and password.\"}"
            );
        }

        AuthResult result =
            auth_signup(
                context->database,
                name,
                username,
                email,
                password
            );

        if (!result.success)
        {
            char escaped_message[512];

            json_escape(
                result.message,
                escaped_message,
                sizeof(escaped_message)
            );

            char response[768];

            int written =
                snprintf(
                    response,
                    sizeof(response),
                    "{\"success\":false,\"error\":\"%s\"}",
                    escaped_message
                );

            if (written < 0 ||
                (size_t)written >= sizeof(response))
            {
                return send_json_response(
                    connection,
                    MHD_HTTP_INTERNAL_SERVER_ERROR,
                    "{\"error\":\"Response generation failed\"}"
                );
            }

            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                response
            );
        }

        char escaped_name[256];

        json_escape(
            result.name,
            escaped_name,
            sizeof(escaped_name)
        );

        char response[768];

        int written =
            snprintf(
                response,
                sizeof(response),
                "{\"success\":true,\"user\":{\"id\":%d,\"name\":\"%s\"},\"message\":\"%s\"}",
                result.user_id,
                escaped_name,
                result.message
            );

        if (written < 0 ||
            (size_t)written >= sizeof(response))
        {
            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"error\":\"Response generation failed\"}"
            );
        }

        return send_json_response(
            connection,
            MHD_HTTP_CREATED,
            response
        );
    }

    /* =====================================================
       LOGIN
       ===================================================== */

    if (strcmp(url, "/api/auth/login") == 0)
    {
        if (!extract_json_string(
                body,
                "email",
                email,
                sizeof(email)) ||
            !extract_json_string(
                body,
                "password",
                password,
                sizeof(password)))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"error\":\"Invalid request. Expected email and password.\"}"
            );
        }

        AuthResult result =
            auth_login(
                context->database,
                email,
                password
            );

        if (!result.success)
        {
            return send_json_response(
                connection,
                MHD_HTTP_UNAUTHORIZED,
                "{\"success\":false,\"error\":\"Invalid email or password.\"}"
            );
        }

        char escaped_name[256];

        json_escape(
            result.name,
            escaped_name,
            sizeof(escaped_name)
        );

        char response[768];

        int written =
            snprintf(
                response,
                sizeof(response),
                "{\"success\":true,\"user\":{\"id\":%d,\"name\":\"%s\"},\"token\":\"%s\",\"message\":\"%s\"}",
                result.user_id,
                escaped_name,
                result.token,
                result.message
            );

        if (written < 0 ||
            (size_t)written >= sizeof(response))
        {
            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"error\":\"Response generation failed\"}"
            );
        }

        return send_json_response(
            connection,
            MHD_HTTP_OK,
            response
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_NOT_FOUND,
        "{\"error\":\"Authentication endpoint not found\"}"
    );
}

/* =========================================================
   SESSION VALIDATION HANDLER
   ========================================================= */

static enum MHD_Result handle_session_validation(
    struct MHD_Connection *connection,
    const char *body)
{
    if (connection == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Session validation service unavailable\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(
        token,
        0,
        sizeof(token)
    );

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    char response[256];

    int written =
        snprintf(
            response,
            sizeof(response),
            "{\"success\":true,\"authenticated\":true,\"user_id\":%d}",
            user_id
        );

    if (written < 0 ||
        (size_t)written >= sizeof(response))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Response generation failed\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        response
    );
}

/* =========================================================
   LOGOUT HANDLER
   ========================================================= */

static enum MHD_Result handle_logout(
    struct MHD_Connection *connection,
    const char *body)
{
    if (connection == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Logout service unavailable\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(token, 0, sizeof(token));

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Token is required.\"}"
        );
    }

    int user_id = 0;

if (!auth_validate_session(token, &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    if (!auth_destroy_session(token))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Failed to terminate session.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"success\":true,\"message\":\"Logout successful.\"}"
    );
}

/* =========================================================
   CHANGE PASSWORD HANDLER
   ========================================================= */

static enum MHD_Result handle_change_password(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Password service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];
    char current_password[129];
    char new_password[129];

    memset(token, 0, sizeof(token));
    memset(current_password, 0, sizeof(current_password));
    memset(new_password, 0, sizeof(new_password));

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)) ||
        !extract_json_string(
            body,
            "current_password",
            current_password,
            sizeof(current_password)) ||
        !extract_json_string(
            body,
            "new_password",
            new_password,
            sizeof(new_password)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Token, current password and new password are required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    if (!auth_change_password(
            context->database,
            user_id,
            current_password,
            new_password))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Current password is incorrect or the new password is invalid.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"success\":true,\"message\":\"Password changed successfully.\"}"
    );
}

/* =========================================================
   TRANSLATION HANDLER
   ========================================================= */

static enum MHD_Result handle_translation(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *url,
    const char *body)
{
    if (context == NULL ||
        context->translator == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"error\":\"Translation service unavailable\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(token, 0, sizeof(token));

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(token, &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    char input[MAX_REQUEST_BODY];

    if (!extract_json_text(
            body,
            input,
            sizeof(input)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"error\":\"Invalid request. Expected JSON with a text field.\"}"
        );
    }

    char *translation = NULL;

    const char *input_type = NULL;
    const char *output_type = NULL;

    if (strcmp(
            url,
            "/api/translate/text-to-morse") == 0)
    {
        translation =
            morse_encode_text(
                context->translator,
                input
            );

        input_type = "TEXT_TO_MORSE";
        output_type = "MORSE";
    }
    else if (strcmp(
                 url,
                 "/api/translate/morse-to-text") == 0)
    {
        translation =
            morse_decode_text(
                context->translator,
                input
            );

        input_type = "MORSE_TO_TEXT";
        output_type = "TEXT";
    }
    else
    {
        return send_json_response(
            connection,
            MHD_HTTP_NOT_FOUND,
            "{\"error\":\"Endpoint not found\"}"
        );
    }

    if (translation == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"error\":\"Translation failed. Check the input format.\"}"
        );
    }

    /* =====================================================
   SAVE TRANSLATION TO HISTORY
   ===================================================== */

int history_id = 0;

if (database_add_history(
        context->database,
        user_id,
        input,
        translation,
        input_type,
        &history_id) == 0)
{
    free(translation);

    return send_json_response(
        connection,
        MHD_HTTP_INTERNAL_SERVER_ERROR,
        "{\"success\":false,\"error\":\"Failed to save translation history.\"}"
    );
}

(void)output_type;

    /* =====================================================
       DYNAMIC JSON ESCAPED TRANSLATION BUFFER
       ===================================================== */

    size_t translation_length =
        strlen(translation);

    /*
     * Worst case:
     * Every character could require two characters
     * after JSON escaping.
     */
    size_t escaped_capacity =
        (translation_length * 2) + 1;

    char *escaped_translation =
        (char *)malloc(escaped_capacity);

    if (escaped_translation == NULL)
    {
        free(translation);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"error\":\"Memory allocation failed\"}"
        );
    }

    json_escape(
        translation,
        escaped_translation,
        escaped_capacity
    );


    /* =====================================================
       DYNAMIC JSON RESPONSE BUFFER
       ===================================================== */

    size_t input_length =
        strlen(input);

    size_t escaped_input_capacity =
        (input_length * 2) + 1;

    char *escaped_input =
        (char *)malloc(escaped_input_capacity);

    if (escaped_input == NULL)
{
    free(escaped_translation);

    return send_json_response(
        connection,
        MHD_HTTP_INTERNAL_SERVER_ERROR,
        "{\"error\":\"Memory allocation failed\"}"
    );
}

json_escape(
    input,
    escaped_input,
    escaped_input_capacity
);

    size_t escaped_length =
        strlen(escaped_translation);

    /*
     * Extra space for:
     * JSON property names
     * quotes
     * commas
     * braces
     * null terminator
     */
size_t escaped_input_length =
    strlen(escaped_input);

size_t response_capacity =
    escaped_input_length +
    escaped_length +
    64;

    char *response_json =
        (char *)malloc(response_capacity);

    if (response_json == NULL)
    {
        free(escaped_translation);
        free(translation);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"error\":\"Memory allocation failed\"}"
        );
    }


    /* =====================================================
       BUILD JSON RESPONSE
       ===================================================== */

    int written =
        snprintf(
            response_json,
            response_capacity,
            "{\"success\":true,\"input\":\"%s\",\"output\":\"%s\"}",
            escaped_input,
            escaped_translation
        );

    if (written < 0 ||
        (size_t)written >= response_capacity)
    {
        free(response_json);
        free(escaped_input);
        free(escaped_translation);
        free(translation);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"error\":\"Failed to create response\"}"
        );
    }

    /* =====================================================
       SEND RESPONSE
       ===================================================== */

    enum MHD_Result result =
        send_json_response(
            connection,
            MHD_HTTP_OK,
            response_json
        );


    /* =====================================================
       MEMORY CLEANUP
       ===================================================== */

    free(response_json);
    free(escaped_input);
    free(escaped_translation);
    free(translation);

    return result;
}

/* =========================================================
   GET TRANSLATION HISTORY
   ========================================================= */

static enum MHD_Result handle_history(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"History service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(
        token,
        0,
        sizeof(token)
    );

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    char **inputs = NULL;
    char **outputs = NULL;
    char **input_types = NULL;
    char **created_ats = NULL;

    int *history_ids = NULL;
    int count = 0;

    if (!database_get_history(
            context->database,
            user_id,
            &inputs,
            &outputs,
            &input_types,
            &created_ats,
            &history_ids,
            &count))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Failed to retrieve translation history.\"}"
        );
    }

    /*
     * Empty history is still a successful response.
     */
    if (count == 0)
    {
        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(history_ids);

        return send_json_response(
            connection,
            MHD_HTTP_OK,
            "{\"success\":true,\"history\":[]}"
        );
    }

    /*
     * Calculate enough memory for the JSON response.
     * JSON escaping can require up to approximately
     * twice the original string length.
     */
    size_t response_capacity = 64;

    for (int i = 0; i < count; i++)
    {
        response_capacity +=
            (strlen(inputs[i]) * 2) +
            (strlen(outputs[i]) * 2) +
            (strlen(input_types[i]) * 2) +
            (strlen(created_ats[i]) * 2) +
            128;
    }

    char *response =
        (char *)malloc(response_capacity);

    if (response == NULL)
    {
        for (int i = 0; i < count; i++)
        {
            free(inputs[i]);
            free(outputs[i]);
            free(input_types[i]);
            free(created_ats[i]);
        }

        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(history_ids);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Memory allocation failed.\"}"
        );
    }

    size_t offset = 0;

    int written =
        snprintf(
            response + offset,
            response_capacity - offset,
            "{\"success\":true,\"history\":["
        );

    if (written < 0)
    {
        free(response);

        for (int i = 0; i < count; i++)
        {
            free(inputs[i]);
            free(outputs[i]);
            free(input_types[i]);
            free(created_ats[i]);
        }

        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(history_ids);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Response generation failed.\"}"
        );
    }

    offset += (size_t)written;

    for (int i = 0; i < count; i++)
    {
        size_t input_capacity =
            (strlen(inputs[i]) * 2) + 1;

        size_t output_capacity =
            (strlen(outputs[i]) * 2) + 1;

        size_t type_capacity =
            (strlen(input_types[i]) * 2) + 1;

        size_t date_capacity =
            (strlen(created_ats[i]) * 2) + 1;

        char *escaped_input =
            (char *)malloc(input_capacity);

        char *escaped_output =
            (char *)malloc(output_capacity);

        char *escaped_type =
            (char *)malloc(type_capacity);

        char *escaped_date =
            (char *)malloc(date_capacity);

        if (escaped_input == NULL ||
            escaped_output == NULL ||
            escaped_type == NULL ||
            escaped_date == NULL)
        {
            free(escaped_input);
            free(escaped_output);
            free(escaped_type);
            free(escaped_date);
            free(response);

            for (int j = 0; j < count; j++)
            {
                free(inputs[j]);
                free(outputs[j]);
                free(input_types[j]);
                free(created_ats[j]);
            }

            free(inputs);
            free(outputs);
            free(input_types);
            free(created_ats);
            free(history_ids);

            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"success\":false,\"error\":\"Memory allocation failed.\"}"
            );
        }

        json_escape(
            inputs[i],
            escaped_input,
            input_capacity
        );

        json_escape(
            outputs[i],
            escaped_output,
            output_capacity
        );

        json_escape(
            input_types[i],
            escaped_type,
            type_capacity
        );

        json_escape(
            created_ats[i],
            escaped_date,
            date_capacity
        );

        written =
            snprintf(
                response + offset,
                response_capacity - offset,
                "%s{\"id\":%d,\"input\":\"%s\",\"output\":\"%s\",\"input_type\":\"%s\",\"created_at\":\"%s\"}",
                i == 0 ? "" : ",",
                history_ids[i],
                escaped_input,
                escaped_output,
                escaped_type,
                escaped_date
            );

        free(escaped_input);
        free(escaped_output);
        free(escaped_type);
        free(escaped_date);

        if (written < 0 ||
            (size_t)written >= response_capacity - offset)
        {
            free(response);

            for (int j = 0; j < count; j++)
            {
                free(inputs[j]);
                free(outputs[j]);
                free(input_types[j]);
                free(created_ats[j]);
            }

            free(inputs);
            free(outputs);
            free(input_types);
            free(created_ats);
            free(history_ids);

            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"success\":false,\"error\":\"Response generation failed.\"}"
            );
        }

        offset += (size_t)written;
    }

    written =
        snprintf(
            response + offset,
            response_capacity - offset,
            "]}"
        );

    if (written < 0 ||
        (size_t)written >= response_capacity - offset)
    {
        free(response);

        for (int i = 0; i < count; i++)
        {
            free(inputs[i]);
            free(outputs[i]);
            free(input_types[i]);
            free(created_ats[i]);
        }

        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(history_ids);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Response generation failed.\"}"
        );
    }

    for (int i = 0; i < count; i++)
    {
        free(inputs[i]);
        free(outputs[i]);
        free(input_types[i]);
        free(created_ats[i]);
    }

    free(inputs);
    free(outputs);
    free(input_types);
    free(created_ats);
    free(history_ids);

    enum MHD_Result result =
        send_json_response(
            connection,
            MHD_HTTP_OK,
            response
        );

    free(response);

    return result;
}

/* =========================================================
   DELETE TRANSLATION HISTORY
   ========================================================= */

static enum MHD_Result handle_history_delete(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"History service unavailable.\"}"
        );
    }

    /* -----------------------------------------------------
       Extract authentication token
       ----------------------------------------------------- */

    char token[AUTH_TOKEN_SIZE + 1];

    memset(
        token,
        0,
        sizeof(token)
    );

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate session
       ----------------------------------------------------- */

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    /* -----------------------------------------------------
       Extract history ID
       ----------------------------------------------------- */

    int history_id = 0;

    if (!extract_json_int(
            body,
            "history_id",
            &history_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Valid history_id is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Delete history record
       ----------------------------------------------------- */

    if (!database_delete_history(
            context->database,
            user_id,
            history_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_NOT_FOUND,
            "{\"success\":false,\"error\":\"History record not found.\"}"
        );
    }

    /* -----------------------------------------------------
       Successful deletion
       ----------------------------------------------------- */

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"success\":true,\"message\":\"History record deleted successfully.\"}"
    );
}

/* =========================================================
   SAVED TRANSLATIONS
   ========================================================= */

/* ---------------------------------------------------------
   SAVE TRANSLATION
   --------------------------------------------------------- */

static enum MHD_Result handle_saved_create(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Saved translation service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];
    char title[256];
    char input_text[MAX_REQUEST_BODY];
    char output_text[MAX_REQUEST_BODY];
    char input_type[64];

    memset(token, 0, sizeof(token));
    memset(title, 0, sizeof(title));
    memset(input_text, 0, sizeof(input_text));
    memset(output_text, 0, sizeof(output_text));
    memset(input_type, 0, sizeof(input_type));

    /* -----------------------------------------------------
       Extract authentication token
       ----------------------------------------------------- */

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate session
       ----------------------------------------------------- */

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    /* -----------------------------------------------------
       Extract saved translation fields
       ----------------------------------------------------- */

    if (!extract_json_string(
            body,
            "title",
            title,
            sizeof(title)) ||
        !extract_json_string(
            body,
            "input",
            input_text,
            sizeof(input_text)) ||
        !extract_json_string(
            body,
            "output",
            output_text,
            sizeof(output_text)) ||
        !extract_json_string(
            body,
            "input_type",
            input_type,
            sizeof(input_type)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Title, input, output and input_type are required.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate title
       ----------------------------------------------------- */

    if (title[0] == '\0')
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Title cannot be empty.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate translation direction
       ----------------------------------------------------- */

    if (strcmp(input_type, "TEXT_TO_MORSE") != 0 &&
        strcmp(input_type, "MORSE_TO_TEXT") != 0)
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Invalid translation direction.\"}"
        );
    }

    /* -----------------------------------------------------
       Save translation in database
       ----------------------------------------------------- */

    int saved_id = 0;

    if (!database_save_translation(
            context->database,
            user_id,
            title,
            input_text,
            output_text,
            input_type,
            &saved_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Failed to save translation.\"}"
        );
    }

    /* -----------------------------------------------------
       Successful response
       ----------------------------------------------------- */

    char response[512];

    int written = snprintf(
        response,
        sizeof(response),
        "{\"success\":true,\"saved_id\":%d,\"message\":\"Translation saved successfully.\"}",
        saved_id
    );

    if (written < 0 ||
        (size_t)written >= sizeof(response))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Response generation failed.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_CREATED,
        response
    );
}


/* ---------------------------------------------------------
   GET SAVED TRANSLATIONS
   --------------------------------------------------------- */

static enum MHD_Result handle_saved_list(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Saved translation service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(token, 0, sizeof(token));

    /* -----------------------------------------------------
       Extract authentication token
       ----------------------------------------------------- */

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate session
       ----------------------------------------------------- */

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    /* -----------------------------------------------------
       Retrieve saved translations
       ----------------------------------------------------- */

    char **titles = NULL;
    char **inputs = NULL;
    char **outputs = NULL;
    char **input_types = NULL;
    char **created_ats = NULL;
    char **updated_ats = NULL;
    int *saved_ids = NULL;
    int count = 0;

    if (!database_get_saved_translations(
            context->database,
            user_id,
            &titles,
            &inputs,
            &outputs,
            &input_types,
            &created_ats,
            &updated_ats,
            &saved_ids,
            &count))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Failed to retrieve saved translations.\"}"
        );
    }

    /* -----------------------------------------------------
       Empty saved list
       ----------------------------------------------------- */

    if (count == 0)
    {
        free(titles);
        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(updated_ats);
        free(saved_ids);

        return send_json_response(
            connection,
            MHD_HTTP_OK,
            "{\"success\":true,\"saved\":[]}"
        );
    }

    /* -----------------------------------------------------
       Calculate response size
       ----------------------------------------------------- */

    size_t response_capacity = 128;

    for (int i = 0; i < count; i++)
    {
        response_capacity +=
            (strlen(titles[i]) * 2) +
            (strlen(inputs[i]) * 2) +
            (strlen(outputs[i]) * 2) +
            (strlen(input_types[i]) * 2) +
            (strlen(created_ats[i]) * 2) +
            (strlen(updated_ats[i]) * 2) +
            160;
    }

    char *response =
        (char *)malloc(response_capacity);

    if (response == NULL)
    {
        for (int i = 0; i < count; i++)
        {
            free(titles[i]);
            free(inputs[i]);
            free(outputs[i]);
            free(input_types[i]);
            free(created_ats[i]);
            free(updated_ats[i]);
        }

        free(titles);
        free(inputs);
        free(outputs);
        free(input_types);
        free(created_ats);
        free(updated_ats);
        free(saved_ids);

        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Memory allocation failed.\"}"
        );
    }

    size_t position = 0;

    position += snprintf(
        response + position,
        response_capacity - position,
        "{\"success\":true,\"saved\":["
    );

    for (int i = 0; i < count; i++)
    {
        char escaped_title[MAX_REQUEST_BODY * 2];
        char escaped_input[MAX_REQUEST_BODY * 2];
        char escaped_output[MAX_REQUEST_BODY * 2];
        char escaped_type[128];
        char escaped_created[128];
        char escaped_updated[128];

        json_escape(
            titles[i],
            escaped_title,
            sizeof(escaped_title)
        );

        json_escape(
            inputs[i],
            escaped_input,
            sizeof(escaped_input)
        );

        json_escape(
            outputs[i],
            escaped_output,
            sizeof(escaped_output)
        );

        json_escape(
            input_types[i],
            escaped_type,
            sizeof(escaped_type)
        );

        json_escape(
            created_ats[i],
            escaped_created,
            sizeof(escaped_created)
        );

        json_escape(
            updated_ats[i],
            escaped_updated,
            sizeof(escaped_updated)
        );

        position += snprintf(
            response + position,
            response_capacity - position,
            "%s{\"id\":%d,\"title\":\"%s\",\"input\":\"%s\",\"output\":\"%s\",\"input_type\":\"%s\",\"created_at\":\"%s\",\"updated_at\":\"%s\"}",
            i == 0 ? "" : ",",
            saved_ids[i],
            escaped_title,
            escaped_input,
            escaped_output,
            escaped_type,
            escaped_created,
            escaped_updated
        );
    }

    position += snprintf(
        response + position,
        response_capacity - position,
        "]}"
    );

    for (int i = 0; i < count; i++)
    {
        free(titles[i]);
        free(inputs[i]);
        free(outputs[i]);
        free(input_types[i]);
        free(created_ats[i]);
        free(updated_ats[i]);
    }

    free(titles);
    free(inputs);
    free(outputs);
    free(input_types);
    free(created_ats);
    free(updated_ats);
    free(saved_ids);

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        response
    );
}


/* ---------------------------------------------------------
   DELETE SAVED TRANSLATION
   --------------------------------------------------------- */

static enum MHD_Result handle_saved_delete(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Saved translation service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(token, 0, sizeof(token));

    /* -----------------------------------------------------
       Extract authentication token
       ----------------------------------------------------- */

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Validate session
       ----------------------------------------------------- */

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    /* -----------------------------------------------------
       Extract saved ID
       ----------------------------------------------------- */

    int saved_id = 0;

    if (!extract_json_int(
            body,
            "saved_id",
            &saved_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Valid saved_id is required.\"}"
        );
    }

    /* -----------------------------------------------------
       Delete saved translation
       ----------------------------------------------------- */

    if (!database_delete_saved_translation(
            context->database,
            user_id,
            saved_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_NOT_FOUND,
            "{\"success\":false,\"error\":\"Saved translation not found.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"success\":true,\"message\":\"Saved translation deleted successfully.\"}"
    );
}

static enum MHD_Result handle_statistics(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    char token[AUTH_TOKEN_SIZE + 1];

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(token, &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    int total_translations = 0;
    int saved_translations = 0;
    int text_to_morse = 0;
    int morse_to_text = 0;
    int weekly_translations = 0;

    if (!database_get_user_statistics(
            context->database,
            user_id,
            &total_translations,
            &saved_translations,
            &text_to_morse,
            &morse_to_text))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Unable to load statistics.\"}"
        );
    }

    if (!database_get_weekly_translation_count(
            context->database,
            user_id,
            &weekly_translations))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Unable to load weekly activity.\"}"
        );
    }

    char response_json[1024];

    snprintf(
        response_json,
        sizeof(response_json),
        "{\"success\":true,\"statistics\":{"
        "\"total_translations\":%d,"
        "\"saved_translations\":%d,"
        "\"text_to_morse\":%d,"
        "\"morse_to_text\":%d,"
        "\"weekly_translations\":%d"
        "}}",
        total_translations,
        saved_translations,
        text_to_morse,
        morse_to_text,
        weekly_translations
    );

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        response_json
    );
}

static enum MHD_Result handle_preferences(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL || context == NULL ||
        context->database == NULL || body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Preferences service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1] = {0};
    if (!extract_json_string(body, "token", token, sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;
    if (!auth_validate_session(token, &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    const char *setting_key = "\"activity_notifications_enabled\"";
    const int setting_present = strstr(body, setting_key) != NULL;

    if (setting_present)
    {
        int enabled = 0;
        if (!extract_json_int_zero_or_one(
                body,
                "activity_notifications_enabled",
                &enabled))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"success\":false,\"error\":\"activity_notifications_enabled must be 0 or 1.\"}"
            );
        }

        if (!database_set_activity_notifications_enabled(
                context->database,
                user_id,
                enabled))
        {
            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"success\":false,\"error\":\"Unable to save notification preference.\"}"
            );
        }
    }

    int enabled = 0;
    if (!database_get_activity_notifications_enabled(
            context->database,
            user_id,
            &enabled))
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Unable to load notification preference.\"}"
        );
    }

    char response[256];
    snprintf(
        response,
        sizeof(response),
        "{\"success\":true,\"preferences\":{\"activity_notifications_enabled\":%d}}",
        enabled
    );

    return send_json_response(connection, MHD_HTTP_OK, response);
}

/* ---------------------------------------------------------
   UPDATE SAVED TRANSLATION
   --------------------------------------------------------- */

static enum MHD_Result handle_saved_update(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Saved translation service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];
    char title[256];
    char input_text[4096];
    char output_text[4096];
    char input_type[32];

    memset(token, 0, sizeof(token));
    memset(title, 0, sizeof(title));
    memset(input_text, 0, sizeof(input_text));
    memset(output_text, 0, sizeof(output_text));
    memset(input_type, 0, sizeof(input_type));

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    int saved_id = 0;

    if (!extract_json_int(
            body,
            "saved_id",
            &saved_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Valid saved_id is required.\"}"
        );
    }

    if (!extract_json_string(
            body,
            "title",
            title,
            sizeof(title)) ||
        !extract_json_string(
            body,
            "input",
            input_text,
            sizeof(input_text)) ||
        !extract_json_string(
            body,
            "output",
            output_text,
            sizeof(output_text)) ||
        !extract_json_string(
            body,
            "input_type",
            input_type,
            sizeof(input_type)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Title, input, output and input_type are required.\"}"
        );
    }

    if (strcmp(input_type, "TEXT_TO_MORSE") != 0 &&
        strcmp(input_type, "MORSE_TO_TEXT") != 0)
    {
        return send_json_response(
            connection,
            MHD_HTTP_BAD_REQUEST,
            "{\"success\":false,\"error\":\"Invalid input_type.\"}"
        );
    }

    if (!database_update_saved_translation(
            context->database,
            user_id,
            saved_id,
            title,
            input_text,
            output_text,
            input_type))
    {
        return send_json_response(
            connection,
            MHD_HTTP_NOT_FOUND,
            "{\"success\":false,\"error\":\"Saved translation not found or could not be updated.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_OK,
        "{\"success\":true,\"message\":\"Saved translation updated successfully.\"}"
    );
}

/* =========================================================
   PROFILE HANDLER
   ========================================================= */

static enum MHD_Result handle_profile(
    struct MHD_Connection *connection,
    ServerContext *context,
    const char *url,
    const char *body)
{
    if (connection == NULL ||
        context == NULL ||
        context->database == NULL ||
        body == NULL)
    {
        return send_json_response(
            connection,
            MHD_HTTP_INTERNAL_SERVER_ERROR,
            "{\"success\":false,\"error\":\"Profile service unavailable.\"}"
        );
    }

    char token[AUTH_TOKEN_SIZE + 1];

    memset(
        token,
        0,
        sizeof(token)
    );

    if (!extract_json_string(
            body,
            "token",
            token,
            sizeof(token)))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Authentication token is required.\"}"
        );
    }

    int user_id = 0;

    if (!auth_validate_session(
            token,
            &user_id))
    {
        return send_json_response(
            connection,
            MHD_HTTP_UNAUTHORIZED,
            "{\"success\":false,\"error\":\"Invalid or expired session.\"}"
        );
    }

    /* =====================================================
       GET PROFILE
       ===================================================== */

    if (strcmp(url, "/api/profile") == 0)
    {
        char name[256];
        char email[256];
        char username[256];
        char created_at[128];
        char last_login[128];

        memset(name, 0, sizeof(name));
        memset(email, 0, sizeof(email));
        memset(username, 0, sizeof(username));
        memset(created_at, 0, sizeof(created_at));
        memset(last_login, 0, sizeof(last_login));

        if (!database_get_user_profile(
                context->database,
                user_id,
                name,
                sizeof(name),
                email,
                sizeof(email),
                username,
                sizeof(username),
                created_at,
                sizeof(created_at),
                last_login,
                sizeof(last_login)))
        {
            return send_json_response(
                connection,
                MHD_HTTP_NOT_FOUND,
                "{\"success\":false,\"error\":\"User profile not found.\"}"
            );
        }

        char escaped_name[512];
        char escaped_email[512];
        char escaped_username[512];
        char escaped_created_at[256];
        char escaped_last_login[256];

        json_escape(
            name,
            escaped_name,
            sizeof(escaped_name)
        );

        json_escape(
            email,
            escaped_email,
            sizeof(escaped_email)
        );

        json_escape(
            username,
            escaped_username,
            sizeof(escaped_username)
        );

        json_escape(
            created_at,
            escaped_created_at,
            sizeof(escaped_created_at)
        );

        json_escape(
            last_login,
            escaped_last_login,
            sizeof(escaped_last_login)
        );

        int total_translations = 0;
        int saved_translations = 0;
        int text_to_morse = 0;
        int morse_to_text = 0;

        if (!database_get_user_statistics(
                context->database,
                user_id,
                &total_translations,
                &saved_translations,
                &text_to_morse,
                &morse_to_text))
        {
            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"success\":false,\"error\":\"Unable to load profile statistics.\"}"
            );
        }

        char response[2048];

        int written = snprintf(
            response,
            sizeof(response),
            "{\"success\":true,\"profile\":{"
            "\"id\":%d,"
            "\"name\":\"%s\","
            "\"email\":\"%s\","
            "\"username\":\"%s\","
            "\"created_at\":\"%s\","
            "\"last_login\":\"%s\","
            "\"translation_count\":%d"
            "}}",
            user_id,
            escaped_name,
            escaped_email,
            escaped_username,
            escaped_created_at,
            escaped_last_login,
            total_translations
        );

        if (written < 0 ||
            (size_t)written >= sizeof(response))
        {
            return send_json_response(
                connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                "{\"success\":false,\"error\":\"Response generation failed.\"}"
            );
        }

        return send_json_response(
            connection,
            MHD_HTTP_OK,
            response
        );
    }

    /* =====================================================
       UPDATE PROFILE
       ===================================================== */

    if (strcmp(url, "/api/profile/update") == 0)
    {
        char name[256];
        char email[256];
        char username[256];

        memset(name, 0, sizeof(name));
        memset(email, 0, sizeof(email));
        memset(username, 0, sizeof(username));

        if (!extract_json_string(
                body,
                "name",
                name,
                sizeof(name)) ||
            !extract_json_string(
                body,
                "email",
                email,
                sizeof(email)) ||
            !extract_json_string(
                body,
                "username",
                username,
                sizeof(username)))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"success\":false,\"error\":\"Name, email and username are required.\"}"
            );
        }

        if (name[0] == '\0' ||
            email[0] == '\0' ||
            username[0] == '\0')
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"success\":false,\"error\":\"Name, email and username cannot be empty.\"}"
            );
        }

        if (!database_update_user_profile(
                context->database,
                user_id,
                name,
                email,
                username))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"success\":false,\"error\":\"Failed to update profile.\"}"
            );
        }

            return send_json_response(
            connection,
            MHD_HTTP_OK,
            "{\"success\":true,\"message\":\"Profile updated successfully.\"}"
        );
    }

    /* =====================================================
       DELETE ACCOUNT
       ===================================================== */

    if (strcmp(url, "/api/profile/delete") == 0)
    {
        if (!database_delete_user(
                context->database,
                user_id))
        {
            return send_json_response(
                connection,
                MHD_HTTP_BAD_REQUEST,
                "{\"success\":false,\"error\":\"Failed to delete account.\"}"
            );
        }

        /*
         * Destroy the current authentication session
         * after the account has been successfully deleted.
         */
        auth_destroy_user_sessions(user_id);

        return send_json_response(
            connection,
            MHD_HTTP_OK,
            "{\"success\":true,\"message\":\"Account deleted successfully.\"}"
        );
    }

    return send_json_response(
        connection,
        MHD_HTTP_NOT_FOUND,
        "{\"success\":false,\"error\":\"Profile endpoint not found.\"}"
    );
}

/* =========================================================
   HTTP REQUEST HANDLER
   ========================================================= */

static enum MHD_Result request_handler(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    (void)version;

    ServerContext *context =
        (ServerContext *)cls;


    /* =====================================================
       FIRST CALLBACK
       ===================================================== */

    if (*con_cls == NULL)
    {
        RequestData *request =
            (RequestData *)calloc(
                1,
                sizeof(RequestData)
            );

        if (request == NULL)
        {
            return MHD_NO;
        }

        *con_cls = request;

        return MHD_YES;
    }

    RequestData *request =
        (RequestData *)(*con_cls);


    /* =====================================================
       CORS PREFLIGHT
       ===================================================== */

    if (strcmp(method, "OPTIONS") == 0)
    {
        if (*upload_data_size != 0)
        {
            *upload_data_size = 0;

            return MHD_YES;
        }

        *con_cls = NULL;

        free(request);

        return send_json_response(
            connection,
            MHD_HTTP_NO_CONTENT,
            ""
        );
    }


    /* =====================================================
       RECEIVE POST BODY
       ===================================================== */

    if ((strcmp(method, "POST") == 0 || strcmp(method, "DELETE") == 0) &&
        *upload_data_size != 0)
    {
        if (request->body_length +
                *upload_data_size >=
            MAX_REQUEST_BODY)
        {
            *upload_data_size = 0;

            *con_cls = NULL;

            free(request);

            return send_json_response(
                connection,
                MHD_HTTP_REQUEST_ENTITY_TOO_LARGE,
                "{\"error\":\"Request body too large\"}"
            );
        }

        memcpy(
            request->body +
                request->body_length,
            upload_data,
            *upload_data_size
        );

        request->body_length +=
            *upload_data_size;

        request->body[
            request->body_length
        ] = '\0';

        *upload_data_size = 0;

        return MHD_YES;
    }


    /* =====================================================
       ROUTING
       ===================================================== */

    enum MHD_Result result;

    if (strcmp(method, "GET") == 0 &&
        strcmp(url, "/api/health") == 0)
    {
        result =
            handle_health(
                connection
            );
    }
    else if ((strcmp(method, "POST") == 0) &&
         (strcmp(url, "/api/auth/signup") == 0 ||
          strcmp(url, "/api/auth/login") == 0))
{
    result =
        handle_authentication(
            connection,
            context,
            url,
            request->body
        );
}
else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/auth/validate") == 0)
{
    result =
        handle_session_validation(
            connection,
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/auth/logout") == 0)
{
    result =
        handle_logout(
            connection,
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/auth/change-password") == 0)
{
    result =
        handle_change_password(
            connection,
            context,
            request->body
        );
}

else if (strcmp(method, "POST") == 0 &&
         strcmp(url, "/api/statistics") == 0)
{
    result =
        handle_statistics(
            connection,
            context,
            request->body
        );
}

else if (strcmp(method, "POST") == 0 &&
         strcmp(url, "/api/preferences") == 0)
{
    result =
        handle_preferences(
            connection,
            context,
            request->body
        );
}

else if (strcmp(method, "POST") == 0 &&
         strcmp(url, "/api/profile") == 0)
{
    result =
        handle_profile(
            connection,
            context,
            "/api/profile",
            request->body
        );
}

else if (strcmp(method, "POST") == 0 &&
         strcmp(url, "/api/profile/update") == 0)
{
    result =
        handle_profile(
            connection,
            context,
            "/api/profile/update",
            request->body
        );
}

else if (strcmp(method, "POST") == 0 &&
         strcmp(url, "/api/profile/delete") == 0)
{
    result =
        handle_profile(
            connection,
            context,
            "/api/profile/delete",
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/history") == 0)
{
    result =
        handle_history(
            connection,
            context,
            request->body
        );
}

else if (strcmp(method, "DELETE") == 0 &&
         strcmp(url, "/api/history") == 0)
{
    result =
        handle_history_delete(
            connection,
            context,
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/saved") == 0)
{
    result =
        handle_saved_create(
            connection,
            context,
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/saved/list") == 0)
{
    result =
        handle_saved_list(
            connection,
            context,
            request->body
        );
}

else if ((strcmp(method, "DELETE") == 0) &&
         strcmp(url, "/api/saved") == 0)
{
    result =
        handle_saved_delete(
            connection,
            context,
            request->body
        );
}

else if ((strcmp(method, "POST") == 0) &&
         strcmp(url, "/api/saved/update") == 0)
{
    result =
        handle_saved_update(
            connection,
            context,
            request->body
        );
}

    else if ((strcmp(method, "POST") == 0) &&
             (strcmp(
                  url,
                  "/api/translate/text-to-morse") == 0 ||
              strcmp(
                  url,
                  "/api/translate/morse-to-text") == 0))
    {
        result =
            handle_translation(
                connection,
                context,
                url,
                request->body
            );
    }
    else
    {
        result =
            send_json_response(
                connection,
                MHD_HTTP_NOT_FOUND,
                "{\"error\":\"Route not found\"}"
            );
    }


    /* =====================================================
       REQUEST CLEANUP
       ===================================================== */

    *con_cls = NULL;

    free(request);

    return result;
}


/* =========================================================
   START SERVER
   ========================================================= */

struct MHD_Daemon *server_start(
    unsigned short port,
    ServerContext *context)
{
    struct MHD_Daemon *daemon =
        MHD_start_daemon(
            MHD_USE_INTERNAL_POLLING_THREAD,
            port,
            NULL,
            NULL,
            &request_handler,
            context,
            MHD_OPTION_END
        );

    return daemon;
}


/* =========================================================
   STOP SERVER
   ========================================================= */

void server_stop(
    struct MHD_Daemon *daemon)
{
    if (daemon != NULL)
    {
        MHD_stop_daemon(daemon);
    }
}
