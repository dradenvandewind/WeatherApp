# --- Step 1: Build ---
FROM ubuntu:24.04 AS builder

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    make \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source files and configuration
COPY ./CMakeLists.txt ./
COPY ./Makefile ./
COPY src/ ./src/

# Compile using the Makefile
RUN make configure && make build


# --- Step 2: Runtime ---
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/

# Retrieve the compiled binary from the builder
COPY --from=builder /app/build/weather_app .

CMD cp ./weather_app /app_bin/weather_app && echo "C++ binary copied." && sleep infinity





