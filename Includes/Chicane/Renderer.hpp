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

#include <cstddef>
#include <cstdint>

#include "Chicane/Core/Color.hpp"

namespace Chicane
{
    namespace Renderer
    {
        // Settings
        static constexpr inline const char*         APPLICATION_NAME = "Chicane Engine";
        static constexpr inline const char*         ENGINE_NAME      = "Chicane";

        // Layers
        static constexpr inline const char*         SCENE_LAYER_ID            = "Engine_Scene";
        static constexpr inline const char*         SCENE_SKY_LAYER_ID        = "Engine_Scene_Sky";
        static constexpr inline const char*         SCENE_SHADOW_LAYER_ID     = "Engine_Scene_Shadow";
        static constexpr inline const char*         SCENE_MESH_LAYER_ID       = "Engine_Scene_Mesh";
        static constexpr inline const char*         SCENE_LINE_LAYER_ID       = "Engine_Scene_Line";
        static constexpr inline const char*         SCENE_FOREGROUND_LAYER_ID = "Engine_Scene_Foreground";

        static constexpr inline const char*         UI_LAYER_ID = "Engine_UI";

        // Targets
        static constexpr inline const char*         SCREEN_TARGET_ID = "Screen";

        // Resources
        // Per-texture native POT size is clamped to TEXTURE_MAX_SIZE. GPU images are
        // allocated at the currently resident mip, not a uniform 4K footprint — a 4K
        // RGBA8 chain is ~85 MB, so the streamer must drop mips under VRAM pressure.
        static constexpr inline const std::uint32_t TEXTURE_MAX_SIZE      = 4096;
        static constexpr inline const std::uint32_t TEXTURE_STREAM_TAIL   = 128;
        static constexpr inline const std::uint32_t TEXTURE_SLOT_MAX         = 4096;
        static constexpr inline const std::uint32_t TEXTURE_TABLE_BINDING    = 5;
        static constexpr inline const std::uint32_t TEXTURE_CLASS_COUNT      = 6;
        static constexpr inline const std::uint32_t TEXTURE_CLASS_BINDING    = 6;
        static constexpr inline const std::uint32_t TEXTURE_CLASS_SIZES[6]   = {128, 256, 512, 1024, 2048, 4096};

        // Images
        static constexpr inline const std::uint32_t SHADOW_MAP_WIDTH     = 2048;
        static constexpr inline const std::uint32_t SHADOW_MAP_HEIGHT    = 2048;
        static constexpr inline const std::uint32_t SHADOW_CASCADE_COUNT = 4;
        static constexpr inline const std::uint32_t MAX_LIGHTS           = 8;

        static constexpr inline const std::uint32_t SKY_TEXTURE_SIZE = 512;
        static constexpr inline const std::uint32_t SKY_MIP_LEVELS   = 10;

        static inline const Color::Rgba             OUTLINE_COLOR = Color::toRgba("#0b997d");
    }
}