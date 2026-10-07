# MORSELAB

MORSELAB is a local Morse translation application. Its React/TypeScript frontend calls a C backend over HTTP; the backend uses libmicrohttpd, SQLite, OpenSSL password hashing, and the project's Morse data structures.

## Existing features

- Account signup, login, session validation, logout, password change, profile editing, and account deletion
- Text-to-Morse and Morse-to-text translation for A–Z and 0–9
- Automatically recorded translation history, with search, direction filtering, copy, and deletion
- Saved translations with list, edit, copy, and delete operations
- Per-user totals and translation-direction statistics, including translations from the last seven days
- Per-user Activity Notifications preference and a browser Compact Interface preference

## Architecture

- **Frontend:** React 19, TypeScript, React Router, Vite, and Tailwind CSS, under `frontend/`
- **Backend:** C17, libmicrohttpd, SQLite, and OpenSSL, under `backend/`
- **Database:** SQLite tables for users, translation history, saved translations, and user preferences
- **DSA:** A hash table and binary Morse tree power translation. Stack, queue, and the general translation linked list are standalone modules with unit tests; they are not used by application features.

The backend stores session tokens in process memory for 24 hours. Restarting the backend invalidates active sessions. User notification preferences are stored in SQLite; compact mode is stored in the browser's local storage.

## Local development

Use an MSYS2 UCRT64 environment with GCC and the SQLite, OpenSSL, and libmicrohttpd development libraries. The backend Makefile describes the source and link dependencies. From `ProjectC/backend`, build with `make` (or `mingw32-make` if that is the installed GNU Make command):

```sh
cd ProjectC/backend
make
```

Run the executable with `ProjectC` as its working directory. The relative database path is `backend/database/morselab.db`; the server listens on port 8080 and waits for Enter to stop:

```sh
cd ProjectC
./backend/morselab.exe
```

In another terminal, install frontend dependencies if needed and start Vite:

```sh
cd ProjectC/frontend
npm ci
npm run dev
```

Open the local URL printed by Vite. Frontend API calls target `http://localhost:8080`; start the C backend separately. `GET /api/health` is the backend health check.

## Documentation

- [API endpoints](docs/api.md)
- [Architecture](docs/architecture.md)
- [Database](docs/database.md)
- [DSA modules](docs/dsa.md)
- [Backend build and run](backend/README.md)
