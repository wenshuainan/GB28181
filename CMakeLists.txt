cmake_minimum_required(VERSION 2.8)

if (CROSS_PLATFORM STREQUAL X86GCC7.5)
	set(DEVICE_DIR ./Device/Demo)
	
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -D_REENTRANT -Wall -ffunction-sections -fdata-sections -fsanitize=address -fno-omit-frame-pointer")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -D_REENTRANT -Wall -ffunction-sections -fdata-sections -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else ()
	set(DEVICE_DIR ./Device/IPC)
endif ()

include_directories(./)
include_directories(${DEVICE_DIR})

aux_source_directory(. SRC_UA)
aux_source_directory(./Agent SRC_AGENT)
aux_source_directory(./MANSCDP SRC_MANSCDP)
aux_source_directory(./MANSRTSP SRC_MANSRTSP)
aux_source_directory(./Interface SRC_INTERFACE)
file(GLOB_RECURSE SRC_DEV "${DEVICE_DIR}/*.cpp")

set(SRC_GB28181 ${SRC_MANSCDP} ${SRC_MANSRTSP} ${SRC_INTERFACE} ${SRC_AGENT} ${SRC_DEV} ${SRC_UA})

add_compile_options(
	#-Wclobbered
	#-Wdeprecated
	#-Wdouble-promotion
	#-Wempty-body
	#-Wextra
	#-Wformat-security
	#-Wmaybe-uninitialized
	#-Wmissing-prototypes
	#-Wpointer-arith
	#-Wmultichar
	#-Wpedantic
	#-Wreturn-type
	#-Wshadow
	#-Wshift-negative-value
	#-Wsizeof-pointer-memaccess
	#-Wtype-limits
	#-Wundef
	#-Wuninitialized
	#-Wunreachable-code
	#-Wfloat-conversion
	#-Wstrict-aliasing
)

add_library(GB28181 STATIC ${SRC_GB28181})

add_subdirectory(./tinyxml2)
add_subdirectory(./PS)
add_subdirectory(./RTP)
add_subdirectory(./SIP)

install(
	TARGETS GB28181
	ARCHIVE DESTINATION lib
)

#install(
#	DIRECTORY MANSCDP
#	DESTINATION "include"
#	FILES_MATCHING PATTERN "*.h"
#)

#install(
#	DIRECTORY MANSRTSP
#	DESTINATION "include"
#	FILES_MATCHING PATTERN "*.h"
#)

install(
	FILES Device/Demo/Device.h Device/Demo/MANSCDPDevice.h
	DESTINATION "include"
)

install(
	DIRECTORY Interface
	DESTINATION "include"
	FILES_MATCHING PATTERN "*.h"
)

install(
	FILES UA.h
	DESTINATION "include"
)