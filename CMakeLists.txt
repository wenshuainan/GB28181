cmake_minimum_required(VERSION 2.8)

include_directories(./)
include_directories(./tinyxml2)
include_directories(./MANSCDP)
include_directories(./Process)
include_directories(./Agent)

if (CROSS_PLATFORM STREQUAL X86GCC7.5)
	include_directories(./UserInterface/Demo)
	aux_source_directory(./UserInterface/Demo SRC_USERINTERFACE)
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else ()
	include_directories(./UserInterface/IPC)
	aux_source_directory(./UserInterface/IPC SRC_USERINTERFACE)
endif ()

file(GLOB_RECURSE SRC_XML ./tinyxml2/tinyxml2.cpp)
aux_source_directory(./MANSCDP SRC_MANSCDP)
aux_source_directory(./Process SRC_PROCESS)
aux_source_directory(./Agent SRC_AGENT)
set(SRC_CUR UA.cpp)
set(SRC_LIB ${SRC_XML} ${SRC_MANSCDP} ${SRC_PROCESS} ${SRC_AGENT} ${SRC_USERINTERFACE} ${SRC_CUR})

add_compile_options(
        -pedantic-errors
        -Wall
        -Wclobbered
        -Wdeprecated
        -Wdouble-promotion
        -Wempty-body
        -Wextra
        -Wformat-security
        -Wmaybe-uninitialized
        # -Wmissing-prototypes
        -Wpointer-arith
        -Wmultichar
        -Wno-pedantic # ignored for now, we convert functions to pointers for properties table.
        -Wreturn-type
        -Wshadow
        -Wshift-negative-value
        -Wsizeof-pointer-memaccess
        -Wtype-limits
        -Wundef
        -Wuninitialized
        -Wunreachable-code
        -Wfloat-conversion
        -Wstrict-aliasing
    )

add_library(GB28181 STATIC ${SRC_LIB})