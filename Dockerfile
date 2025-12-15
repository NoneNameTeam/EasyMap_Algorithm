# Multi-stage Dockerfile for A* Pathfinding Server
# Stage 1: Build environment
FROM ubuntu:22.04 AS builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libssl-dev \
    ca-certificates \
    ninja-build \
    libboost-all-dev \
    && update-ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Build and install cpprestsdk from source
WORKDIR /opt
RUN GIT_SSL_NO_VERIFY=1 git clone https://github.com/microsoft/cpprestsdk.git --depth=1 && \
    cd cpprestsdk && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DBUILD_TESTS=OFF \
             -DBUILD_SAMPLES=OFF \
             -DCPPREST_EXCLUDE_WEBSOCKETS=ON \
             -DWERROR=OFF \
             -DCMAKE_CXX_FLAGS="-Wno-error=format-truncation" \
             -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && \
    make install && \
    cd /opt && \
    rm -rf cpprestsdk

# Create application directory
WORKDIR /app

# Copy source files
COPY Astar.cpp .

# Create a CMakeLists.txt suitable for Linux Docker environment
RUN echo 'cmake_minimum_required(VERSION 3.20.0)' > CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'project(Astar)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'set(CMAKE_CXX_STANDARD 11)' >> CMakeLists.txt && \
    echo 'set(CMAKE_CXX_STANDARD_REQUIRED ON)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'find_package(OpenSSL REQUIRED)' >> CMakeLists.txt && \
    echo 'find_package(cpprestsdk REQUIRED)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'add_executable(Astar Astar.cpp)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'target_link_libraries(Astar PRIVATE cpprestsdk::cpprest OpenSSL::SSL OpenSSL::Crypto)' >> CMakeLists.txt

# Build the application
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --config Release

# Stage 2: Runtime environment
FROM ubuntu:22.04

# Install runtime dependencies only
RUN apt-get update && apt-get install -y \
    libssl3 \
    ca-certificates \
    curl \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    libboost-filesystem1.74.0 \
    libboost-chrono1.74.0 \
    libboost-atomic1.74.0 \
    libboost-date-time1.74.0 \
    libboost-regex1.74.0 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user for security
RUN useradd -m -u 1000 -s /bin/bash astar && \
    mkdir -p /app && \
    chown -R astar:astar /app

# Set working directory
WORKDIR /app

# Copy built executable and required libraries from builder stage
COPY --from=builder /app/build/Astar /app/Astar
COPY --from=builder /usr/local/lib/libcpprest.so.2.10 /usr/local/lib/libcpprest.so.2.10
RUN ldconfig

# Change to non-root user
USER astar

# Expose the application port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/ || exit 1

# Run the application with tail -f to keep stdin open indefinitely
CMD ["sh", "-c", "tail -f /dev/null | ./Astar"]
