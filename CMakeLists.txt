cmake_minimum_required(VERSION 2.8)

include_directories(../tinyxml2)
include_directories(MANSCDP)
include_directories(Process)

file(GLOB_RECURSE SRC_XML ../tinyxml2/tinyxml2.cpp)
aux_source_directory(MANSCDP SRC_MANSCDP)
aux_source_directory(Process SRC_PROCESS)
aux_source_directory(. SRC_CUR)
set(SRC_LIB ${SRC_MANSCDP} ${SRC_XML} ${SRC_PROCESS} ${SRC_CUR})

add_library(GB28181 STATIC ${SRC_LIB})