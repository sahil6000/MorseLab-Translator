# 🟣 MORSELAB

## Advanced Full-Stack Morse Code Translator

**DSA in C • Custom C HTTP Backend • SQLite • React • TypeScript • Vite**

[![GitHub Repository](https://img.shields.io/badge/GitHub-MorseLab--Translator-181717?style=for-the-badge&logo=github)](https://github.com/sahil6000/MorseLab-Translator)
[![Live Frontend](https://img.shields.io/badge/Live%20Frontend-GitHub%20Pages-222222?style=for-the-badge&logo=githubpages)](https://sahil6000.github.io/MorseLab-Translator/)
[![Backend](https://img.shields.io/badge/Backend-Live-46E3B7?style=for-the-badge&logo=render)](https://morselab-translator.onrender.com/api/health)

![React](https://img.shields.io/badge/Frontend-React%2019%20%7C%20TypeScript%20%7C%20Vite-61DAFB?style=flat-square&logo=react)
![C](https://img.shields.io/badge/Backend-C17%20%7C%20libmicrohttpd-A8B9CC?style=flat-square&logo=c)
![SQLite](https://img.shields.io/badge/Database-SQLite-003B57?style=flat-square&logo=sqlite)
![Tailwind CSS](https://img.shields.io/badge/Styling-Tailwind%20CSS-06B6D4?style=flat-square&logo=tailwindcss)
![Vite](https://img.shields.io/badge/Build-Vite-646CFF?style=flat-square&logo=vite)

> A complete full-stack application demonstrating how Data Structures and Algorithms can be integrated into a real-world web application using a custom C backend, SQLite database, and modern React frontend.

------------------------------------------------------------------------

## 📌 Table of Contents

-   [Overview](#-overview)
-   [Project Goals](#-project-goals)
-   [Core Features](#-core-features)
-   [Live Deployment](#-live-deployment)
-   [Technology Stack](#-technology-stack)
-   [System Architecture](#-system-architecture)
-   [Application Flow](#-application-flow)
-   [Frontend Architecture](#-frontend-architecture)
-   [Backend Architecture](#-backend-architecture)
-   [DSA Implementation](#-dsa-implementation)
-   [Morse Translation Engine](#-morse-translation-engine)
-   [Authentication and Security](#-authentication-and-security)
-   [Database Design](#-database-design)
-   [API Documentation](#-api-documentation)
-   [Frontend Routes](#-frontend-routes)
-   [Project Structure](#-project-structure)
-   [Local Development](#-local-development)
-   [Backend Build and Run](#-backend-build-and-run)
-   [Frontend Build and Run](#-frontend-build-and-run)
-   [Environment Configuration](#-environment-configuration)
-   [Deployment Architecture](#-deployment-architecture)
-   [Testing and Validation](#-testing-and-validation)
-   [Error Handling](#-error-handling)
-   [Performance and Complexity](#-performance-and-complexity)
-   [Design Principles](#-design-principles)
-   [Security Considerations](#-security-considerations)
-   [Deployment Limitation](#-deployment-limitation)
-   [Future Improvements](#-future-improvements)
-   [Learning Outcomes](#-learning-outcomes)
-   [Documentation](#-documentation)
-   [Author](#-author)
-   [License](#-license)

------------------------------------------------------------------------

# 🔭 Overview

**MORSELAB** is a full-stack Morse code translation platform designed
around a strict academic requirement: the application backend must be
implemented in **C with Data Structures and Algorithms**, while the
user-facing application is delivered through a modern web frontend.

Instead of treating DSA as separate classroom programs, MORSELAB
integrates multiple data structures directly into the application's
translation and data-management workflow.

The application provides:

-   Text → Morse translation
-   Morse → Text translation
-   User registration and login
-   Session-based authentication
-   Translation history
-   Saved translations
-   Saved translation editing and deletion
-   User statistics
-   Profile management
-   Password change
-   Account deletion
-   Protected frontend routes
-   REST-style HTTP API
-   SQLite persistence
-   Production deployment with GitHub Pages + Render

The project therefore combines **low-level systems programming, DSA,
database programming, HTTP networking, authentication, frontend
engineering, API integration, and deployment** in one application.

------------------------------------------------------------------------

# 🎯 Project Goals

MORSELAB was built with the following goals:

  -----------------------------------------------------------------------
  Goal                                Implementation
  ----------------------------------- -----------------------------------
  Apply DSA in a real system          Hash table, binary tree, linked
                                      list, stack and queue modules in C

  Build a real backend in C           Custom HTTP server using
                                      `libmicrohttpd`

  Implement persistent application    SQLite database
  data                                

  Build a modern web interface        React + TypeScript + Vite

  Support secure authentication       PBKDF2-HMAC-SHA256 password
                                      hashing + session tokens

  Demonstrate API communication       React frontend communicates with C
                                      backend through HTTP/JSON

  Provide complete user workflows     Auth, translation, history, saved
                                      items, statistics and profile

  Deploy the application              GitHub Pages frontend + Render
                                      backend

  Maintain a structured codebase      Separate frontend, backend, DSA,
                                      database and documentation layers
  -----------------------------------------------------------------------

------------------------------------------------------------------------

# ✨ Core Features

## 🔤 Translation

### Text → Morse

Converts normal text into Morse code.

Example:

``` text
HELLO WORLD
```

becomes:

``` text
.... . .-.. .-.. --- / .-- --- .-. .-.. -..
```

### Morse → Text

Converts Morse code back into readable text.

Example:

``` text
.... . .-.. .-.. ---
```

becomes:

``` text
HELLO
```

------------------------------------------------------------------------

## 👤 Authentication

MORSELAB includes a complete account lifecycle:

-   User signup
-   Email-based login
-   Password verification
-   Session creation
-   Session validation
-   Logout
-   Password change
-   Account deletion
-   Session invalidation after account deletion

The frontend also protects application routes so authenticated features
are not directly accessible to unauthenticated users.

------------------------------------------------------------------------

## 🕘 Translation History

Successful translations can be stored in the database and later
retrieved for the authenticated user.

History functionality includes:

-   Create history entry
-   View user-specific history
-   Delete history entry
-   Timestamped records
-   Input/output type tracking

------------------------------------------------------------------------

## ⭐ Saved Translations

Users can save useful translations for later use.

Supported operations:

-   Create saved translation
-   List saved translations
-   Update saved translation
-   Delete saved translation

Each saved translation can contain:

-   Title
-   Input text
-   Output text
-   Input type
-   Creation timestamp
-   Update timestamp

------------------------------------------------------------------------

## 📊 Statistics

The backend calculates user-specific translation statistics including:

-   Total translations
-   Saved translations
-   Text → Morse count
-   Morse → Text count

This data is presented through the Statistics page.

------------------------------------------------------------------------

## 👤 Profile Management

Users can:

-   View profile information
-   Update profile information
-   Change password
-   Delete account

Account deletion also removes the user's related records through the
database's foreign-key cascade configuration and invalidates active
authentication sessions.

------------------------------------------------------------------------

# 🌐 Live Deployment

  -----------------------------------------------------------------------------------------------------
  Component               Deployment              URL
  ----------------------- ----------------------- -----------------------------------------------------
  Frontend                GitHub Pages            https://sahil6000.github.io/MorseLab-Translator/

  Backend                 Render Web Service      https://morselab-translator.onrender.com

  Backend Health          Render API              https://morselab-translator.onrender.com/api/health

  Source Code             GitHub                  https://github.com/sahil6000/MorseLab-Translator
  -----------------------------------------------------------------------------------------------------

### Production request flow

``` mermaid
flowchart LR
    U[User Browser] --> F[GitHub Pages<br/>React + TypeScript]
    F -->|HTTPS / JSON API| B[Render<br/>C HTTP Backend]
    B --> D[(SQLite Database)]
    B --> M[Morse Translation Engine]
    M --> D
```

------------------------------------------------------------------------

# 🧰 Technology Stack

## Frontend

  Technology     Purpose
  -------------- --------------------------------------------
  React 19       Component-based UI
  TypeScript     Static typing
  Vite           Development server and production bundling
  React Router   Client-side routing
  Tailwind CSS   UI styling
  Lucide React   Interface icons
  Fetch API      Frontend ↔ backend communication

## Backend

  Technology             Purpose
  ---------------------- -----------------------------------------------
  C                      Core backend language
  GNU GCC                C compiler
  GNU Make               Build automation
  `libmicrohttpd`        Embedded HTTP server
  OpenSSL                Password hashing and cryptographic operations
  SQLite3                Relational persistence
  Custom JSON handling   Request/response processing

## Data Structures and Algorithms

  -----------------------------------------------------------------------
  Structure                           Role
  ----------------------------------- -----------------------------------
  Hash Table                          Fast character → Morse lookup
                                      during encoding

  Binary Tree                         Morse code → character decoding

  Linked List                         Dynamic translation records /
                                      in-memory list operations

  Stack                               LIFO data structure implementation

  Queue                               FIFO operation management

  String processing                   Translation and request parsing

  Dynamic memory                      Runtime allocation and cleanup
  -----------------------------------------------------------------------

## Deployment

  Service          Responsibility
  ---------------- ---------------------------------------
  GitHub           Source control and repository hosting
  GitHub Actions   Automated frontend build/deployment
  GitHub Pages     Production frontend hosting
  Render           Production C backend hosting
  Docker           Backend containerization for Render

------------------------------------------------------------------------

# 🏗️ System Architecture

MORSELAB follows a layered architecture:

``` mermaid
flowchart TB

    subgraph CLIENT["Presentation Layer"]
        UI["React UI"]
        ROUTER["React Router"]
        STATE["Component State"]
    end

    subgraph API["Application/API Layer"]
        FETCH["API Utility"]
        HTTP["HTTP / JSON"]
        SERVER["C HTTP Server"]
    end

    subgraph LOGIC["Business Logic Layer"]
        AUTH["Authentication"]
        TRANS["Translation Service"]
        HISTORY["History Service"]
        SAVED["Saved Translation Service"]
        STATS["Statistics Service"]
        PROFILE["Profile Service"]
    end

    subgraph DSA["DSA Layer"]
        HASH["Hash Table"]
        TREE["Morse Binary Tree"]
        LIST["Linked List"]
        STACK["Stack"]
        QUEUE["Queue"]
    end

    subgraph DATA["Persistence Layer"]
        SQLITE["SQLite"]
        SCHEMA["Database Schema"]
    end

    UI --> ROUTER
    ROUTER --> STATE
    STATE --> FETCH
    FETCH --> HTTP
    HTTP --> SERVER

    SERVER --> AUTH
    SERVER --> TRANS
    SERVER --> HISTORY
    SERVER --> SAVED
    SERVER --> STATS
    SERVER --> PROFILE

    TRANS --> HASH
    TRANS --> TREE
    HISTORY --> LIST
    AUTH --> SQLITE
    HISTORY --> SQLITE
    SAVED --> SQLITE
    STATS --> SQLITE
    PROFILE --> SQLITE
    SQLITE --> SCHEMA
```

------------------------------------------------------------------------

# 🔄 Application Flow

## Translation Flow

``` mermaid
sequenceDiagram
    participant User
    participant React
    participant CServer as C HTTP Server
    participant Morse as Morse Engine
    participant DSA as DSA Structures
    participant DB as SQLite

    User->>React: Enter translation
    React->>CServer: POST /api/translate/*
    CServer->>Morse: Process input

    alt Text → Morse
        Morse->>DSA: Hash Table lookup
        DSA-->>Morse: Morse symbols
    else Morse → Text
        Morse->>DSA: Binary Tree traversal
        DSA-->>Morse: Characters
    end

    Morse-->>CServer: Translation result
    CServer->>DB: Store history
    DB-->>CServer: Success
    CServer-->>React: JSON response
    React-->>User: Display result
```

------------------------------------------------------------------------

# 🖥️ Frontend Architecture

The frontend is organized as a React single-page application.

## Public pages

-   `/`
-   `/login`
-   `/signup`

## Protected pages

-   `/dashboard`
-   `/translator`
-   `/history`
-   `/saved`
-   `/statistics`
-   `/profile`
-   `/settings`

Protected routes check for the local/session authentication token before
rendering private pages.

``` mermaid
flowchart TD
    ROOT["/"] --> LANDING["Landing"]
    ROOT --> LOGIN["/login"]
    ROOT --> SIGNUP["/signup"]

    LOGIN --> AUTH{"Authenticated?"}
    SIGNUP --> AUTH

    AUTH -->|Yes| DASH["/dashboard"]
    AUTH -->|No| LOGIN

    DASH --> TRANS["/translator"]
    DASH --> HISTORY["/history"]
    DASH --> SAVED["/saved"]
    DASH --> STATS["/statistics"]
    DASH --> PROFILE["/profile"]
    DASH --> SETTINGS["/settings"]
```

------------------------------------------------------------------------

# ⚙️ Backend Architecture

The backend is written in C and is intentionally structured into
independent modules.

``` text
backend/
├── include/
│   ├── auth.h
│   ├── database.h
│   ├── hash_table.h
│   ├── linked_list.h
│   ├── morse.h
│   ├── morse_tree.h
│   ├── queue.h
│   ├── server.h
│   └── stack.h
│
├── src/
│   ├── auth.c
│   ├── database.c
│   ├── hash_table.c
│   ├── linked_list.c
│   ├── main.c
│   ├── morse.c
│   ├── morse_tree.c
│   ├── queue.c
│   ├── server.c
│   └── stack.c
│
├── database/
│   └── schema.sql
│
└── tests/
    ├── test_auth.c
    ├── test_database.c
    ├── test_hash_table.c
    ├── test_linked_list.c
    ├── test_morse.c
    ├── test_morse_tree.c
    ├── test_queue.c
    └── test_stack.c
```

### Backend startup lifecycle

``` text
Program start
     │
     ▼
Open SQLite database
     │
     ▼
Initialize database schema
     │
     ▼
Create Morse translator
     │
     ├── Build encoding hash table
     │
     └── Build decoding tree
     │
     ▼
Start libmicrohttpd server
     │
     ▼
Accept API requests
     │
     ▼
Route request
     │
     ▼
Execute business logic
     │
     ▼
Return JSON response
     │
     ▼
Graceful shutdown
```

------------------------------------------------------------------------

# 🧠 DSA Implementation

One of the defining characteristics of MORSELAB is that the backend's
core translation logic is implemented using classical data structures.

## 1. Hash Table --- Text → Morse

The hash table stores character-to-Morse mappings.

Conceptually:

``` text
'A' → ".-"
'B' → "-..."
'C' → "-.-."
...
'Z' → "--.."
```

The implementation uses buckets and linked entries.

``` text
Hash Table
│
├── Bucket 0
├── Bucket 1
├── Bucket 2
│     └── Entry
├── ...
└── Bucket 52
```

### Main operations

``` c
hash_table_create()
hash_table_insert()
hash_table_search()
hash_table_free()
```

### Why it is used

Encoding repeatedly needs to answer:

> "What Morse sequence corresponds to this character?"

A hash table provides efficient average-case lookup.

------------------------------------------------------------------------

## 2. Binary Tree --- Morse → Text

Morse decoding uses a binary tree where:

-   Dot moves to one child
-   Dash moves to the other child

Conceptual structure:

``` text
                    Root
                   /    \
                E (.)   T (-)
                / \      / \
             I   A     N   M
            / \ / \   / \ / \
           ... ...   ... ...
```

This makes Morse decoding a tree traversal problem.

### Main operations

``` c
morse_create_node()
morse_create_tree()
morse_insert()
morse_decode()
morse_free_tree()
```

------------------------------------------------------------------------

## 3. Linked List

The project includes a linked-list implementation for dynamic
translation records.

Each node contains:

``` text
ID
Input Text
Output Text
Input Type
Next Pointer
```

### Main operations

``` c
linked_list_create()
linked_list_append()
linked_list_find()
linked_list_delete()
linked_list_print()
linked_list_free()
```

------------------------------------------------------------------------

## 4. Stack

The stack implementation follows the **LIFO** principle:

``` text
Last In → First Out
```

Operations:

``` c
stack_push()
stack_pop()
stack_peek()
stack_is_empty()
stack_free()
```

------------------------------------------------------------------------

## 5. Queue

The queue implementation follows the **FIFO** principle:

``` text
First In → First Out
```

Operations:

``` c
queue_enqueue()
queue_dequeue()
queue_peek()
queue_is_empty()
queue_free()
```

------------------------------------------------------------------------

# 🔤 Morse Translation Engine

The translation engine combines two different DSA strategies.

``` mermaid
flowchart LR
    INPUT["Input Text"] --> CHECK{"Direction?"}

    CHECK -->|Text → Morse| HASH["Hash Table"]
    HASH --> ENCODE["Character Lookup"]
    ENCODE --> OUT1["Morse Output"]

    CHECK -->|Morse → Text| TREE["Binary Tree"]
    TREE --> TRAVERSE["Dot / Dash Traversal"]
    TRAVERSE --> OUT2["Text Output"]
```

### Encoding

``` text
Input character
      ↓
Normalize / process
      ↓
Hash table search
      ↓
Morse code found
      ↓
Append to output
```

### Decoding

``` text
Morse token
      ↓
Read dot / dash
      ↓
Traverse binary tree
      ↓
Reach character node
      ↓
Append decoded character
```

------------------------------------------------------------------------

# 🔐 Authentication and Security

MORSELAB includes a custom authentication subsystem in C.

## Password hashing

Passwords are not stored as plaintext.

The authentication implementation uses:

**PBKDF2-HMAC-SHA256**

with a configured iteration count of:

``` text
600,000 iterations
```

The stored password representation includes a version prefix:

``` text
v1$
```

and uses a generated salt.

------------------------------------------------------------------------

## Session authentication

After successful login, the backend creates a session token.

The backend keeps active session information in memory while the server
is running.

Session properties include:

-   User ID
-   Token
-   Creation time
-   Expiration time

Configured session duration:

``` text
24 hours
```

### Session flow

``` mermaid
sequenceDiagram
    participant Browser
    participant Backend
    participant DB

    Browser->>Backend: Login credentials
    Backend->>DB: Find user
    DB-->>Backend: Password hash
    Backend->>Backend: Verify PBKDF2 password
    Backend->>Backend: Create session token
    Backend-->>Browser: Auth token

    Browser->>Backend: Protected API + token
    Backend->>Backend: Validate token
    Backend-->>Browser: Authorized response
```

------------------------------------------------------------------------

## Account deletion

When an account is deleted:

1.  User data is removed from the database.
2.  Related records are handled through foreign-key cascade rules.
3.  Active authentication sessions for that user are destroyed.
4.  Future authenticated requests using those sessions fail.

------------------------------------------------------------------------

# 🗄️ Database Design

MORSELAB uses SQLite for relational persistence.

## Entity relationship

``` mermaid
erDiagram
    USERS ||--o{ TRANSLATION_HISTORY : owns
    USERS ||--o{ SAVED_TRANSLATIONS : owns

    USERS {
        INTEGER id PK
        TEXT name
        TEXT username UK
        TEXT email UK
        TEXT password_hash
        DATETIME created_at
        DATETIME last_login
    }

    TRANSLATION_HISTORY {
        INTEGER id PK
        INTEGER user_id FK
        TEXT title
        TEXT input_text
        TEXT output_text
        TEXT input_type
        DATETIME created_at
        DATETIME updated_at
    }

    SAVED_TRANSLATIONS {
        INTEGER id PK
        INTEGER user_id FK
        TEXT title
        TEXT input_text
        TEXT output_text
        TEXT input_type
        DATETIME created_at
        DATETIME updated_at
    }
```

## Tables

### `users`

Stores account information.

  Column            Type       Purpose
  ----------------- ---------- -----------------------
  `id`              INTEGER    Primary key
  `name`            TEXT       User display name
  `username`        TEXT       Unique username
  `email`           TEXT       Unique email
  `password_hash`   TEXT       Hashed password
  `created_at`      DATETIME   Account creation time
  `last_login`      DATETIME   Last successful login

### `translation_history`

Stores completed translations.

  Column          Type       Purpose
  --------------- ---------- -----------------------
  `id`            INTEGER    Primary key
  `user_id`       INTEGER    Owner
  `title`         TEXT       Record title
  `input_text`    TEXT       Original input
  `output_text`   TEXT       Translation output
  `input_type`    TEXT       Translation direction
  `created_at`    DATETIME   Creation time
  `updated_at`    DATETIME   Update time

### `saved_translations`

Stores user-selected saved translations.

  Column          Type       Purpose
  --------------- ---------- -----------------------
  `id`            INTEGER    Primary key
  `user_id`       INTEGER    Owner
  `title`         TEXT       Saved title
  `input_text`    TEXT       Original input
  `output_text`   TEXT       Translation output
  `input_type`    TEXT       Translation direction
  `created_at`    DATETIME   Creation time
  `updated_at`    DATETIME   Last update time

### Referential integrity

The database enables:

``` sql
PRAGMA foreign_keys = ON;
```

User-owned records use:

``` sql
ON DELETE CASCADE
```

so dependent history/saved records are removed when the owning account
is deleted.

------------------------------------------------------------------------

# 🔌 API Documentation

The C backend exposes JSON-based HTTP endpoints.

## Health

  Method   Endpoint        Purpose
  -------- --------------- ----------------------
  GET      `/api/health`   Backend health check

Example response:

``` json
{
  "status": "ok",
  "service": "MorseLab C Backend"
}
```

------------------------------------------------------------------------

## Authentication

  Method   Endpoint                      Purpose
  -------- ----------------------------- ---------------------
  POST     `/api/auth/signup`            Register a user
  POST     `/api/auth/login`             Authenticate a user
  POST     `/api/auth/validate`          Validate session
  POST     `/api/auth/logout`            Destroy session
  POST     `/api/auth/change-password`   Change password

------------------------------------------------------------------------

## Translation

  Method   Endpoint                         Purpose
  -------- -------------------------------- -----------------------
  POST     `/api/translate/text-to-morse`   Convert text to Morse
  POST     `/api/translate/morse-to-text`   Convert Morse to text

Typical request shape:

``` json
{
  "text": "HELLO WORLD"
}
```

------------------------------------------------------------------------

## History

  Method   Endpoint         Purpose
  -------- ---------------- -------------------------------
  POST     `/api/history`   Create/read history operation
  DELETE   `/api/history`   Delete a history item

------------------------------------------------------------------------

## Saved translations

  Method   Endpoint              Purpose
  -------- --------------------- --------------------------
  POST     `/api/saved`          Create saved translation
  POST     `/api/saved/list`     List saved translations
  POST     `/api/saved/update`   Update saved translation
  DELETE   `/api/saved`          Delete saved translation

------------------------------------------------------------------------

## Statistics

  Method   Endpoint            Purpose
  -------- ------------------- ---------------------------------
  POST     `/api/statistics`   Get user translation statistics

------------------------------------------------------------------------

## Profile

  Method   Endpoint                Purpose
  -------- ----------------------- ----------------
  POST     `/api/profile`          Read profile
  POST     `/api/profile/update`   Update profile
  POST     `/api/profile/delete`   Delete account

> The backend uses JSON requests/responses and C-level request routing.
> Protected operations require an authenticated session.

------------------------------------------------------------------------

# 🧭 Frontend Routes

  Route           Access      Page
  --------------- ----------- ---------------------
  `/`             Public      Landing page
  `/login`        Public      Login
  `/signup`       Public      Signup
  `/dashboard`    Protected   Dashboard
  `/translator`   Protected   Translator
  `/history`      Protected   Translation history
  `/saved`        Protected   Saved translations
  `/statistics`   Protected   Statistics
  `/profile`      Protected   Profile
  `/settings`     Protected   Settings

------------------------------------------------------------------------

# 📁 Project Structure

``` text
MorseLab-Translator/
│
├── .github/
│   └── workflows/
│       └── deploy-pages.yml
│
├── .vscode/
│   ├── settings.json
│   ├── launch.json
│   └── c_cpp_properties.json
│
├── backend/
│   ├── database/
│   │   └── schema.sql
│   │
│   ├── include/
│   │   ├── auth.h
│   │   ├── database.h
│   │   ├── hash_table.h
│   │   ├── linked_list.h
│   │   ├── morse.h
│   │   ├── morse_tree.h
│   │   ├── queue.h
│   │   ├── server.h
│   │   └── stack.h
│   │
│   ├── src/
│   │   ├── auth.c
│   │   ├── database.c
│   │   ├── hash_table.c
│   │   ├── linked_list.c
│   │   ├── main.c
│   │   ├── morse.c
│   │   ├── morse_tree.c
│   │   ├── queue.c
│   │   ├── server.c
│   │   └── stack.c
│   │
│   ├── tests/
│   │   ├── test_auth.c
│   │   ├── test_database.c
│   │   ├── test_hash_table.c
│   │   ├── test_linked_list.c
│   │   ├── test_morse.c
│   │   ├── test_morse_tree.c
│   │   ├── test_queue.c
│   │   └── test_stack.c
│   │
│   ├── Makefile
│   └── README.md
│
├── docs/
│   ├── api.md
│   ├── architecture.md
│   ├── database.md
│   └── dsa.md
│
├── frontend/
│   ├── public/
│   ├── src/
│   │   ├── assets/
│   │   ├── pages/
│   │   ├── utils/
│   │   ├── App.tsx
│   │   ├── App.css
│   │   ├── index.css
│   │   └── main.tsx
│   │
│   ├── .env.example
│   ├── index.html
│   ├── package.json
│   ├── package-lock.json
│   ├── tsconfig.json
│   └── vite.config.ts
│
├── .dockerignore
├── .gitignore
├── Dockerfile
└── README.md
```

------------------------------------------------------------------------

# 💻 Local Development

## Prerequisites

Install:

-   Git
-   Node.js and npm
-   MSYS2 UCRT64
-   GCC
-   GNU Make
-   SQLite3
-   OpenSSL
-   `libmicrohttpd`
-   A modern browser
-   VS Code recommended

------------------------------------------------------------------------

# 🧱 Backend Build and Run

MORSELAB's C backend is designed to run in the **MSYS2 UCRT64**
environment on Windows.

Open the **MSYS2 UCRT64** terminal:

``` bash
cd /c/Users/LENOVO/Desktop/MorseLab/ProjectC
```

Build the backend using the project's build configuration.

If using the development executable directly after compilation:

``` bash
./backend/morselab.exe
```

The backend starts on:

``` text
http://localhost:8080
```

Health endpoint:

``` text
http://localhost:8080/api/health
```

Expected response:

``` json
{
  "status": "ok",
  "service": "MorseLab C Backend"
}
```

### Why UCRT64?

The backend depends on the MSYS2 UCRT64 toolchain and runtime libraries.
Running the compiled C executable from a normal Windows shell can result
in missing runtime DLL errors, so the UCRT64 environment is the intended
development runtime.

------------------------------------------------------------------------

# 🌐 Frontend Build and Run

Open **CMD**, PowerShell, or another Node.js-capable terminal:

``` cmd
cd C:\Users\LENOVO\Desktop\MorseLab\ProjectC\frontend
```

Install dependencies:

``` cmd
npm install
```

Start the development server:

``` cmd
npm run dev
```

Vite normally serves the application at:

``` text
http://localhost:5173
```

------------------------------------------------------------------------

## Production frontend build

``` cmd
npm run build
```

Preview the production build:

``` cmd
npm run preview
```

------------------------------------------------------------------------

# 🔧 Environment Configuration

The frontend supports an API base URL configuration.

Create a local environment file from:

``` text
frontend/.env.example
```

The production deployment uses the backend URL:

``` text
https://morselab-translator.onrender.com
```

The GitHub Pages deployment workflow receives the production API base
URL through the repository Actions variable:

``` text
VITE_API_BASE_URL
```

This keeps the deployment target configurable without hard-coding the
production API URL into every frontend request.

------------------------------------------------------------------------

# 🚀 Deployment Architecture

MORSELAB uses a split deployment model.

``` mermaid
flowchart LR
    DEV["Developer"]
    GH["GitHub Repository"]

    GH --> GA["GitHub Actions"]
    GA --> PAGES["GitHub Pages<br/>React Production Build"]

    GH --> RENDER["Render"]
    RENDER --> DOCKER["Docker Container"]
    DOCKER --> C["C HTTP Backend"]
    C --> SQLITE["SQLite"]

    USER["User Browser"] --> PAGES
    PAGES -->|HTTPS API| C
```

## Frontend deployment

The GitHub Actions workflow:

1.  Checks out the repository.
2.  Installs frontend dependencies.
3.  Builds the React/Vite application.
4.  Produces the production bundle.
5.  Deploys the bundle to GitHub Pages.

## Backend deployment

Render builds the backend using the project's Docker configuration.

The resulting service exposes the C HTTP API publicly.

------------------------------------------------------------------------

# 🐳 Docker

The repository contains a Dockerfile for the backend deployment.

The container packages the C backend and its runtime dependencies so
Render can build and run the service consistently.

The application is therefore deployable without requiring the production
host to manually reproduce the local MSYS2 development environment.

------------------------------------------------------------------------

# 🧪 Testing and Validation

The project contains dedicated C test programs for the major backend
data structures and subsystems.

  Test                   Scope
  ---------------------- ------------------------------
  `test_auth.c`          Authentication functionality
  `test_database.c`      SQLite/database operations
  `test_hash_table.c`    Hash table
  `test_linked_list.c`   Linked list
  `test_morse.c`         Translation engine
  `test_morse_tree.c`    Morse decoding tree
  `test_queue.c`         Queue
  `test_stack.c`         Stack

## End-to-end validation performed

The deployed/local application has been validated across the major user
workflows:

-   Backend build
-   Backend startup
-   Health endpoint
-   Signup
-   Login
-   Session validation
-   Text → Morse
-   Morse → Text
-   Translation history
-   History deletion
-   Saved translation creation
-   Saved translation listing
-   Saved translation update
-   Saved translation deletion
-   Statistics
-   Profile retrieval
-   Profile update
-   Password functionality
-   Account deletion
-   Session invalidation after account deletion
-   Frontend production build
-   GitHub Pages deployment
-   Render backend deployment
-   Frontend ↔ backend production connection

------------------------------------------------------------------------

# 🛡️ Error Handling

The backend uses defensive C programming practices including:

-   NULL pointer validation
-   Allocation failure checks
-   Input validation
-   Request-size limits
-   Database operation result checks
-   Resource cleanup
-   HTTP status handling
-   Session validation before protected operations
-   Explicit cleanup during shutdown

The HTTP server limits request body processing using a defined maximum
request size.

Dynamic allocations are released through the corresponding cleanup
functions.

------------------------------------------------------------------------

# ⚡ Performance and Complexity

The project intentionally uses DSA where the data model benefits from
it.

  -----------------------------------------------------------------------
  Operation               Structure               Typical Complexity
  ----------------------- ----------------------- -----------------------
  Character → Morse       Hash table              Average `O(1)`
  lookup                                          

  Morse token → character Binary tree             `O(L)` where `L` is
                                                  Morse token length

  Linked-list search      Linked list             `O(n)`

  Linked-list append      Linked list with tail   `O(1)`

  Linked-list             Linked list             `O(n)`
  deletion/search                                 

  Stack push              Stack                   `O(1)`

  Stack pop               Stack                   `O(1)`

  Stack peek              Stack                   `O(1)`

  Queue enqueue           Queue with rear         `O(1)`

  Queue dequeue           Queue with front        `O(1)`

  Queue peek              Queue                   `O(1)`

  SQLite queries          Indexed relational      Depends on query and
                          operations              data volume
  -----------------------------------------------------------------------

For translation, the hash-table approach is especially useful because
each input character can be mapped efficiently, while the binary-tree
approach naturally represents Morse's dot/dash branching structure.

------------------------------------------------------------------------

# 🎨 Design Principles

The project was built around several engineering principles.

## Separation of concerns

Frontend, HTTP routing, authentication, database operations, translation
logic, and DSA implementations are kept in separate modules.

## Reusability

Core operations are exposed through C functions instead of being
duplicated across request handlers.

## Explicit memory management

The backend follows C's explicit allocation/deallocation model and
provides cleanup functions for dynamic structures.

## Layered architecture

``` text
UI
 ↓
API
 ↓
C Server
 ↓
Business Logic
 ↓
DSA / Database
 ↓
Persistence
```

## Academic DSA integration

The data structures are not merely included for demonstration. They
participate in the application's actual translation and data-processing
design.

------------------------------------------------------------------------

# 🔒 Security Considerations

The application includes several security-oriented mechanisms:

### Password protection

Passwords are stored as PBKDF2-HMAC-SHA256 derived values rather than
plaintext.

### Session expiration

Authentication sessions have a defined lifetime.

### Protected API operations

User-specific operations validate authentication before accessing
protected resources.

### User-scoped records

History and saved translations are associated with a `user_id`.

### Database foreign keys

SQLite foreign-key enforcement is enabled.

### Account cleanup

Account deletion also invalidates the user's active sessions.

### Secrets and generated files

Local credentials and generated runtime artifacts are excluded from
version control through `.gitignore`.

------------------------------------------------------------------------

# ⚠️ Deployment Limitation

The current production backend uses **SQLite on Render's Free web
service**.

Render Free web services use an **ephemeral filesystem**. This means:

-   The application code remains safely stored in GitHub.
-   The deployed service can restart or sleep normally.
-   The SQLite file is local to the running service.
-   SQLite data should **not** be treated as durable production storage
    on a free ephemeral service.
-   A restart, redeploy, or service lifecycle event can cause locally
    stored SQLite data to be lost.

This does **not** mean the GitHub project is destroyed.

For a permanent production deployment, the persistence layer should
eventually be moved to durable managed storage, such as a managed
relational database, or the backend should be hosted with persistent
storage.

For academic demonstrations, portfolio presentation, and project
evaluation, the current architecture is useful because it keeps the
DSA-in-C + SQLite design visible and easy to understand.

------------------------------------------------------------------------

# 🔮 Future Improvements

The current implementation is intentionally kept focused on its core
requirements. Potential future improvements include:

-   Durable production database
-   More comprehensive automated integration testing
-   API documentation generated from an explicit schema
-   More granular authorization middleware
-   Rate limiting
-   Stronger JSON parsing/serialization
-   Structured server logging
-   Automated backend CI builds
-   Automated API regression tests
-   Better production observability
-   More extensive international Morse support
-   Additional accessibility testing
-   Expanded deployment health monitoring

These are future directions, not requirements of the current
implementation.

------------------------------------------------------------------------

# 📚 Documentation

The repository contains additional technical documentation:

  File                     Contents
  ------------------------ -------------------------------
  `docs/architecture.md`   System architecture
  `docs/api.md`            API reference
  `docs/database.md`       Database design
  `docs/dsa.md`            DSA implementation details
  `backend/README.md`      Backend-specific information
  `frontend/README.md`     Frontend-specific information

------------------------------------------------------------------------

# 🧩 Why MORSELAB Is Technically Different

Many Morse-code projects stop at:

``` text
Input → Conversion → Output
```

MORSELAB goes further:

``` text
                    ┌───────────────────────┐
                    │       React UI        │
                    └───────────┬───────────┘
                                │
                                ▼
                    ┌───────────────────────┐
                    │   TypeScript / API    │
                    └───────────┬───────────┘
                                │
                         HTTP + JSON
                                │
                                ▼
              ┌─────────────────────────────────┐
              │        Custom C HTTP Server     │
              └───────────────┬─────────────────┘
                              │
             ┌────────────────┼────────────────┐
             │                │                │
             ▼                ▼                ▼
        Authentication    Translation       Database
             │                │                │
             │          ┌─────┴─────┐          │
             │          ▼           ▼          │
             │      Hash Table   Binary Tree   │
             │          │           │          │
             │          └─────┬─────┘          │
             │                │                │
             └────────────────┼────────────────┘
                              ▼
                           SQLite
```

This makes MORSELAB a practical demonstration of how:

**Data Structures + Algorithms + Systems Programming + Databases + Web
Development**

can operate together as one software system.

------------------------------------------------------------------------

# 🧠 Learning Outcomes

Building MORSELAB provides practical experience with:

### C programming

-   Pointers
-   Structures
-   Dynamic memory
-   Header/source separation
-   Modular programming
-   Error handling
-   Resource management

### Data Structures

-   Hash tables
-   Binary trees
-   Linked lists
-   Stacks
-   Queues

### Algorithms

-   Hash-based lookup
-   Tree traversal
-   String processing
-   Search and deletion operations
-   Translation algorithms

### Backend engineering

-   HTTP server development
-   REST-style routing
-   JSON request/response handling
-   Authentication
-   Session management
-   Database integration

### Database engineering

-   SQLite
-   SQL schema design
-   Primary keys
-   Foreign keys
-   Unique constraints
-   Cascading deletes
-   CRUD operations

### Frontend engineering

-   React components
-   TypeScript
-   Client-side routing
-   Protected routes
-   API integration
-   State management
-   Responsive UI development

### DevOps / deployment

-   Git
-   GitHub
-   GitHub Actions
-   GitHub Pages
-   Docker
-   Render
-   Environment configuration

------------------------------------------------------------------------

# 👨‍💻 Author

::: {align="center"}
### Sahil Kumar

**BCA Graduate • MCA Student • Full-Stack & Software Development**

MORSELAB was developed as a practical academic and portfolio project
focused on applying **Data Structures and Algorithms in C** inside a
complete full-stack web application.

`<a href="https://github.com/sahil6000">`{=html}
`<img src="https://img.shields.io/badge/GitHub-sahil6000-181717?style=for-the-badge&logo=github" alt="GitHub Profile">`{=html}
`</a>`{=html}
:::

------------------------------------------------------------------------

# 📄 License

This repository currently does not declare a separate open-source
license.

Unless a license is added to the repository, the source code should be
treated as **all rights reserved by the author**.

------------------------------------------------------------------------

::: {align="center"}
## 🟣 MORSELAB

**Translate • Save • Analyze • Learn**

Built with **C + DSA + SQLite + React + TypeScript**

⭐ If you find the project useful or interesting, consider starring the
repository.
:::
