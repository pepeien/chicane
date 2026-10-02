#pragma once

#include "Chicane/Core/Mailbox.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Texture/Streamer/DecodeResult.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER TextureStreamerMailbox
        {
        public:
            Mailbox<TextureStreamerDecodeResult> ready;
        };
    }
}
