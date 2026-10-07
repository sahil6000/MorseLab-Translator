#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"

static char *database_duplicate_text(const char *text)
{
    if (text == NULL)
    {
        return NULL;
    }

    size_t length = strlen(text) + 1;

    char *copy = malloc(length);

    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, text, length);

    return copy;
}

Database *database_open(const char *database_path)
{
    if (database_path == NULL)
    {
        return NULL;
    }

    Database *database = (Database *)malloc(sizeof(Database));

    if (database == NULL)
    {
        return NULL;
    }

    database->connection = NULL;

    int result = sqlite3_open(database_path, &database->connection);

    if (result != SQLITE_OK)
    {
        sqlite3_close(database->connection);
        free(database);
        return NULL;
    }

    return database;
}

void database_close(Database *database)
{
    if (database == NULL)
    {
        return;
    }

    if (database->connection != NULL)
    {
        sqlite3_close(database->connection);
    }

    free(database);
}

int database_initialize_schema(Database *database)
{
    if (database == NULL ||
        database->connection == NULL)
    {
        return 0;
    }

    const char *sql =
        "PRAGMA foreign_keys = ON;"

        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "username TEXT NOT NULL UNIQUE,"
        "email TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "last_login DATETIME"
        ");"

        "CREATE TABLE IF NOT EXISTS translation_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "title TEXT NOT NULL DEFAULT 'Untitled',"
        "input_text TEXT NOT NULL,"
        "output_text TEXT NOT NULL,"
        "input_type TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS saved_translations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "title TEXT NOT NULL DEFAULT 'Untitled',"
        "input_text TEXT NOT NULL,"
        "output_text TEXT NOT NULL,"
        "input_type TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS user_preferences ("
        "user_id INTEGER PRIMARY KEY,"
        "activity_notifications_enabled INTEGER NOT NULL DEFAULT 0 "
        "CHECK (activity_notifications_enabled IN (0, 1)),"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");";

    char *error_message = NULL;

    int result = sqlite3_exec(
        database->connection,
        sql,
        NULL,
        NULL,
        &error_message
    );

    if (result != SQLITE_OK)
    {
        if (error_message != NULL)
        {
            sqlite3_free(error_message);
        }

        return 0;
    }

    return 1;
}

int database_create_user(
    Database *database,
    const char *name,
    const char *username,
    const char *email,
    const char *password_hash,
    int *user_id
)
{
    if (database == NULL ||
        database->connection == NULL ||
        name == NULL ||
        username == NULL ||
        email == NULL ||
        password_hash == NULL ||
        user_id == NULL)
    {
        return 0;
    }

    const char *sql =
        "INSERT INTO users "
        "(name, username, email, password_hash) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_text(
        statement,
        1,
        name,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        username,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        email,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        4,
        password_hash,
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    *user_id =
        (int)sqlite3_last_insert_rowid(
            database->connection
        );

    sqlite3_finalize(statement);

    return 1;
}

