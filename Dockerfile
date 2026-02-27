# =============================================================================
# Droy Language - Dockerfile
# =============================================================================
# Multi-stage build for Droy Language compiler and tools
#
# Build:
#   docker build -t droy-lang .
#
# Run:
#   docker run -it droy-lang
#   docker run -it -v $(pwd):/workspace droy-lang
#
# =============================================================================

# =============================================================================
# Stage 1: Builder
# =============================================================================
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    llvm-14 \
    llvm-14-dev \
    clang-14 \
    libllvm14 \
    golang-go \
    nodejs \
    npm \
    git \
    wget \
    curl \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set LLVM environment variables
ENV LLVM_CONFIG=/usr/bin/llvm-config-14
ENV CC=clang-14
ENV CXX=clang++-14

# Set working directory
WORKDIR /build

# Copy source code
COPY . .

# Build Droy Language
RUN make clean && make all -j$(nproc)

# Run tests
RUN make test

# =============================================================================
# Stage 2: Runtime
# =============================================================================
FROM ubuntu:22.04 AS runtime

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libllvm14 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash droy

# Copy binaries from builder
COPY --from=builder /build/bin/* /usr/local/bin/
COPY --from=builder /build/lib /usr/local/lib/droy
COPY --from=builder /build/include /usr/local/include/droy
COPY --from=builder /build/examples /usr/local/share/droy/examples

# Set permissions
RUN chmod +x /usr/local/bin/droy* && \
    chown -R droy:droy /usr/local/lib/droy /usr/local/share/droy

# Switch to non-root user
USER droy
WORKDIR /home/droy

# Set environment variables
ENV DROY_HOME=/usr/local/lib/droy
ENV PATH="/usr/local/bin:${PATH}"

# Default command
CMD ["droy", "-h"]

# =============================================================================
# Stage 3: Development
# =============================================================================
FROM builder AS dev

# Install additional development tools
RUN apt-get update && apt-get install -y \
    gdb \
    valgrind \
    cppcheck \
    clang-format \
    clang-tidy \
    vim \
    nano \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /droy-lang

# Copy source code
COPY . .

# Build with debug symbols
RUN make clean && make DEBUG=1 -j$(nproc)

# Default command
CMD ["/bin/bash"]

# =============================================================================
# Stage 4: Web IDE
# =============================================================================
FROM node:18-alpine AS editor

# Install serve for static files
RUN npm install -g serve

# Set working directory
WORKDIR /app

# Copy editor source
COPY editor/ .

# Install dependencies and build
RUN npm install && npm run build

# Expose port
EXPOSE 3000

# Serve the built application
CMD ["serve", "-s", "dist", "-l", "3000"]

# =============================================================================
# Stage 5: Minimal
# =============================================================================
FROM alpine:latest AS minimal

# Install minimal dependencies
RUN apk add --no-cache \
    libstdc++ \
    musl-dev

# Copy only the essential binary
COPY --from=builder /build/bin/droy /usr/local/bin/

# Set user
USER nobody

# Default command
CMD ["droy", "-h"]
