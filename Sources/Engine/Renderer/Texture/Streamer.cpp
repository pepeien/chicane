#include "Chicane/Renderer/Texture/Streamer.hpp"

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Worker.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static std::uint64_t decodeKey(Draw::Id inId, std::uint32_t inMip)
        {
            return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(inId)) << 32) | inMip;
        }

        static float columnLength(const Mat4& inModel, int inColumn)
        {
            const glm::vec3 axis = glm::vec3(inModel[inColumn]);

            return glm::length(axis);
        }

        static std::uint32_t mipFromScreen(const DrawTexture& inTexture, float inScreenPx)
        {
            if (inTexture.getMipCount() == 0)
            {
                return 0;
            }

            const float mip = std::log2(
                std::max(static_cast<float>(std::max(1u, inTexture.width)) / std::max(inScreenPx, 1.0f), 1.0f)
            );

            return std::min(
                inTexture.getMipCount() - 1,
                static_cast<std::uint32_t>(std::max(0.0f, std::floor(mip - 0.5f)))
            );
        }

        void TextureStreamer::tick(
            const Frame&         inFrame,
            DrawTextureResource& inResources,
            std::size_t          inBudgetBytes,
            std::uint32_t        inScreenHeight
        )
        {
            pumpDecoded(inResources);

            std::unordered_map<Draw::Id, Priority> priorities;
            auto consider = [&](Draw::Id inId, float inScreenPx, bool inPinned)
            {
                if (inId <= Draw::InvalidId)
                {
                    return;
                }

                Priority& entry = priorities[inId];
                entry.id        = inId;
                entry.bPinned   = entry.bPinned || inPinned;
                entry.screenPx  = std::max(entry.screenPx, inScreenPx);
            };

            for (const DrawPoly2DInstance& instance : inFrame.getInstances2D())
            {
                consider(instance.texture, static_cast<float>(std::max(1u, inScreenHeight)), true);
            }

            const Vec4& camera = inFrame.getCamera().translation;
            for (const DrawPoly3DInstance& instance : inFrame.getInstances3D())
            {
                const float scale = std::max(
                    {columnLength(instance.model, 0), columnLength(instance.model, 1), columnLength(instance.model, 2)}
                );
                const float radius = std::max(scale * 0.5f, 0.01f);
                const Vec3  origin = instance.model.getTranslation();
                const float dx     = origin.x - camera.x;
                const float dy     = origin.y - camera.y;
                const float dz     = origin.z - camera.z;
                const float dist          = std::max(std::sqrt(dx * dx + dy * dy + dz * dz), 0.01f);
                const float distToSurface = std::max(dist - radius, 0.01f);
                const float screen =
                    (radius / distToSurface) * static_cast<float>(std::max(1u, inScreenHeight));

                for (std::uint8_t map = 0; map < TEXTURE_MAP_COUNT; map++)
                {
                    consider(instance.textures[map], screen, false);
                }
            }

            std::size_t used = 0;
            for (const DrawTexture& draw : inResources.getDraws())
            {
                used += draw.getResidentBytes();
                if (draw.id <= Draw::InvalidId || !draw.bStreamable)
                {
                    continue;
                }

                if (priorities.find(draw.id) == priorities.end())
                {
                    consider(draw.id, 0.0f, false);
                }
            }

            bool bDirty = false;

            auto applyResident = [&](DrawTexture& inTexture, std::uint32_t inMip)
            {
                if (inTexture.getMipCount() == 0)
                {
                    return false;
                }

                const std::uint32_t mip = std::min(inMip, inTexture.getMipCount() - 1);
                if (mip == inTexture.residentMinMip)
                {
                    return false;
                }

                if (inTexture.mips && !inTexture.mips->levels[mip].decoded)
                {
                    if (!inTexture.mips->levels[mip].encoded.empty())
                    {
                        requestDecode(inTexture, mip);

                        return false;
                    }

                    inTexture.mips->decode(mip);
                }

                inTexture.residentMinMip = mip;
                inTexture.desiredMinMip  = mip;
                bDirty                   = true;

                return true;
            };

            std::vector<Priority> ranked;
            ranked.reserve(priorities.size());
            for (const auto& [id, priority] : priorities)
            {
                ranked.push_back(priority);
            }

            std::sort(
                ranked.begin(),
                ranked.end(),
                [](const Priority& inLeft, const Priority& inRight)
                {
                    if (inLeft.bPinned != inRight.bPinned)
                    {
                        return inLeft.bPinned;
                    }

                    return inLeft.screenPx > inRight.screenPx;
                }
            );

            std::uint32_t promotions = 0;
            for (const Priority& priority : ranked)
            {
                DrawTexture* texture = inResources.getDrawMutable(priority.id);
                if (!texture || !texture->bStreamable || texture->getMipCount() == 0)
                {
                    continue;
                }

                const std::uint32_t want = priority.bPinned ? 0 : mipFromScreen(*texture, priority.screenPx);
                texture->desiredMinMip   = want;
                if (want >= texture->residentMinMip)
                {
                    continue;
                }

                if (texture->mips && want < texture->mips->levels.size() &&
                    !texture->mips->levels[want].decoded && !texture->mips->levels[want].encoded.empty())
                {
                    requestDecode(*texture, want);
                }

                if (promotions >= MAX_PROMOTIONS_PER_FRAME)
                {
                    continue;
                }

                std::uint32_t target = texture->residentMinMip - 1;
                if (texture->mips && want < target && want < texture->mips->levels.size() &&
                    texture->mips->levels[want].decoded)
                {
                    target = want;
                }

                const std::size_t nextBytes = Image::mipChainBytes(
                    Image::mipDimension(texture->width, target),
                    Image::mipDimension(texture->height, target)
                );
                const std::size_t currentBytes = texture->getResidentBytes();
                if (inBudgetBytes > 0 && promotions > 0 && used - currentBytes + nextBytes > inBudgetBytes)
                {
                    continue;
                }

                if (applyResident(*texture, target))
                {
                    used = used - currentBytes + nextBytes;
                    promotions++;
                }
            }

            if (inBudgetBytes > 0 && used > inBudgetBytes)
            {
                std::vector<Priority> eviction = ranked;
                std::sort(
                    eviction.begin(),
                    eviction.end(),
                    [](const Priority& inLeft, const Priority& inRight)
                    {
                        if (inLeft.bPinned != inRight.bPinned)
                        {
                            return !inLeft.bPinned && inRight.bPinned;
                        }

                        return inLeft.screenPx < inRight.screenPx;
                    }
                );

                for (const Priority& priority : eviction)
                {
                    if (used <= inBudgetBytes)
                    {
                        break;
                    }

                    DrawTexture* texture = inResources.getDrawMutable(priority.id);
                    if (!texture || !texture->bStreamable || priority.bPinned)
                    {
                        continue;
                    }

                    const std::uint32_t tail = texture->getStreamTailMinMip();
                    if (texture->residentMinMip >= tail)
                    {
                        continue;
                    }

                    const std::size_t currentBytes = texture->getResidentBytes();
                    if (applyResident(*texture, texture->residentMinMip + 1))
                    {
                        used = used - currentBytes + texture->getResidentBytes();
                    }
                }
            }

            if (bDirty)
            {
                inResources.markAsDirty();
            }
        }

        void TextureStreamer::pumpDecoded(DrawTextureResource& inResources)
        {
            std::vector<DecodeResult> ready;
            {
                std::lock_guard<std::mutex> lock(m_mailbox->mutex);
                ready.swap(m_mailbox->ready);
            }

            for (const DecodeResult& result : ready)
            {
                m_inFlight.erase(decodeKey(result.id, result.mip));
                DrawTexture* texture = inResources.getDrawMutable(result.id);
                if (!texture || !texture->mips || !result.image)
                {
                    continue;
                }

                if (result.mip < texture->mips->levels.size())
                {
                    texture->mips->levels[result.mip].decoded = result.image;
                }
            }
        }

        void TextureStreamer::requestDecode(DrawTexture& inTexture, std::uint32_t inMip)
        {
            if (!inTexture.mips || inMip >= inTexture.mips->getCount())
            {
                return;
            }

            const std::uint64_t key = decodeKey(inTexture.id, inMip);
            if (m_inFlight.find(key) != m_inFlight.end())
            {
                return;
            }

            if (inTexture.mips->levels[inMip].encoded.empty())
            {
                return;
            }

            const Draw::Id                         id      = inTexture.id;
            const std::shared_ptr<Image::MipChain> mips    = inTexture.mips;
            const std::shared_ptr<Mailbox>         mailbox = m_mailbox;
            m_inFlight.insert(key);

            Worker::submit(
                [mailbox, mips, id, inMip]()
                {
                    if (!mailbox || !mips || inMip >= mips->levels.size())
                    {
                        return;
                    }

                    const Image::Raw& encoded = mips->levels[inMip].encoded;
                    if (encoded.empty())
                    {
                        return;
                    }

                    Image::Instance image = std::make_shared<Image>(encoded, ImageVendor::Png);
                    std::lock_guard<std::mutex> lock(mailbox->mutex);
                    mailbox->ready.push_back({id, inMip, image});
                }
            );
        }
    }
}
