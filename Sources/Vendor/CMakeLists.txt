##############  Add subdirectories  #######################
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

set(SDL_TEST_LIBRARY OFF CACHE BOOL "")

set(SDL_INSTALL OFF CACHE BOOL "")

add_subdirectory(SDL3 EXCLUDE_FROM_ALL)