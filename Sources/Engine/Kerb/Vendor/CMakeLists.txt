##############  Add subdirectories  #######################
# Jolt Phyisics

set(PHYSICS_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/JoltPhysics)

include(JoltPhysics/Jolt/Jolt.cmake)

target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:RelWithDebInfo>:JPH_PROFILE_ENABLED;JPH_DEBUG_RENDERER>")