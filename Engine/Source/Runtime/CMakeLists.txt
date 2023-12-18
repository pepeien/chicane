##############  Set values  #######################
set(TARGET_NAME "${RUNTIME_TARGET_NAME}")
set(SOURCES_DIR "${RUNTIME_SOURCE_DIR}")

##############  Set source files  #######################
file(
  GLOB_RECURSE
  SOURCES
  
  ${SOURCES_DIR}/*.cpp
)

add_library(${TARGET_NAME} ${SOURCES})

############## Set Property Balues  #######################
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${BUILD_BINARY_DIR}")

############## Linking Libs  #######################
if (WIN32)
  if (USE_MINGW)
    target_include_directories(
      ${TARGET_NAME}
      PUBLIC

      ${MINGW_PATH}/include
    )

    target_link_directories(
      ${TARGET_NAME}
      PUBLIC

      ${MINGW_PATH}/lib
    )
  endif()
endif()

if (DEFINED VULKAN_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_PATH}/Include")
  set(Vulkan_LIBRARIES "${VULKAN_PATH}/Lib")
  set(Vulkan_FOUND "True")
else()
  find_package(Vulkan REQUIRED)
  
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
  message(FATAL_ERROR "Could not find Vulkan library!")
else()
  message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

if (DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env")

  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")

  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2022")
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")

    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64")
  endif()
else()
  find_package(glfw3 3.3 REQUIRED)

  set(GLFW_LIB glfw)

  message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

target_include_directories(
  ${TARGET_NAME}
  PRIVATE

  # Source
  ${SOURCES_DIR}
)

target_include_directories(
  ${TARGET_NAME}
  PUBLIC

  # Source
  ${SOURCES_DIR}

  # Third Party
  ${GLM_PATH}
  ${STB_PATH}
)

if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")
 
  if (USE_MINGW)
    target_include_directories(
      ${TARGET_NAME}
      PUBLIC

      ${MINGW_PATH}/include
    )

    target_link_directories(
      ${TARGET_NAME}
      PUBLIC

      ${MINGW_PATH}/lib
    )
  endif()
 
  target_include_directories(
    ${TARGET_NAME}
    PUBLIC

    # Third Party
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
  )

  target_link_directories(
    ${TARGET_NAME}
    PUBLIC

    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
  )

  target_link_libraries(
    ${TARGET_NAME}
    PUBLIC

    glfw3
    vulkan-1
  )
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")

    target_link_libraries(
      ${TARGET_NAME}
      PUBLIC

      glfw
      ${Vulkan_LIBRARIES}
    )
endif()