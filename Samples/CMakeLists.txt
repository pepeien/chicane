##################### Values #######################
set(NAME "Samples")

set(SHOOTER_TARGET_NAME         "${NAME}-Shooter")
set(SHOOTER_HEADER_DIR          "${CMAKE_CURRENT_LIST_DIR}/Shooter")
set(SHOOTER_SOURCE_DIR          "${CMAKE_CURRENT_LIST_DIR}/Shooter")
set(SHOOTER_BUILD_INTERFACE_DIR "${CMAKE_CURRENT_LIST_DIR}/Shooter/Sources")

set(SHOOTER_CONTENT_TARGET_NAME "${SHOOTER_TARGET_NAME}-Content")
set(SHOOTER_CONTENT_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/Shooter/Contents")

##################### Directories #######################
add_subdirectory(Shooter)