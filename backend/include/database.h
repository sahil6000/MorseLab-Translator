#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

typedef struct Database {
    sqlite3 *connection;
} Database;

/* Database lifecycle */
Database *database_open(const char *database_path);
void database_close(Database *database);
int database_initialize_schema(Database *database);

/* User operations */
int database_create_user(
    Database *database,
    const char *name,
    const char *username,
    const char *email,
    const char *password_hash,
    int *user_id
);

int database_find_user_by_email(
    Database *database,
    const char *email,
    int *user_id,
    char *name,
    int name_size,
    char *password_hash,
    int password_hash_size
);

int database_get_user_profile(
    Database *database,
    int user_id,
    char *name,
    int name_size,
    char *email,
    int email_size,
    char *username,
    int username_size,
    char *created_at,
    int created_at_size,
    char *last_login,
    int last_login_size
);

int database_update_user_profile(
    Database *database,
    int user_id,
    const char *name,
    const char *email,
    const char *username
);

int database_delete_user(
    Database *database,
    int user_id
);

int database_update_user_password(
    Database *database,
    int user_id,
    const char *password_hash
);

int database_update_last_login(
    Database *database,
    int user_id
);  

int database_get_user_password_hash(
    Database *database,
    int user_id,
    char *password_hash,
    int password_hash_size
);

/* Translation history */
int database_add_history(
    Database *database,
    int user_id,
    const char *input_text,
    const char *output_text,
    const char *input_type,
    int *history_id
);

int database_get_history(
    Database *database,
    int user_id,
    char ***inputs,
    char ***outputs,
    char ***input_types,
    char ***created_ats,
    int **history_ids,
    int *count
);

int database_delete_history(
    Database *database,
    int user_id,
    int history_id
);

/* Saved translations */
int database_save_translation(
    Database *database,
    int user_id,
    const char *title,
    const char *input_text,
    const char *output_text,
    const char *input_type,
    int *saved_id
);

int database_delete_saved_translation(
    Database *database,
    int user_id,
    int saved_id
);

int database_update_saved_translation(
    Database *database,
    int user_id,
    int saved_id,
    const char *title,
    const char *input_text,
    const char *output_text,
    const char *input_type
);

int database_get_saved_translations(
    Database *database,
    int user_id,
    char ***titles,
    char ***inputs,
    char ***outputs,
    char ***input_types,
    char ***created_ats,
    char ***updated_ats,
    int **saved_ids,
    int *count
);

int database_get_user_statistics(
    Database *database,
    int user_id,
    int *total_translations,
    int *saved_translations,
    int *text_to_morse,
    int *morse_to_text
);

int database_get_weekly_translation_count(
    Database *database,
    int user_id,
    int *weekly_translations
);

int database_get_activity_notifications_enabled(
    Database *database,
    int user_id,
    int *enabled
);

int database_set_activity_notifications_enabled(
    Database *database,
    int user_id,
    int enabled
);

#endif