int database_find_user_by_email(
    Database *database,
    const char *email,
    int *user_id,
    char *name,
    int name_size,
    char *password_hash,
    int password_hash_size)
{
    if (database == NULL ||
        database->connection == NULL ||
        email == NULL ||
        user_id == NULL ||
        name == NULL ||
        password_hash == NULL ||
        name_size <= 0 ||
        password_hash_size <= 0)
    {
        return 0;
    }

    const char *sql =
        "SELECT id, name, password_hash "
        "FROM users "
        "WHERE email = ? "
        "LIMIT 1;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_text(statement, 1, email, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        *user_id = sqlite3_column_int(statement, 0);

        const unsigned char *db_name =
            sqlite3_column_text(statement, 1);

        const unsigned char *db_password_hash =
            sqlite3_column_text(statement, 2);

        if (db_name == NULL || db_password_hash == NULL)
        {
            sqlite3_finalize(statement);
            return 0;
        }

        strncpy(
            name,
            (const char *)db_name,
            (size_t)name_size - 1
        );

        name[name_size - 1] = '\0';

        strncpy(
            password_hash,
            (const char *)db_password_hash,
            (size_t)password_hash_size - 1
        );

        password_hash[password_hash_size - 1] = '\0';

        sqlite3_finalize(statement);

        return 1;
    }

    sqlite3_finalize(statement);

    return 0;
}

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
    int last_login_size)
{
    if (database == NULL ||
        database->connection == NULL ||
        name == NULL ||
        email == NULL ||
        username == NULL ||
        created_at == NULL ||
        last_login == NULL ||
        name_size <= 0 ||
        email_size <= 0 ||
        username_size <= 0 ||
        created_at_size <= 0 ||
        last_login_size <= 0)
    {
        return 0;
    }

    const char *sql =
        "SELECT name, email, username, created_at, last_login "
        "FROM users "
        "WHERE id = ? "
        "LIMIT 1;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);

    result = sqlite3_step(statement);

    if (result != SQLITE_ROW)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    const unsigned char *database_name =
        sqlite3_column_text(statement, 0);

    const unsigned char *database_email =
        sqlite3_column_text(statement, 1);

    const unsigned char *database_username =
        sqlite3_column_text(statement, 2);

    const unsigned char *database_created_at =
        sqlite3_column_text(statement, 3);

    const unsigned char *database_last_login =
        sqlite3_column_text(statement, 4);

    snprintf(
        name,
        name_size,
        "%s",
        database_name != NULL
            ? (const char *)database_name
            : ""
    );

    snprintf(
        email,
        email_size,
        "%s",
        database_email != NULL
            ? (const char *)database_email
            : ""
    );

    snprintf(
        username,
        username_size,
        "%s",
        database_username != NULL
            ? (const char *)database_username
            : ""
    );

    snprintf(
        created_at,
        created_at_size,
        "%s",
        database_created_at != NULL
            ? (const char *)database_created_at
            : ""
    );

    snprintf(
        last_login,
        last_login_size,
        "%s",
        database_last_login != NULL
            ? (const char *)database_last_login
            : ""
    );

    sqlite3_finalize(statement);

    return 1;
}

int database_update_user_profile(
    Database *database,
    int user_id,
    const char *name,
    const char *email,
    const char *username)
{
    if (database == NULL ||
        database->connection == NULL ||
        name == NULL ||
        email == NULL ||
        username == NULL)
    {
        return 0;
    }

    const char *sql =
        "UPDATE users "
        "SET name = ?, "
        "email = ?, "
        "username = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_text(
        statement,
        1,
        name,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        email,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        username,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        4,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int updated =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return updated;
}

int database_delete_user(
    Database *database,
    int user_id)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0)
    {
        return 0;
    }

    const char *sql =
        "DELETE FROM users "
        "WHERE id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(
        statement,
        1,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int deleted =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return deleted;
}

int database_update_last_login(
    Database *database,
    int user_id)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0)
    {
        return 0;
    }

    const char *sql =
        "UPDATE users "
        "SET last_login = CURRENT_TIMESTAMP "
        "WHERE id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(
        statement,
        1,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int updated =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return updated;
}

int database_update_user_password(
    Database *database,
    int user_id,
    const char *password_hash)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0 ||
        password_hash == NULL)
    {
        return 0;
    }

    const char *sql =
        "UPDATE users "
        "SET password_hash = ? "
        "WHERE id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_text(
        statement,
        1,
        password_hash,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        2,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int updated =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return updated;
}



