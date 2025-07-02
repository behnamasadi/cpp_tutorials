# Use Ubuntu 24.04 as the base image
FROM ubuntu:24.04

LABEL maintainer="Behnam Asadi <behnam.asadi@gmail.com>"

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin

# Update package lists and install essential packages with better error handling
RUN apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        gnupg \
        lsb-release \
        software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Add Kitware repository for latest cmake
RUN curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/kitware.list

# Install cmake and other build tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        build-essential \
        git \
        doxygen \
        graphviz \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Verify that cmake and ninja are installed and available
RUN cmake --version && ninja --version

# Set working directory
WORKDIR /cpp_tutorials


