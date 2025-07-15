set(PROJECT_DIR ..)

aux_source_directory(. SRC_PROTOCOL)
aux_source_directory(./Transport SRC_TRANSPORT)
aux_source_directory(./Payload SRC_PAYLOAD)

add_library(rtp STATIC ${SRC_PROTOCOL} ${SRC_TRANSPORT} ${SRC_PAYLOAD})

install(
	TARGETS rtp
	ARCHIVE DESTINATION lib
)