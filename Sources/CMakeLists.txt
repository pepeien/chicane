##############  Set values  #######################
set(RUNTIME_TARGET_NAME "${NAME}_Runtime")
set(RUNTIME_SOURCE_DIR  "${SOURCES_DIR}/Runtime")

set(SAMPLE_TARGET_NAME "${NAME}_Sample")
set(SAMPLE_SOURCE_DIR  "${SOURCES_DIR}/Sample")

##############  Add subdirectories  #######################
add_subdirectory(Runtime)
add_subdirectory(Sample)