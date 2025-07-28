set(PROJECT_DIR ..)
set(ADAPTER ./Adapter/Resiprocate)
set(VENDOR ./Vendor/resiprocate)

include_directories(${PROJECT_DIR})
include_directories(${VENDOR})

aux_source_directory(${ADAPTER} SRC_ADAPTER)

add_subdirectory(${VENDOR})

add_library(sipadapter STATIC ${SRC_ADAPTER})

install(
	TARGETS sipadapter
	ARCHIVE DESTINATION lib
)

install(
	FILES ./Adapter/SipAdapter.h
	DESTINATION "include"
)