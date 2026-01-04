# KontolOS Build Environment
# Uses a Debian-based image with cross-compilation tools

FROM debian:bookworm-slim

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    gcc \
    binutils \
    make \
    xorriso \
    mtools \
    && rm -rf /var/lib/apt/lists/*

# We'll use gcc with -m32 flag instead of a full cross-compiler
# This works for our simple 32-bit kernel

# Install 32-bit libraries for cross-compilation
RUN dpkg --add-architecture i386 && \
    apt-get update && apt-get install -y \
    gcc-multilib \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /src

# Default command
CMD ["make", "docker-build"]
