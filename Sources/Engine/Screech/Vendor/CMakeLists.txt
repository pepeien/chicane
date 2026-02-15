##############  Add subdirectories  #######################
# miniaudio

set(MINIAUDIO_NO_RUNTIME_LINKING OFF CACHE BOOL "")

add_subdirectory(miniaudio)

set_target_properties(miniaudio PROPERTIES POSITION_INDEPENDENT_CODE ON)