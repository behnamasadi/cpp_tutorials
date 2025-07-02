# Use a pre-built image with cmake and build tools to avoid package repository issues
FROM ubuntu/cmake:latest

LABEL maintainer="Behnam Asadi <behnam.asadi@gmail.com>"

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install additional required packages that aren't in the base image
RUN apt-get update && apt-get install -y --no-install-recommends \
    doxygen \
    graphviz \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Verify that cmake and ninja are installed and available
RUN cmake --version && ninja --version

# Set working directory
WORKDIR /cpp_tutorials


