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
    && rm -rf /var/lib/apt/lists/*

# Install vcpkg
WORKDIR /opt
RUN git clone https://github.com/Microsoft/vcpkg.git && \
    cd vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install

# Set vcpkg environment variables
ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"

# Install cpprestsdk via vcpkg
RUN /opt/vcpkg/vcpkg install cpprestsdk:x64-linux

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
    echo 'set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake")' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'find_package(cpprestsdk REQUIRED)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'add_executable(Astar Astar.cpp)' >> CMakeLists.txt && \
    echo '' >> CMakeLists.txt && \
    echo 'target_link_libraries(Astar PRIVATE cpprestsdk::cpprest)' >> CMakeLists.txt

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
    && rm -rf /var/lib/apt/lists/*

# Create non-root user for security
RUN useradd -m -u 1000 -s /bin/bash astar && \
    mkdir -p /app && \
    chown -R astar:astar /app

# Set working directory
WORKDIR /app

# Copy built executable from builder stage
COPY --from=builder /app/build/Astar /app/Astar

# Change to non-root user
USER astar

# Expose the application port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/ || exit 1

# Run the application
CMD ["./Astar"]
