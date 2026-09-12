#include "Chicane/Renderer/Draw/Texture/Resource.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Renderer
    {
        static void populateDrawTexture(DrawTexture& outDraw, const DrawTextureData& inData)
        {
            outDraw.reference   = inData.reference;
            outDraw.bIsVolatile = inData.bIsVolatile;
            outDraw.image       = inData.image;
            outDraw.mips        = inData.mips;
            outDraw.bStreamable = inData.bStreamable;

            if (!outDraw.mips && inData.image)
            {
                outDraw.mips =
                    std::make_shared<Image::MipChain>(Image::makeMipChain(*inData.image, TEXTURE_MAX_SIZE, false));
            }

            if (outDraw.mips && !outDraw.mips->isEmpty())
            {
                outDraw.width  = outDraw.mips->getWidth();
                outDraw.height = outDraw.mips->getHeight();
            }
            else if (inData.image)
            {
                outDraw.width  = static_cast<std::uint32_t>(std::max(0, inData.image->getWidth()));
                outDraw.height = static_cast<std::uint32_t>(std::max(0, inData.image->getHeight()));
            }

            outDraw.residentMinMip = outDraw.getStreamTailMinMip();
            outDraw.desiredMinMip  = outDraw.bStreamable ? outDraw.residentMinMip : 0;

            if (outDraw.mips)
            {
                outDraw.mips->ensureDecoded(outDraw.residentMinMip);
                if (Image::Instance image = outDraw.mips->decode(outDraw.residentMinMip))
                {
                    outDraw.image = image;
                }
            }
        }

        bool DrawTextureResource::isEmpty() const
        {
            return m_draws.empty();
        }

        const DrawTexture::List& DrawTextureResource::getDraws() const
        {
            return m_draws;
        }

        Draw::Id DrawTextureResource::findId(const Draw::Reference& inReference)
        {
            if (inReference.isEmpty())
            {
                return Draw::InvalidId;
            }

            for (const DrawTexture& draw : m_draws)
            {
                if (!draw.reference.equals(inReference))
                {
                    continue;
                }

                return draw.id;
            }

            return Draw::InvalidId;
        }

        const DrawTexture& DrawTextureResource::getDraw(const Draw::Reference& inReference)
        {
            const Draw::Id id = findId(inReference);

            if (id <= Draw::InvalidId)
            {
                return DrawTexture::empty();
            }

            return getDraw(id);
        }

        const DrawTexture& DrawTextureResource::getDraw(Draw::Id inId)
        {
            for (const DrawTexture& draw : m_draws)
            {
                if (draw.id != inId)
                {
                    continue;
                }

                return draw;
            }

            return DrawTexture::empty();
        }

        DrawTexture* DrawTextureResource::getDrawMutable(Draw::Id inId)
        {
            for (DrawTexture& draw : m_draws)
            {
                if (draw.id != inId)
                {
                    continue;
                }

                return &draw;
            }

            return nullptr;
        }

        Draw::Id DrawTextureResource::add(const DrawTextureData& inData)
        {
            for (DrawTexture& draw : m_draws)
            {
                if (!draw.reference.equals(inData.reference))
                {
                    continue;
                }

                populateDrawTexture(draw, inData);
                markAsDirty();

                return draw.id;
            }

            DrawTexture draw;
            draw.id = static_cast<Draw::Id>(m_draws.size());
            populateDrawTexture(draw, inData);

            m_draws.push_back(draw);

            markAsDirty();

            return draw.id;
        }

        void DrawTextureResource::reset()
        {
            if (isVolatile())
            {
                m_draws.clear();
                clearHashes();
                markAsDirty();

                return;
            }

            bool bHasVolatile = false;
            for (auto it = m_draws.begin(); it != m_draws.end();)
            {
                if (!it->bIsVolatile)
                {
                    ++it;

                    continue;
                }

                bHasVolatile = true;
                it           = m_draws.erase(it);
            }

            if (bHasVolatile)
            {
                markAsDirty();
            }
        }
    }
}
