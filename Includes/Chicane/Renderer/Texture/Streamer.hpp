#pragma once

#include <cstdint>
#include <memory>
#include <unordered_set>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Texture/Streamer/Mailbox.hpp"
#include "Chicane/Renderer/Texture/Streamer/Priority.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER TextureStreamer
        {
        public:
            static constexpr inline const std::uint32_t MAX_PROMOTIONS_PER_FRAME = 4;

        public:
            void tick(
                const Frame&         inFrame,
                DrawTextureResource& inResources,
                std::size_t          inBudgetBytes,
                std::uint32_t        inScreenHeight
            );

        private:
            void pumpDecoded(DrawTextureResource& inResources);
            void requestDecode(DrawTexture& inTexture, std::uint32_t inMip);

        private:
            std::shared_ptr<TextureStreamerMailbox> m_mailbox = std::make_shared<TextureStreamerMailbox>();
            std::unordered_set<std::uint64_t>       m_inFlight;
        };
    }
}
