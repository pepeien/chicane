##############  Add subdirectories  #######################
# base64
set(BASE64_ENABLE_TESTING OFF CACHE BOOL "")

add_subdirectory(base64)

# glm
set(GLM_TESTS OFF CACHE BOOL "")

add_subdirectory(glm)

# libtess2
add_subdirectory(libtess2)

# SDL
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

set(SDL_EXAMPLES             OFF CACHE BOOL "")
set(SDL_EXAMPLES_LINK_SHARED OFF CACHE BOOL "")

set(SDL_TEST_LIBRARY OFF CACHE BOOL "")

set(SDL_INSTALL OFF CACHE BOOL "")

add_subdirectory(SDL EXCLUDE_FROM_ALL)

# pugixml
add_subdirectory(pugixml)

# lua
add_subdirectory(lua)

# tracy
if (CHICANE_TRACER)
    if (CHICANE_SHARED)
        set(TRACY_STATIC OFF CACHE BOOL "")
    else()
        set(TRACY_STATIC ON CACHE BOOL "")
    endif()

    set(TRACY_ENABLE         ON CACHE BOOL "")
    set(TRACY_ON_DEMAND      ON CACHE BOOL "")
    set(TRACY_ONLY_LOCALHOST ON CACHE BOOL "")

    add_subdirectory(tracy EXCLUDE_FROM_ALL)
endif()