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

# SDL3
set(SDL_AUDIO ON CACHE BOOL "")

set(SDL_VIDEO ON CACHE BOOL "")

set(SDL_GPU     ON  CACHE BOOL "")
set(SDL_OPENGL  ON  CACHE BOOL "")
set(SDL_VULKAN  ON  CACHE BOOL "")
set(SDL_DIRECTX OFF CACHE BOOL "")

set(SDL_RENDER OFF CACHE BOOL "")

set(SDL_CAMERA OFF CACHE BOOL "")

set(SDL_JOYSTICK ON CACHE BOOL "")
set(SDL_HIDAPI   ON CACHE BOOL "")
set(SDL_HAPTIC   ON CACHE BOOL "")
set(SDL_SENSOR   ON CACHE BOOL "")

set(SDL_POWER OFF CACHE BOOL "")

set(SDL_DIALOG OFF CACHE BOOL "")

set(SDL_SHARED ON  CACHE BOOL "")
set(SDL_STATIC OFF CACHE BOOL "")

set(SDL_EXAMPLES OFF CACHE BOOL "")

set(SDL_EXAMPLES OFF CACHE BOOL "")

set(SDL_TEST_LIBRARY OFF CACHE BOOL "")

add_subdirectory(SDL3)