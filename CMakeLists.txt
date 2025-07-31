set(BUILD_TESTING FALSE)

if (CROSS_PLATFORM STREQUAL X86GCC7.5)
	set(DEVICE_DIR ./Device/Demo)
	
	string(REPLACE "-O2 -Os" "-g" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
	string(REPLACE "-O2 -Os" "-g" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else ()
	set(DEVICE_DIR ./Device/IPC)
endif ()

include_directories(../Include)
include_directories(./)
include_directories(./tinyxml2)
include_directories(./SIP/Adapter)
include_directories(${DEVICE_DIR})
include_directories(./PS)
include_directories(./RTP)

aux_source_directory(. SRC_UA)
aux_source_directory(./Agent SRC_AGENT)
aux_source_directory(./MANSCDP SRC_MANSCDP)
aux_source_directory(./MANSRTSP SRC_MANSRTSP)
aux_source_directory(./Interface SRC_INTERFACE)
aux_source_directory(${DEVICE_DIR} SRC_DEV)

set(dir_source ${SRC_MANSCDP} ${SRC_MANSRTSP} ${SRC_INTERFACE} ${SRC_AGENT} ${SRC_DEV} ${SRC_UA})

if (CONF_LIBRARY STREQUAL static)
	add_library(GB28181 STATIC ${dir_source})
elseif (CONF_LIBRARY STREQUAL share)
	add_library(GB28181 SHARED ${dir_source})
elseif (CONF_LIBRARY STREQUAL static_share)
	add_library(GB28181 SHARED ${dir_source})
	add_library(GB28181_static STATIC ${dir_source})

	set_target_properties(GB28181 PROPERTIES OUTPUT_NAME "GB28181")
	set_target_properties(GB28181_static PROPERTIES OUTPUT_NAME "GB28181")
	set_target_properties(GB28181 PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set_target_properties(GB28181_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set_target_properties(GB28181 PROPERTIES VERSION 1.2 SOVERSION 1)

	get_target_property(OUTPUT_VALUE GB28181_static OUTPUT_NAME)
	message(STATUS "This is Library(static and share) Mark =====" ${OUTPUT_VALUE})
elseif (NOT CONF_LIBRARY)
	message(WARNING "x xx xxx xxxx xxxxx xxxxxx xxxxxxx xxxxxxxx xxxxxxxxx xxxxxxxxxxx")
endif (CONF_LIBRARY STREQUAL static)

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
#	DESTINATION include
#	FILES_MATCHING PATTERN "*.h"
#)

#install(
#	DIRECTORY MANSRTSP
#	DESTINATION include
#	FILES_MATCHING PATTERN "*.h"
#)

install(
	DIRECTORY Interface
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)

install(
	FILES UA.h
	DESTINATION include
)
