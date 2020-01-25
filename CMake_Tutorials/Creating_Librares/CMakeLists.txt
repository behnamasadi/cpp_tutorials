cmake_minimum_required(VERSION 2.8)
PROJECT( mean )

#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

#ADD_LIBRARY(<name> [STATIC | SHARED | MODULE] source1 source2 ... sourceN)
ADD_LIBRARY(mean  SHARED src/calc_mean.c)

#SOVERSION is a version number on the .so file 
#you create and is usually understood as an indicator for API compatibility (and ABI too).
#So whenever you change the library API in a way that apps that linked
#against an old version won't work against the new version increase the SOVERSION. Its just an always increasing integer.

SET_TARGET_PROPERTIES(mean PROPERTIES VERSION 1.0.0 SOVERSION 1)

#get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#message("inc_dirs = ${inc_dirs}")

#get_property(link_dirs DIRECTORY PROPERTY LINK_DIRECTORIES)
#message("link_dirs = ${link_dirs}")

