# MORSELAB HTTP API

The C/libmicrohttpd server listens on port `8080`. JSON requests use `Content-Type: application/json`; session tokens are passed in the JSON body. Responses are JSON. The server also handles CORS preflight requests.

## Health

| Method and path | Body | Result |
|---|---|---|
| `GET /api/health` | none | `{ "status": "ok", "service": "MorseLab C Backend" }` |

## Authentication and sessions

| Method and path | Request body | Result |
|---|---|---|
| `POST /api/auth/signup` | `{ "name", "username", "email", "password" }` | `201`; success and user `id`/`name`. Signup does not create a session. |
| `POST /api/auth/login` | `{ "email", "password" }` | `200`; success, user `id`/`name`, and session `token`. |
| `POST /api/auth/validate` | `{ "token" }` | `200`; `authenticated: true` and `user_id`, or `401` for invalid/expired token. |
| `POST /api/auth/logout` | `{ "token" }` | Destroys that token's session. |
| `POST /api/auth/change-password` | `{ "token", "current_password", "new_password" }` | Verifies the current password and updates the hash. |

Sessions are held in backend memory, expire after 24 hours, and are invalidated by a backend restart. Passwords are hashed using PBKDF2-HMAC-SHA256.

## Translation

Both translation routes require `{ "token", "text" }`. A successful response contains `success`, `input`, and `output`. Every successful translation is also inserted into translation history.

| Method and path | Direction |
|---|---|
| `POST /api/translate/text-to-morse` | Text to Morse |
| `POST /api/translate/morse-to-text` | Morse to text |

The current mapping supports letters A–Z and digits 0–9. Spaces between words are represented by `/` in Morse input/output.

## History

| Method and path | Request body | Result |
|---|---|---|
| `POST /api/history` | `{ "token" }` | `history` array of `{ id, input, output, input_type, created_at }` |
| `DELETE /api/history` | `{ "token", "history_id" }` | Deletes that user's history row. |

## Saved translations

| Method and path | Request body | Result |
|---|---|---|
| `POST /api/saved` | `{ "token", "title", "input", "output", "input_type" }` | Creates a saved translation and returns `saved_id`. |
| `POST /api/saved/list` | `{ "token" }` | `saved` array with IDs, title, input/output, direction, and timestamps. |
| `POST /api/saved/update` | `{ "token", "saved_id", "title", "input", "output", "input_type" }` | Updates that user's saved row. |
| `DELETE /api/saved` | `{ "token", "saved_id" }` | Deletes that user's saved row. |

`input_type` is `TEXT_TO_MORSE` or `MORSE_TO_TEXT`.

## Statistics and preferences

| Method and path | Request body | Result |
|---|---|---|
| `POST /api/statistics` | `{ "token" }` | Totals for translations, saved translations, directions, and `weekly_translations` from the last seven days. |
| `POST /api/preferences` | `{ "token" }` | Reads `preferences.activity_notifications_enabled` as `0` or `1`. |
| `POST /api/preferences` | `{ "token", "activity_notifications_enabled": 0 }` or `1` | Saves and returns the authenticated user's notification preference. |

## Profile and account

| Method and path | Request body | Result |
|---|---|---|
| `POST /api/profile` | `{ "token" }` | Profile fields, creation/last-login timestamps, and translation count. |
| `POST /api/profile/update` | `{ "token", "name", "email", "username" }` | Updates the authenticated user's profile. |
| `POST /api/profile/delete` | `{ "token" }` | Deletes the account and its cascading database rows, then revokes all of that user's active sessions. |

All history, saved-item, statistics, preference, and profile operations validate the supplied session token and scope database operations to its user ID.