int database_add_history(
    Database *database,
    int user_id,
    const char *input_text,
    const char *output_text,
    const char *input_type,
    int *history_id)
{
    if (database == NULL ||
        database->connection == NULL ||
        input_text == NULL ||
        output_text == NULL ||
        input_type == NULL ||
        history_id == NULL)
    {
        return 0;
    }

    const char *sql =
        "INSERT INTO translation_history "
        "(user_id, input_text, output_text, input_type) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);
    sqlite3_bind_text(statement, 2, input_text, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, output_text, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, input_type, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    *history_id =
        (int)sqlite3_last_insert_rowid(database->connection);

    sqlite3_finalize(statement);

    return 1;
}

int database_get_history(
    Database *database,
    int user_id,
    char ***inputs,
    char ***outputs,
    char ***input_types,
    char ***created_ats,
    int **history_ids,
    int *count)
{
    if (database == NULL ||
        database->connection == NULL ||
        inputs == NULL ||
        outputs == NULL ||
        input_types == NULL ||
        created_ats == NULL ||
        history_ids == NULL ||
        count == NULL)
    {
        return 0;
    }

    *inputs = NULL;
    *outputs = NULL;
    *input_types = NULL;
    *created_ats = NULL;
    *history_ids = NULL;
    *count = 0;

    const char *sql =
        "SELECT id, input_text, output_text, input_type, created_at "
        "FROM translation_history "
        "WHERE user_id = ? "
        "ORDER BY id DESC;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(
        statement,
        1,
        user_id
    );

    int capacity = 10;

    char **input_array =
        calloc(
            (size_t)capacity,
            sizeof(char *)
        );

    char **output_array =
        calloc(
            (size_t)capacity,
            sizeof(char *)
        );

    char **type_array =
        calloc(
            (size_t)capacity,
            sizeof(char *)
        );

    char **date_array =
        calloc(
            (size_t)capacity,
            sizeof(char *)
        );

    int *id_array =
        calloc(
            (size_t)capacity,
            sizeof(int)
        );

    if (input_array == NULL ||
        output_array == NULL ||
        type_array == NULL ||
        date_array == NULL ||
        id_array == NULL)
    {
        free(input_array);
        free(output_array);
        free(type_array);
        free(date_array);
        free(id_array);

        sqlite3_finalize(statement);

        return 0;
    }

    int item_count = 0;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        if (item_count >= capacity)
{
    int new_capacity = capacity * 2;

    char **new_inputs =
        calloc(
            (size_t)new_capacity,
            sizeof(char *)
        );

    char **new_outputs =
        calloc(
            (size_t)new_capacity,
            sizeof(char *)
        );

    char **new_types =
        calloc(
            (size_t)new_capacity,
            sizeof(char *)
        );

    char **new_dates =
        calloc(
            (size_t)new_capacity,
            sizeof(char *)
        );

    int *new_ids =
        calloc(
            (size_t)new_capacity,
            sizeof(int)
        );

    if (new_inputs == NULL ||
        new_outputs == NULL ||
        new_types == NULL ||
        new_dates == NULL ||
        new_ids == NULL)
    {
        free(new_inputs);
        free(new_outputs);
        free(new_types);
        free(new_dates);
        free(new_ids);

        for (int i = 0; i < item_count; i++)
        {
            free(input_array[i]);
            free(output_array[i]);
            free(type_array[i]);
            free(date_array[i]);
        }

        free(input_array);
        free(output_array);
        free(type_array);
        free(date_array);
        free(id_array);

        sqlite3_finalize(statement);

        return 0;
    }

    memcpy(
        new_inputs,
        input_array,
        (size_t)item_count * sizeof(char *)
    );

    memcpy(
        new_outputs,
        output_array,
        (size_t)item_count * sizeof(char *)
    );

    memcpy(
        new_types,
        type_array,
        (size_t)item_count * sizeof(char *)
    );

    memcpy(
        new_dates,
        date_array,
        (size_t)item_count * sizeof(char *)
    );

    memcpy(
        new_ids,
        id_array,
        (size_t)item_count * sizeof(int)
    );

    free(input_array);
    free(output_array);
    free(type_array);
    free(date_array);
    free(id_array);

    input_array = new_inputs;
    output_array = new_outputs;
    type_array = new_types;
    date_array = new_dates;
    id_array = new_ids;

    capacity = new_capacity;
}

        const unsigned char *input_text =
            sqlite3_column_text(statement, 1);

        const unsigned char *output_text =
            sqlite3_column_text(statement, 2);

        const unsigned char *input_type =
            sqlite3_column_text(statement, 3);

        const unsigned char *created_at =
            sqlite3_column_text(statement, 4);

        input_array[item_count] =
            database_duplicate_text(
                input_text != NULL
                    ? (const char *)input_text
                    : ""
            );

        output_array[item_count] =
            database_duplicate_text(
                output_text != NULL
                    ? (const char *)output_text
                    : ""
            );

        type_array[item_count] =
            database_duplicate_text(
                input_type != NULL
                    ? (const char *)input_type
                    : ""
            );

        date_array[item_count] =
            database_duplicate_text(
                created_at != NULL
                    ? (const char *)created_at
                    : ""
            );

        id_array[item_count] =
            sqlite3_column_int(
                statement,
                0
            );

        if (input_array[item_count] == NULL ||
            output_array[item_count] == NULL ||
            type_array[item_count] == NULL ||
            date_array[item_count] == NULL)
        {
            for (int i = 0; i <= item_count; i++)
            {
                free(input_array[i]);
                free(output_array[i]);
                free(type_array[i]);
                free(date_array[i]);
            }

            free(input_array);
            free(output_array);
            free(type_array);
            free(date_array);
            free(id_array);

            sqlite3_finalize(statement);

            return 0;
        }

        item_count++;
    }

    if (result != SQLITE_DONE)
    {
        for (int i = 0; i < item_count; i++)
        {
            free(input_array[i]);
            free(output_array[i]);
            free(type_array[i]);
            free(date_array[i]);
        }

        free(input_array);
        free(output_array);
        free(type_array);
        free(date_array);
        free(id_array);

        sqlite3_finalize(statement);

        return 0;
    }

    sqlite3_finalize(statement);

    *inputs = input_array;
    *outputs = output_array;
    *input_types = type_array;
    *created_ats = date_array;
    *history_ids = id_array;
    *count = item_count;

    return 1;
}

