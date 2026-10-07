# MORSELAB SQLite Database

The runtime database path is `backend/database/morselab.db` relative to the `ProjectC` directory. `database_initialize_schema()` in `backend/src/database.c` enables foreign keys and creates the tables if absent. The same table definitions are recorded in `backend/database/schema.sql`. Schema initialization is non-destructive for existing tables; it adds the `user_preferences` table with `CREATE TABLE IF NOT EXISTS`.

## Tables

### `users`

| Column | Definition |
|---|---|
| `id` | Integer primary key, autoincrement |
| `name` | Required text |
| `username` | Required unique text |
| `email` | Required unique text |
| `password_hash` | Required text containing the encoded PBKDF2 hash, salt, and iteration count |
| `created_at` | Datetime, defaults to `CURRENT_TIMESTAMP` |
| `last_login` | Datetime, nullable; updated on successful login |

### `translation_history`

| Column | Definition |
|---|---|
| `id` | Integer primary key, autoincrement |
| `user_id` | Required reference to `users.id`, `ON DELETE CASCADE` |
| `title` | Required text, defaults to `Untitled` |
| `input_text`, `output_text` | Required text |
| `input_type` | Required direction marker (`TEXT_TO_MORSE` or `MORSE_TO_TEXT`) |
| `created_at`, `updated_at` | Datetime values defaulting to `CURRENT_TIMESTAMP` |

Successful translations insert history rows. The current history API does not update history titles or rows; it lists and deletes them.

### `saved_translations`

This table has the same ID, user reference, title, input/output, direction, and timestamp fields as history. The user reference cascades on account deletion. The backend supports create, list, update, and delete operations scoped to the authenticated user.

### `user_preferences`

| Column | Definition |
|---|---|
| `user_id` | Primary key and reference to `users.id`, `ON DELETE CASCADE` |
| `activity_notifications_enabled` | Required integer, defaults to `0`, constrained to `0` or `1` |

This is the persisted per-user Activity Notifications preference. A missing row is read as disabled (`0`) until a user changes the setting.

## Relationships and session state

Each history row, saved translation, and preferences row belongs to one user. SQLite foreign-key enforcement is enabled on the backend connection, so deleting a user cascades to those rows. Authentication sessions are not database records: they live in backend memory and are removed on logout, account deletion, expiry, or backend restart. Compact Interface remains a browser-local setting and is not stored in SQLite.

`morselab_backup.db`, `test_auth.db`, and `test_morselab.db` are separate files already present in the project; the normal backend executable opens only `morselab.db` using its relative path.
