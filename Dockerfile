# Use Ubuntu 24.04 as the base image
FROM ubuntu:24.04

LABEL maintainer="Behnam Asadi <behnam.asadi@gmail.com>"

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install required packages with better error handling
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    ninja-build \
    doxygen \
    build-essential \
    graphviz \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


