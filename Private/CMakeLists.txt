##############  Set values  #######################
set(CHICANE_TARGET_NAME "${NAME}_Runtime")
set(CHICANE_SOURCE_DIR  "${PRIVATE_DIR}/Chicane")

set(SAMPLE_TARGET_NAME "${NAME}_Sample")
set(SAMPLE_SOURCE_DIR  "${PRIVATE_DIR}/Sample")

##############  Add subdirectories  #######################
add_subdirectory(Chicane)
add_subdirectory(Sample)