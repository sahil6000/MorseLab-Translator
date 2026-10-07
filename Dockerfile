FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        gcc \
        make \
        libmicrohttpd-dev \
        libsqlite3-dev \
        libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY backend/Makefile backend/Makefile
COPY backend/include/ backend/include/
COPY backend/src/ backend/src/

RUN make -C backend

FROM debian:bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libmicrohttpd12 \
        libsqlite3-0 \
        libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN mkdir -p /app/backend/database
COPY --from=build /app/backend/morselab.exe /app/backend/morselab.exe

ENV PORT=8080
EXPOSE 8080

CMD ["./backend/morselab.exe"]
