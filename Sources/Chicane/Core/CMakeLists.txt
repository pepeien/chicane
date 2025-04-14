##############  Set source files  #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${CORE_HEADER_DIR}/*.hpp

    # Vendors
    ${VENDORS_DIR}/base64/base64.cpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)

############## Setup Static #######################
add_library(${CORE_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Core"

    ALIAS
        ${CORE_TARGET_NAME}
)

target_include_directories(
    ${CORE_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

        # Systems
        ${CORE_INCLUDE_DIRS}
)

target_link_libraries(
    ${CORE_TARGET_NAME}

    PUBLIC
        pugixml::pugixml
        SDL3::SDL3
)

##############  Set compiler  #######################
CH_ADD_COMPILER(${CORE_TARGET_NAME})