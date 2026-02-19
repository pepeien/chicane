#pragma once

// Macros
#if defined(BUILD_SHARED)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        #if defined(BUILD_EXPORT)
            #define CHICANE_RENDERER __declspec(dllexport)
        #else
            #define CHICANE_RENDERER __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define CHICANE_RENDERER __attribute__((visibility("default")))
    #else
        #define CHICANE_RENDERER
    #endif
#else
    #define CHICANE_RENDERER
#endif

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        // Settings
        static constexpr inline const char*         APPLICATION_NAME = "Chicane Engine";
        static constexpr inline const char*         ENGINE_NAME      = "Chicane";

        // Layers
        static constexpr inline const char*         SCENE_LAYER_ID        = "Engine_Scene";
        static constexpr inline const char*         SCENE_SKY_LAYER_ID    = "Engine_Scene_Sky";
        static constexpr inline const char*         SCENE_SHADOW_LAYER_ID = "Engine_Scene_Shadow";
        static constexpr inline const char*         SCENE_MESH_LAYER_ID   = "Engine_Scene_Mesh";

        static constexpr inline const char*         UI_LAYER_ID = "Engine_UI";

        // Resources
        static constexpr inline const std::size_t   TEXTURE_COUNT = 512;

        // Images
        static constexpr inline const std::uint32_t SHADOW_MAP_WIDTH  = 512;
        static constexpr inline const std::uint32_t SHADOW_MAP_HEIGHT = 512;
    }
}