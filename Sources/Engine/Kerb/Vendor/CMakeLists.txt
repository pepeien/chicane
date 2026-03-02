##############  Add subdirectories  #######################
# Jolt Phyisics

set(PHYSICS_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/JoltPhysics)

include(JoltPhysics/Jolt/Jolt.cmake)

set_target_properties(
    Jolt

    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)
