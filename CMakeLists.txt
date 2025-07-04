cmake_minimum_required(VERSION 2.8)

set(SIP_VENDOR_ADAPTER_DIR ./SIP/Adapter/Resiprocate)
set(SIP_VENDOR_DIR ./SIP/Vendor/resiprocate/cmake/output/include)

if (CROSS_PLATFORM STREQUAL X86GCC7.5)
	set(DEVICE_DIR ./Device/Demo)
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else ()
	set(DEVICE_DIR ./Device/IPC)
endif ()

include_directories(./)
include_directories(./tinyxml2)
include_directories(./MANSCDP)
include_directories(./Process)
include_directories(./Agent)
include_directories(./SIP/Adapter)
include_directories(${SIP_VENDOR_ADAPTER_DIR})
include_directories(${SIP_VENDOR_DIR})
include_directories(${DEVICE_DIR})
include_directories(./PS)
include_directories(./RTP)

file(GLOB_RECURSE SRC_XML ./tinyxml2/tinyxml2.cpp)
aux_source_directory(./MANSCDP SRC_MANSCDP)
aux_source_directory(./Process SRC_PROCESS)
aux_source_directory(./Agent SRC_AGENT)
aux_source_directory(./SIP/Adapter SRC_SIP_ADAPTER)
aux_source_directory(${SIP_VENDOR_ADAPTER_DIR} SRC_SIP_VENDOR_ADAPTER)
aux_source_directory(${DEVICE_DIR} SRC_DEV)
file(GLOB SRC_PS "./PS/*.cpp" "./PS/*/*.cpp")
file(GLOB SRC_RTP "./RTP/*.cpp" "./RTP/*/*.cpp")

set(SRC_CUR UA.cpp)
set(SRC_LIB ${SRC_XML} ${SRC_SIP_ADAPTER} ${SRC_SIP_VENDOR_ADAPTER} ${SRC_SIP_VENDOR} ${SRC_PS} ${SRC_RTP} ${SRC_MANSCDP} ${SRC_PROCESS} ${SRC_AGENT} ${SRC_DEV} ${SRC_CUR})

add_compile_options(
		-g
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