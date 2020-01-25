cmake_minimum_required(VERSION 2.8)
PROJECT( mean )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)




#Before that, in lib directory we need to:

#ln -sf libmean.so.1.0.1 libmean.so
#ln -sf libmean.so.1.0.1 libmean.so.1

#The link to ./libmean.so allows the naming convention for the compile flag -lmean to work.
#The link to ./libmean.so.1 allows the run time binding to work.


#Binaries themselves know which version of a shared library they depend on, and request it specifically. You can use ldd to show the dependencies;
ADD_EXECUTABLE(main src/main.c)
TARGET_LINK_LIBRARIES(main mean )
