# MORSELAB C Backend

The backend is a C17 application built with libmicrohttpd, SQLite, and OpenSSL. `src/main.c` opens `backend/database/morselab.db`, initializes the schema, creates the Morse translator, and starts the HTTP server on port 8080.

## Build prerequisites

Use the existing MSYS2 UCRT64 toolchain. GCC must be on `PATH`, with the UCRT64 headers and link libraries for libmicrohttpd, SQLite, and OpenSSL available. GNU Make is used by the provided Makefile.

From this directory:

```sh
make
```

The Makefile compiles the C files in `src/` and writes `morselab.exe` in this directory. Its link dependencies are `microhttpd`, `sqlite3`, and `crypto`. To override the compiler or flags, pass standard Make variables, for example `make CC=gcc CFLAGS="-std=c17 -Wall -Wextra -O0 -g"`.

## Run

Run the executable with the **ProjectC directory** as the working directory so the relative database path resolves correctly:

```sh
cd ProjectC
./backend/morselab.exe
```

The server listens at `http://localhost:8080`. It prints its health endpoint at `GET /api/health` and waits for Enter before shutting down. Start the frontend separately from `frontend/` with `npm run dev`.

## Tests

Standalone C tests are in `tests/`. The auth and database tests use relative paths under `backend/database/`; run them from an isolated test working directory if you need to preserve the checked-in database files. Their source files can be compiled against `src/auth.c`, `src/database.c`, and the relevant system libraries.
