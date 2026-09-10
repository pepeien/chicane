##############  Add subdirectories  #######################
# fast_obj
set(FAST_OBJ_BUILD_TEST OFF CACHE BOOL "" FORCE)

add_subdirectory(fast_obj)

# Freetype
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB     ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PCF      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BDF      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_TYPE42   ON CACHE BOOL "" FORCE)
set(FT_DISABLE_WINFONTS ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PFR      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_CID      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_WINFONTS ON CACHE BOOL "" FORCE)

add_subdirectory(freetype)

set_target_properties(
    freetype

    PROPERTIES
        POSITION_INDEPENDENT_CODE
            TRUE
)

# tinygltf

set(TINYGLTF3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TINYGLTF3_INSTALL     OFF CACHE BOOL "" FORCE)

add_subdirectory(tinygltf)

add_library(tinygltf STATIC tinygltf/tiny_gltf_v3.c)
target_include_directories(
    tinygltf

    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf
)
target_compile_definitions(
    tinygltf

    PRIVATE
        TINYGLTF3_ENABLE_FS
)
set_target_properties(
    tinygltf

    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE TRUE
)