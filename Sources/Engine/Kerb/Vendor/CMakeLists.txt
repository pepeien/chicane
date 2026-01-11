##############  Add subdirectories  #######################
# Jolt Phyisics

set(PHYSICS_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/JoltPhysics)

include(JoltPhysics/Jolt/Jolt.cmake)

target_compile_definitions(
    Jolt
        PUBLIC
            "$<$<CONFIG:Debug>:JPH_PROFILE_ENABLED>"
            "$<$<CONFIG:RelWithDebInfo>:JPH_PROFILE_ENABLED>"
            "$<$<CONFIG:Release>:JPH_DEBUG_RENDERER>"
)

set_target_properties(
    Jolt

    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)
