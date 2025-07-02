# Use Ubuntu 24.04 as the base image
FROM ubuntu:24.04

LABEL maintainer="Behnam Asadi <behnam.asadi@gmail.com>"

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install required packages with retry logic and better error handling
RUN --mount=type=cache,target=/var/cache/apt \
    for i in $(seq 1 3); do \
        apt-get update --fix-missing && \
        apt-get install -y --no-install-recommends \
            git \
            cmake \
            ninja-build \
            doxygen \
            build-essential \
            graphviz \
            ca-certificates \
        && break || sleep 10; \
    done && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*