int database_delete_history(
    Database *database,
    int user_id,
    int history_id)
{
    if (database == NULL || database->connection == NULL)
    {
        return 0;
    }

    const char *sql =
        "DELETE FROM translation_history "
        "WHERE id = ? AND user_id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, history_id);
    sqlite3_bind_int(statement, 2, user_id);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int deleted =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return deleted;
}

int database_save_translation(
    Database *database,
    int user_id,
    const char *title,
    const char *input_text,
    const char *output_text,
    const char *input_type,
    int *saved_id)
{
    if (database == NULL ||
        database->connection == NULL ||
        title == NULL ||
        input_text == NULL ||
        output_text == NULL ||
        input_type == NULL ||
        saved_id == NULL)
    {
        return 0;
    }

    const char *sql =
        "INSERT INTO saved_translations "
        "(user_id, title, input_text, output_text, input_type) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);
    sqlite3_bind_text(statement, 2, title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, input_text, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, output_text, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, input_type, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    *saved_id =
        (int)sqlite3_last_insert_rowid(database->connection);

    sqlite3_finalize(statement);

    return 1;
}

int database_update_saved_translation(
    Database *database,
    int user_id,
    int saved_id,
    const char *title,
    const char *input_text,
    const char *output_text,
    const char *input_type)
{
    if (database == NULL ||
        database->connection == NULL ||
        title == NULL ||
        input_text == NULL ||
        output_text == NULL ||
        input_type == NULL)
    {
        return 0;
    }

    const char *sql =
        "UPDATE saved_translations "
        "SET title = ?, "
        "input_text = ?, "
        "output_text = ?, "
        "input_type = ?, "
        "updated_at = CURRENT_TIMESTAMP "
        "WHERE id = ? AND user_id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_text(
        statement,
        1,
        title,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        input_text,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        output_text,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        4,
        input_type,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        5,
        saved_id
    );

    sqlite3_bind_int(
        statement,
        6,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int updated =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return updated;
}

int database_delete_saved_translation(
    Database *database,
    int user_id,
    int saved_id)
{
    if (database == NULL || database->connection == NULL)
    {
        return 0;
    }

    const char *sql =
        "DELETE FROM saved_translations "
        "WHERE id = ? AND user_id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, saved_id);
    sqlite3_bind_int(statement, 2, user_id);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    int deleted =
        sqlite3_changes(database->connection) > 0;

    sqlite3_finalize(statement);

    return deleted;
}

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
    int *count)
{
    if (database == NULL ||
        database->connection == NULL ||
        titles == NULL ||
        inputs == NULL ||
        outputs == NULL ||
        input_types == NULL ||
        created_ats == NULL ||
        updated_ats == NULL ||
        saved_ids == NULL ||
        count == NULL)
    {
        return 0;
    }

    *titles = NULL;
    *inputs = NULL;
    *outputs = NULL;
    *input_types = NULL;
    *created_ats = NULL;
    *updated_ats = NULL;
    *saved_ids = NULL;
    *count = 0;

    const char *sql =
        "SELECT id, title, input_text, output_text, input_type, "
        "created_at, updated_at "
        "FROM saved_translations "
        "WHERE user_id = ? "
        "ORDER BY id DESC;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);

    int capacity = 10;

    char **title_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    char **input_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    char **output_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    char **type_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    char **created_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    char **updated_array = calloc(
        (size_t)capacity,
        sizeof(char *)
    );

    int *id_array = calloc(
        (size_t)capacity,
        sizeof(int)
    );

    if (title_array == NULL ||
        input_array == NULL ||
        output_array == NULL ||
        type_array == NULL ||
        created_array == NULL ||
        updated_array == NULL ||
        id_array == NULL)
    {
        free(title_array);
        free(input_array);
        free(output_array);
        free(type_array);
        free(created_array);
        free(updated_array);
        free(id_array);

        sqlite3_finalize(statement);

        return 0;
    }

    int item_count = 0;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        if (item_count >= capacity)
        {
            int new_capacity = capacity * 2;

            char **new_titles = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            char **new_inputs = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            char **new_outputs = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            char **new_types = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            char **new_created = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            char **new_updated = calloc(
                (size_t)new_capacity,
                sizeof(char *)
            );

            int *new_ids = calloc(
                (size_t)new_capacity,
                sizeof(int)
            );

            if (new_titles == NULL ||
                new_inputs == NULL ||
                new_outputs == NULL ||
                new_types == NULL ||
                new_created == NULL ||
                new_updated == NULL ||
                new_ids == NULL)
            {
                free(new_titles);
                free(new_inputs);
                free(new_outputs);
                free(new_types);
                free(new_created);
                free(new_updated);
                free(new_ids);

                for (int i = 0; i < item_count; i++)
                {
                    free(title_array[i]);
                    free(input_array[i]);
                    free(output_array[i]);
                    free(type_array[i]);
                    free(created_array[i]);
                    free(updated_array[i]);
                }

                free(title_array);
                free(input_array);
                free(output_array);
                free(type_array);
                free(created_array);
                free(updated_array);
                free(id_array);

                sqlite3_finalize(statement);

                return 0;
            }

            memcpy(
                new_titles,
                title_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_inputs,
                input_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_outputs,
                output_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_types,
                type_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_created,
                created_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_updated,
                updated_array,
                (size_t)item_count * sizeof(char *)
            );

            memcpy(
                new_ids,
                id_array,
                (size_t)item_count * sizeof(int)
            );

            free(title_array);
            free(input_array);
            free(output_array);
            free(type_array);
            free(created_array);
            free(updated_array);
            free(id_array);

            title_array = new_titles;
            input_array = new_inputs;
            output_array = new_outputs;
            type_array = new_types;
            created_array = new_created;
            updated_array = new_updated;
            id_array = new_ids;

            capacity = new_capacity;
        }

        const unsigned char *title =
            sqlite3_column_text(statement, 1);

        const unsigned char *input_text =
            sqlite3_column_text(statement, 2);

        const unsigned char *output_text =
            sqlite3_column_text(statement, 3);

        const unsigned char *input_type =
            sqlite3_column_text(statement, 4);

        const unsigned char *created_at =
            sqlite3_column_text(statement, 5);

        const unsigned char *updated_at =
            sqlite3_column_text(statement, 6);

        title_array[item_count] =
            database_duplicate_text(
                title != NULL
                    ? (const char *)title
                    : ""
            );

        input_array[item_count] =
            database_duplicate_text(
                input_text != NULL
                    ? (const char *)input_text
                    : ""
            );

        output_array[item_count] =
            database_duplicate_text(
                output_text != NULL
                    ? (const char *)output_text
                    : ""
            );

        type_array[item_count] =
            database_duplicate_text(
                input_type != NULL
                    ? (const char *)input_type
                    : ""
            );

        created_array[item_count] =
            database_duplicate_text(
                created_at != NULL
                    ? (const char *)created_at
                    : ""
            );

        updated_array[item_count] =
            database_duplicate_text(
                updated_at != NULL
                    ? (const char *)updated_at
                    : ""
            );

        id_array[item_count] =
            sqlite3_column_int(statement, 0);

        if (title_array[item_count] == NULL ||
            input_array[item_count] == NULL ||
            output_array[item_count] == NULL ||
            type_array[item_count] == NULL ||
            created_array[item_count] == NULL ||
            updated_array[item_count] == NULL)
        {
            for (int i = 0; i <= item_count; i++)
            {
                free(title_array[i]);
                free(input_array[i]);
                free(output_array[i]);
                free(type_array[i]);
                free(created_array[i]);
                free(updated_array[i]);
            }

            free(title_array);
            free(input_array);
            free(output_array);
            free(type_array);
            free(created_array);
            free(updated_array);
            free(id_array);

            sqlite3_finalize(statement);

            return 0;
        }

        item_count++;
    }

    if (result != SQLITE_DONE)
    {
        for (int i = 0; i < item_count; i++)
        {
            free(title_array[i]);
            free(input_array[i]);
            free(output_array[i]);
            free(type_array[i]);
            free(created_array[i]);
            free(updated_array[i]);
        }

        free(title_array);
        free(input_array);
        free(output_array);
        free(type_array);
        free(created_array);
        free(updated_array);
        free(id_array);

        sqlite3_finalize(statement);

        return 0;
    }

    sqlite3_finalize(statement);

    *titles = title_array;
    *inputs = input_array;
    *outputs = output_array;
    *input_types = type_array;
    *created_ats = created_array;
    *updated_ats = updated_array;
    *saved_ids = id_array;
    *count = item_count;

    return 1;
}

