##############  Add subdirectories  #######################
# base64
set(BASE64_ENABLE_TESTING OFF CACHE BOOL "")

add_subdirectory(base64)

# earcut.hpp
set(EARCUT_BUILD_TESTS OFF CACHE BOOL "")
set(EARCUT_BUILD_BENCH OFF CACHE BOOL "")
set(EARCUT_BUILD_VIZ   OFF CACHE BOOL "")

add_subdirectory(earcut)

# glm
set(GLM_TESTS OFF CACHE BOOL "")

add_subdirectory(glm)

# pugixml
add_subdirectory(pugixml)