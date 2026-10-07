#include <stdio.h>
#include <stdlib.h>

#include "database.h"

int main(void)
{
    Database *database = database_open("backend/database/test_morselab.db");

    if (database == NULL)
    {
        printf("Failed to open database.\n");
        return 1;
    }

    printf("Database opened successfully.\n");

    if (!database_initialize_schema(database))
    {
        printf("Failed to initialize database schema.\n");
        database_close(database);
        return 1;
    }

    printf("Database schema initialized successfully.\n");

    int user_id = 0;

    if (!database_create_user(
            database,
            "Test User",
            "test_user",
            "test@morselab.com",
            "TEST_HASH",
            &user_id))
    {
        printf("User creation failed.\n");
        database_close(database);
        return 1;
    }

    printf("User created successfully. User ID: %d\n", user_id);

    int notifications_enabled = -1;
    if (!database_get_activity_notifications_enabled(
            database,
            user_id,
            &notifications_enabled) ||
        notifications_enabled != 0 ||
        !database_set_activity_notifications_enabled(
            database,
            user_id,
            1) ||
        !database_get_activity_notifications_enabled(
            database,
            user_id,
            &notifications_enabled) ||
        notifications_enabled != 1)
    {
        printf("Activity notification preference test failed.\n");
        database_close(database);
        return 1;
    }

    printf("Activity notification preference persistence: SUCCESS\n");

    int found_user_id;
    char name[100];
    char password_hash[200];

    if (database_find_user_by_email(
            database,
            "test@morselab.com",
            &found_user_id,
            name,
            sizeof(name),
            password_hash,
            sizeof(password_hash)))
    {
        printf(
            "User found: ID %d | Name: %s | Hash: %s\n",
            found_user_id,
            name,
            password_hash
        );
    }
    else
    {
        printf("User lookup failed.\n");
    }

    int history_id;

    if (database_add_history(
            database,
            user_id,
            "HELLO",
            ".... . .-.. .-.. ---",
            "TEXT_TO_MORSE",
            &history_id))
    {
        printf(
            "History added successfully. History ID: %d\n",
            history_id
        );
    }
    else
    {
        printf("History insertion failed.\n");
        database_close(database);
        return 1;
    }

    int weekly_translations = 0;
    if (!database_get_weekly_translation_count(
            database,
            user_id,
            &weekly_translations) ||
        weekly_translations != 1)
    {
        printf("Weekly translation count test failed.\n");
        database_close(database);
        return 1;
    }

    printf("Weekly translation count: %d\n", weekly_translations);

    int saved_id;

    if (database_save_translation(
        database,
        user_id,
        "SOS Translation",
        "SOS",
        "... --- ...",
        "TEXT_TO_MORSE",
        &saved_id))
    {
        printf(
            "Saved translation added successfully. Saved ID: %d\n",
            saved_id
        );
    }
    else
    {
        printf("Saved translation insertion failed.\n");
    }

    char **saved_titles = NULL;
    char **saved_inputs = NULL;
    char **saved_outputs = NULL;
    char **saved_input_types = NULL;
    char **saved_created_ats = NULL;
    char **saved_updated_ats = NULL;
    int *saved_ids = NULL;
    int saved_count = 0;

    if (database_get_saved_translations(
            database,
            user_id,
            &saved_titles,
            &saved_inputs,
            &saved_outputs,
            &saved_input_types,
            &saved_created_ats,
            &saved_updated_ats,
            &saved_ids,
            &saved_count))
    {
        printf(
            "Saved translations retrieved successfully. Count: %d\n",
            saved_count
        );

        for (int i = 0; i < saved_count; i++)
        {
            printf(
                "Saved [%d] ID: %d | Title: %s | Input: %s | "
                "Output: %s | Type: %s | Created: %s | Updated: %s\n",
                i,
                saved_ids[i],
                saved_titles[i],
                saved_inputs[i],
                saved_outputs[i],
                saved_input_types[i],
                saved_created_ats[i],
                saved_updated_ats[i]
            );

            free(saved_titles[i]);
            free(saved_inputs[i]);
            free(saved_outputs[i]);
            free(saved_input_types[i]);
            free(saved_created_ats[i]);
            free(saved_updated_ats[i]);
        }

        free(saved_titles);
        free(saved_inputs);
        free(saved_outputs);
        free(saved_input_types);
        free(saved_created_ats);
        free(saved_updated_ats);
        free(saved_ids);
    }
    else
    {
        printf("Saved translation retrieval failed.\n");
    }

    if (database_delete_history(
            database,
            user_id,
            history_id))
    {
        printf("History deleted successfully.\n");
    }
    else
    {
        printf("History deletion failed.\n");
    }

    if (database_delete_saved_translation(
            database,
            user_id,
            saved_id))
    {
        printf("Saved translation deleted successfully.\n");
    }
    else
    {
        printf("Saved translation deletion failed.\n");
    }

    database_close(database);

    printf("Database closed successfully.\n");

    return 0;
}
