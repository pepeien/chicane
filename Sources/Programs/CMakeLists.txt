######################### Values #########################
## Box Utils
set(CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME  "${CHICANE_PROGRAMS_TARGET_NAME}-BoxUtils" CACHE INTERNAL "")
set(CHICANE_PROGRAMS_BOX_UTILS_INCLUDES_DIR "${CHICANE_PROGRAMS_SOURCES_DIR}/BoxUtils" CACHE INTERNAL "")
set(CHICANE_PROGRAMS_BOX_UTILS_SOURCES_DIR  "${CHICANE_PROGRAMS_SOURCES_DIR}/BoxUtils" CACHE INTERNAL "")

######################### Directories #########################
add_subdirectory(BoxUtils)