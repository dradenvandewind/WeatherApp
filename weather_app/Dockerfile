# --- Step 1: Build ---
FROM drogonframework/drogon:latest AS builder

WORKDIR /build_context

# Copy source files and configuration
COPY ./CMakeLists.txt ./
COPY ./Makefile ./
COPY src/ ./src/

RUN make configure && make build

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libjsoncpp-dev \
    uuid-dev \
    libssl-dev \
    zlib1g-dev \
    libc-ares2 \
    libpq5 \
    libmariadb3 \
    libsqlite3-0 \
    ca-certificates \
    libhiredis1.1.0 \
    && ln -s /usr/lib/x86_64-linux-gnu/libhiredis.so.1.1.0 \
             /usr/lib/x86_64-linux-gnu/libhiredis.so.0.14 \
    && ldconfig \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build_context/build/drogon_weather .

EXPOSE 8000
CMD ["./drogon_weather"]
