set(PROJECT_DIR ..)

aux_source_directory(. SRC_PS)
aux_source_directory(./Packetizer SRC_PACKETIZER)

add_library(mpeg2 STATIC ${SRC_PS} ${SRC_PACKETIZER})

install(
	TARGETS mpeg2
	ARCHIVE DESTINATION lib
)