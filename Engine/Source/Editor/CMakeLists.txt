##############  Set Values  #######################
set(TARGET_NAME "${EDITOR_TARGET_NAME}")
set(SOURCES_DIR "${EDITOR_SOURCE_DIR}")

##############  Set Source Files  #######################
file(
	GLOB_RECURSE
	SOURCES

	${SOURCES_DIR}/*.cpp
)

add_executable(${TARGET_NAME} ${SOURCES})

############## Set Property Balues  #######################
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY         "${BUILD_DIR}")
set_property(TARGET ${TARGET_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY              "${BUILD_BINARY_DIR}")
set_property(TARGET ${TARGET_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG        "${BUILD_BINARY_DIR}")
set_property(TARGET ${TARGET_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE      "${BUILD_BINARY_DIR}")

############## Linking Libs  #######################

if (WIN32)
  	if (USE_MINGW)
    	target_include_directories(
			${TARGET_NAME}
			PUBLIC

    	  	${MINGW_PATH}/include
    	)

    	target_link_directories(
			${TARGET_NAME}
			PUBLIC

    	  	${MINGW_PATH}/lib
    	)
  	endif()
endif()

target_include_directories(
	${TARGET_NAME}
	PUBLIC

	# Source
	${SOURCES_DIR}
)

target_link_libraries(
	${TARGET_NAME}
	PUBLIC

	${RUNTIME_TARGET_NAME}
)