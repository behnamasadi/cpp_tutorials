# ubunutu is the base image

FROM ubuntu:20.04


MAINTAINER Behnam Asadi behnam.asadi@gmail.com


# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
#-y is for accepting yes when the system asked us for installing the package
RUN apt-get install -y build-essential cmake git openssh-server gdb pkg-config valgrind systemd-coredump





# 1) google benchmark
RUN echo "************************ google benchmark ************************"
RUN git clone https://github.com/google/benchmark
RUN mkdir -p  benchmark/build && cd benchmark/build
WORKDIR "benchmark/build"
#RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DGOOGLETEST_PATH=../../googletest   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && cmake --build .  --parallel && cmake --install .
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DBENCHMARK_DOWNLOAD_DEPENDENCIES=TRUE   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && cmake --build .  --parallel && cmake --install .
WORKDIR "/"
RUN rm -rf benchmark



# 2)  tinyxml2
RUN echo "************************ tinyxml2 ************************"
RUN git clone https://github.com/leethomason/tinyxml2
RUN mkdir -p  tinyxml2/build && cd tinyxml2/build
WORKDIR "tinyxml2/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && cmake --build .  --parallel && cmake --install .
WORKDIR "/"
RUN rm -rf tinyxml2



# 3)  yaml-cpp
RUN echo "************************ yaml-cpp ************************"
RUN git clone https://github.com/jbeder/yaml-cpp
RUN mkdir -p  yaml-cpp/build && cd yaml-cpp/build
WORKDIR "yaml-cpp/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && cmake --build .  --parallel && cmake --install .
WORKDIR "/"
RUN rm -rf yaml-cpp
