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

## Deployment preparation

### Local frontend configuration

The frontend reads its backend origin from `VITE_API_BASE_URL`. For local development, copy the example file and start Vite as usual:

```sh
cd ProjectC/frontend
cp .env.example .env.local
npm ci
npm run dev
```

The example points to `http://localhost:8080`. If the variable is absent, the frontend uses that same local development fallback. The C backend continues to default to port `8080`; its existing SQLite path is relative to the ProjectC working directory.

### GitHub Pages frontend

The workflow in `.github/workflows/deploy-pages.yml` builds and deploys the frontend to the repository Pages site at `https://sahil6000.github.io/MorseLab-Translator/`. It keeps `/` as the local Vite base and uses `/MorseLab-Translator/` in GitHub Actions. The existing `BrowserRouter` is given the matching base path, and `frontend/public/404.html` restores direct visits to application routes.

Before running the workflow, enable **Settings → Pages → Build and deployment → GitHub Actions** and add a repository Actions variable named `VITE_API_BASE_URL` with the actual deployed backend origin. The workflow stops with an error if this variable is missing; no production backend URL is included in the source. Vite embeds this public API origin at build time.

### C backend container

The root `Dockerfile` builds the existing C backend on Debian and starts `backend/morselab.exe`. It honors the hosting platform's `PORT` variable and defaults to `8080`. The current libmicrohttpd startup does not set a socket address, so it accepts connections on all interfaces. To build and run the container locally:

```sh
docker build -t morselab-backend .
docker run --rm -p 8080:8080 -e PORT=8080 \
  -v morselab-data:/app/backend/database morselab-backend
```

The container opens `backend/database/morselab.db` relative to `/app`, creating the database there when needed. Many cloud services use ephemeral container filesystems; without a persistent volume mounted at `/app/backend/database`, SQLite data can be lost when the service is replaced or restarted. Configure a persistent disk at that path when the hosting platform supports one. The application remains on SQLite and does not migrate data.

To deploy, configure the Pages setting and API variable, then push the approved source to `main` or run the workflow manually. Build and deploy the Dockerfile using the cloud provider's container service, set its `PORT` value, and use the provider-assigned backend URL for `VITE_API_BASE_URL`.

## Documentation

- [API endpoints](docs/api.md)
- [Architecture](docs/architecture.md)
- [Database](docs/database.md)
- [DSA modules](docs/dsa.md)
- [Backend build and run](backend/README.md)
