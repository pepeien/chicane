##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${BOX_HEADER_DIR}/*.hpp
)
list(FILTER HEADERS EXCLUDE REGEX "Vendors/.*")

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${BOX_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendors/.*")

##################### Values #######################
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

# fast_obj
set(FAST_OBJ_BUILD_TEST OFF CACHE BOOL "" FORCE)

##################### Setup #######################
add_library(${BOX_TARGET_NAME} ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Box"

    ALIAS
        ${BOX_TARGET_NAME}
)

set_target_properties(
    ${BOX_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Box"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Core

        # Vendors
        fast_obj
        freetype
)

CH_ADD_COMPILER(${BOX_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendors)