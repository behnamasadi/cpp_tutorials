# ubunutu is the base image

FROM ubuntu:20.04


MAINTAINER Behnam Asadi behnam.asadi@gmail.com


# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
#-y is for accepting yes when the system asked us for installing the package
RUN apt-get install -y build-essential cmake git openssh-server gdb pkg-config valgrind


# 1) gflags
RUN echo "************************ gflags ************************"
RUN git clone https://github.com/gflags/gflags
RUN mkdir -p gflags/build &&  cd gflags/build
WORKDIR "gflags/build"
RUN pwd
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++11 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON   ../ && make -j8 all install 
RUN cd ../../
RUN rm -rf gflags


# 2) glog
RUN echo "************************ glog ************************"
RUN git clone https://github.com/google/glog
WORKDIR "glog/build"
RUN mkdir -p  glog/build && cd glog/build
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++11 -DCMAKE_BUILD_TYPE=Release  -DBUILD_SHARED_LIBS=ON   ../ && make -j8 all install 
WORKDIR "/"
RUN rm -rf glog


RUN echo "************************ googletest ************************"
# 3) googletest
RUN git clone https://github.com/google/googletest
RUN mkdir -p  googletest/build && cd googletest/build
WORKDIR "googletest/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && make -j8 all install 
WORKDIR "/"
# we need googletest source code for google benchmark, so we shouldn't delete it! 
# RUN rm -rf googletest


# 4) google benchmark
RUN echo "************************ google benchmark ************************"
RUN git clone https://github.com/google/benchmark
RUN mkdir -p  benchmark/build && cd benchmark/build
WORKDIR "benchmark/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DGOOGLETEST_PATH=../../googletest   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && make -j8 all install 
WORKDIR "/"
RUN rm -rf benchmark



# 5)  tinyxml2
RUN echo "************************ tinyxml2 ************************"
RUN git clone https://github.com/leethomason/tinyxml2
RUN mkdir -p  tinyxml2/build && cd tinyxml2/build
WORKDIR "tinyxml2/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && make -j8 all install 
WORKDIR "/"
RUN rm -rf tinyxml2



# 6)  yaml-cpp
RUN echo "************************ yaml-cpp ************************"
RUN git clone https://github.com/jbeder/yaml-cpp
RUN mkdir -p  yaml-cpp/build && cd yaml-cpp/build
WORKDIR "yaml-cpp/build"
RUN cmake -DCMAKE_CXX_FLAGS=-std=c++1z   -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../ && make -j8 all install 
WORKDIR "/"
RUN rm -rf yaml-cpp
