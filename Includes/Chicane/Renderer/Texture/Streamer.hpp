#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"

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
            struct DecodeResult
            {
                Draw::Id         id    = Draw::InvalidId;
                std::uint32_t    mip   = 0;
                Image::Instance  image = {};
            };

            struct Priority
            {
                Draw::Id id        = Draw::InvalidId;
                float    screenPx  = 0.0f;
                bool     bPinned   = false;
            };

            void pumpDecoded(DrawTextureResource& inResources);
            void requestDecode(DrawTexture& inTexture, std::uint32_t inMip);

        private:
            struct Mailbox
            {
                std::mutex                mutex;
                std::vector<DecodeResult> ready;
            };

            std::shared_ptr<Mailbox>                  m_mailbox = std::make_shared<Mailbox>();
            std::unordered_set<std::uint64_t>         m_inFlight;
        };
    }
}
