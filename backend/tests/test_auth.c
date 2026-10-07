#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auth.h"
#include "database.h"

int main(void)
{
    const char *database_path =
        "backend/database/test_auth.db";

    Database *database =
        database_open(database_path);

    if (database == NULL)
    {
        printf("Database open failed.\n");
        return EXIT_FAILURE;
    }

    if (!database_initialize_schema(database))
    {
        printf("Database schema initialization failed.\n");
        database_close(database);
        return EXIT_FAILURE;
    }

    printf("Authentication test database ready.\n");

    /* =====================================================
       TEST SIGNUP
       ===================================================== */

    AuthResult signup = auth_signup(
        database,
        "Sahil Test",
        "sahil_test",
        "sahil.test@example.com",
        "SecurePass123!"
    );

    printf(
        "Signup: %s | User ID: %d | Message: %s\n",
        signup.success ? "SUCCESS" : "FAILED",
        signup.user_id,
        signup.message
    );

    if (!signup.success)
    {
        database_close(database);
        return EXIT_FAILURE;
    }

    /* =====================================================
       TEST CORRECT LOGIN
       ===================================================== */

    AuthResult login = auth_login(
        database,
        "sahil.test@example.com",
        "SecurePass123!"
    );

    printf(
        "Correct password login: %s | User ID: %d | Message: %s\n",
        login.success ? "SUCCESS" : "FAILED",
        login.user_id,
        login.message
    );

    if (!login.success)
    {
        database_close(database);
        return EXIT_FAILURE;
    }

    /* =====================================================
       TEST SESSION TOKEN GENERATION
       ===================================================== */

    printf(
        "Session token generated (length: %zu)\n",
        strlen(login.token)
    );

    if (strlen(login.token) != AUTH_TOKEN_SIZE)
    {
        printf(
            "Session token test: FAILED | Invalid token length.\n"
        );

        database_close(database);
        return EXIT_FAILURE;
    }

    printf(
        "Session token generation: SUCCESS\n"
    );

    /* =====================================================
       TEST SESSION TOKEN VALIDATION
       ===================================================== */

    int validated_user_id = 0;

    int session_valid =
        auth_validate_session(
            login.token,
            &validated_user_id
        );

    printf(
        "Session validation: %s | User ID: %d\n",
        session_valid ? "SUCCESS" : "FAILED",
        validated_user_id
    );

    if (!session_valid ||
        validated_user_id != login.user_id)
    {
        printf(
            "Session validation test failed.\n"
        );

        database_close(database);
        return EXIT_FAILURE;
    }

    /* =====================================================
       TEST INCORRECT PASSWORD
       ===================================================== */

    AuthResult wrong_login = auth_login(
        database,
        "sahil.test@example.com",
        "WrongPassword123!"
    );

    printf(
        "Wrong password login: %s | Message: %s\n",
        wrong_login.success
            ? "UNEXPECTED SUCCESS"
            : "CORRECTLY REJECTED",
        wrong_login.message
    );

    if (wrong_login.success)
    {
        printf(
            "Wrong password test failed.\n"
        );

        database_close(database);
        return EXIT_FAILURE;
    }

    /* =====================================================
       TEST SESSION DESTRUCTION
       ===================================================== */

    int session_destroyed =
        auth_destroy_session(
            login.token
        );

    printf(
        "Session destruction: %s\n",
        session_destroyed
            ? "SUCCESS"
            : "FAILED"
    );

    if (!session_destroyed)
    {
        printf(
            "Session destruction test failed.\n"
        );

        database_close(database);
        return EXIT_FAILURE;
    }

    /* =====================================================
       TEST TOKEN AFTER LOGOUT
       ===================================================== */

    validated_user_id = 0;

    int session_after_logout =
        auth_validate_session(
            login.token,
            &validated_user_id
        );

    printf(
        "Token after logout: %s\n",
        session_after_logout
            ? "UNEXPECTEDLY VALID"
            : "CORRECTLY INVALIDATED"
    );

    /* =====================================================
       TEST REVOKING ALL USER SESSIONS
       ===================================================== */

    char second_token[AUTH_TOKEN_SIZE + 1] = {0};
    char third_token[AUTH_TOKEN_SIZE + 1] = {0};

    if (!auth_create_session(
            login.user_id,
            second_token,
            sizeof(second_token)) ||
        !auth_create_session(
            login.user_id,
            third_token,
            sizeof(third_token)))
    {
        printf("Could not create additional user sessions.\n");
        database_close(database);
        return EXIT_FAILURE;
    }

    auth_destroy_user_sessions(login.user_id);

    int second_session_valid =
        auth_validate_session(second_token, &validated_user_id);
    int third_session_valid =
        auth_validate_session(third_token, &validated_user_id);

    if (second_session_valid || third_session_valid)
    {
        printf("User session revocation test failed.\n");
        database_close(database);
        return EXIT_FAILURE;
    }

    printf("All sessions for the user were revoked successfully.\n");

    database_close(database);

    if (!session_after_logout)
    {
        printf(
            "\nAll authentication and session tests "
            "passed successfully.\n"
        );

        return EXIT_SUCCESS;
    }

    printf(
        "\nSession invalidation test failed.\n"
    );

    return EXIT_FAILURE;
}
