# MORSELAB Architecture

## Frontend

`frontend/src/main.tsx` mounts the React app under `StrictMode` and `BrowserRouter`. `App.tsx` defines the public landing, login, and signup routes and the protected dashboard, translator, history, saved, statistics, profile, and settings routes. Protected routes call `POST /api/auth/validate` before rendering. If validation fails, browser session data is cleared and the app redirects to login.

Page components call the backend directly with `fetch` and JSON request bodies. The API base URL is currently `http://localhost:8080`; Vite does not proxy the backend. `index.css` imports Tailwind and contains compact-mode rules for the existing translator and history layouts.

## C backend

`backend/src/main.c` initializes the database and translator, then starts a libmicrohttpd server on port 8080. `backend/src/server.c` contains request-body collection, JSON extraction/escaping, CORS response headers, route dispatch, and the endpoint handlers. Requests have a fixed 8192-byte buffer. The backend uses OpenSSL for PBKDF2 password hashing and random session tokens.

`backend/src/database.c` owns SQLite schema initialization and prepared-statement operations for users, history, saved translations, statistics, and notification preferences. The translator is initialized in `morse.c`; encoding uses `hash_table.c`, and decoding uses `morse_tree.c`.

## SQLite and state

The database file path in `main.c` is relative: `backend/database/morselab.db`. Start the process with `ProjectC` as its current working directory. Tables are created with `CREATE TABLE IF NOT EXISTS`, so the current schema initializer adds the preferences table to an existing database without replacing existing tables or rows.

Account data, translation history, saved translations, and the Activity Notifications preference are persisted in SQLite. Foreign-key cascades remove a user's history, saved rows, and preferences during account deletion. Session tokens remain in a linked list in process memory and are not persisted. Compact Interface is a browser-local preference.

## Build and local development

Build the backend from `backend/` with its Makefile, run it from `ProjectC/`, and run the frontend separately with Vite from `frontend/`. See [backend build/run instructions](../backend/README.md) and the [API reference](api.md).