int database_get_user_statistics(
    Database *database,
    int user_id,
    int *total_translations,
    int *saved_translations,
    int *text_to_morse,
    int *morse_to_text)
{
    if (database == NULL ||
        database->connection == NULL ||
        total_translations == NULL ||
        saved_translations == NULL ||
        text_to_morse == NULL ||
        morse_to_text == NULL)
    {
        return 0;
    }

    *total_translations = 0;
    *saved_translations = 0;
    *text_to_morse = 0;
    *morse_to_text = 0;

    const char *history_sql =
        "SELECT "
        "COUNT(*), "
        "SUM(CASE WHEN input_type = 'TEXT_TO_MORSE' THEN 1 ELSE 0 END), "
        "SUM(CASE WHEN input_type = 'MORSE_TO_TEXT' THEN 1 ELSE 0 END) "
        "FROM translation_history "
        "WHERE user_id = ?;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        history_sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        *total_translations = sqlite3_column_int(statement, 0);
        *text_to_morse = sqlite3_column_int(statement, 1);
        *morse_to_text = sqlite3_column_int(statement, 2);
    }
    else
    {
        sqlite3_finalize(statement);
        return 0;
    }

    sqlite3_finalize(statement);

    const char *saved_sql =
        "SELECT COUNT(*) "
        "FROM saved_translations "
        "WHERE user_id = ?;";

    statement = NULL;

    result = sqlite3_prepare_v2(
        database->connection,
        saved_sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        *saved_translations = sqlite3_column_int(statement, 0);
    }
    else
    {
        sqlite3_finalize(statement);
        return 0;
    }

    sqlite3_finalize(statement);

    return 1;
}

