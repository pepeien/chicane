#pragma once

#include <mutex>
#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Texture/Streamer/DecodeResult.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER TextureStreamerMailbox
        {
        public:
            using Textures = std::vector<TextureStreamerDecodeResult>;

        public:
            std::mutex mutex;
            Textures   ready = {};
        };
    }
}