int database_get_weekly_translation_count(
    Database *database,
    int user_id,
    int *weekly_translations)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0 ||
        weekly_translations == NULL)
    {
        return 0;
    }

    *weekly_translations = 0;

    const char *sql =
        "SELECT COUNT(*) "
        "FROM translation_history "
        "WHERE user_id = ? "
        "AND created_at >= datetime('now', '-7 days');";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);
    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        *weekly_translations = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);

    return result == SQLITE_ROW;
}

int database_get_activity_notifications_enabled(
    Database *database,
    int user_id,
    int *enabled)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0 ||
        enabled == NULL)
    {
        return 0;
    }

    *enabled = 0;

    const char *sql =
        "SELECT activity_notifications_enabled "
        "FROM user_preferences WHERE user_id = ? LIMIT 1;";

    sqlite3_stmt *statement = NULL;
    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);
    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        *enabled = sqlite3_column_int(statement, 0);
        result = SQLITE_OK;
    }
    else if (result == SQLITE_DONE)
    {
        /* Existing accounts default to notifications disabled. */
        result = SQLITE_OK;
    }

    sqlite3_finalize(statement);
    return result == SQLITE_OK;
}

int database_set_activity_notifications_enabled(
    Database *database,
    int user_id,
    int enabled)
{
    if (database == NULL ||
        database->connection == NULL ||
        user_id <= 0 ||
        (enabled != 0 && enabled != 1))
    {
        return 0;
    }

    const char *sql =
        "INSERT INTO user_preferences "
        "(user_id, activity_notifications_enabled) VALUES (?, ?) "
        "ON CONFLICT(user_id) DO UPDATE SET "
        "activity_notifications_enabled = excluded.activity_notifications_enabled;";

    sqlite3_stmt *statement = NULL;
    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(statement, 1, user_id);
    sqlite3_bind_int(statement, 2, enabled);
    result = sqlite3_step(statement);

    sqlite3_finalize(statement);
    return result == SQLITE_DONE;
}

int database_get_user_password_hash(
    Database *database,
    int user_id,
    char *password_hash,
    int password_hash_size)
{
    if (database == NULL ||
        database->connection == NULL ||
        password_hash == NULL ||
        password_hash_size <= 0)
    {
        return 0;
    }

    const char *sql =
        "SELECT password_hash "
        "FROM users "
        "WHERE id = ? "
        "LIMIT 1;";

    sqlite3_stmt *statement = NULL;

    int result = sqlite3_prepare_v2(
        database->connection,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return 0;
    }

    sqlite3_bind_int(
        statement,
        1,
        user_id
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_ROW)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    const unsigned char *stored_hash =
        sqlite3_column_text(statement, 0);

    if (stored_hash == NULL)
    {
        sqlite3_finalize(statement);
        return 0;
    }

    snprintf(
        password_hash,
        password_hash_size,
        "%s",
        (const char *)stored_hash
    );

    sqlite3_finalize(statement);

    return 1;
}
