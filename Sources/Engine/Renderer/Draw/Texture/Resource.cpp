#include "Chicane/Renderer/Draw/Texture/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        bool DrawTextureResource::isEmpty() const
        {
            return m_draws.empty();
        }

        const DrawTexture::List& DrawTextureResource::getDraws() const
        {
            return m_draws;
        }

        Draw::Id DrawTextureResource::findId(const DrawTextureData& inData)
        {
            ImageVendor vendor = inData.image.getVendor();

            return findHash(
                &vendor,
                sizeof(ImageVendor),
                inData.image.getPixels(),
                inData.image.getSize() * sizeof(Image::Pixel)
            );
        }

        Draw::Id DrawTextureResource::findId(const Draw::Reference& inReference)
        {
            for (const DrawTexture& draw : m_draws)
            {
                if (!draw.reference.equals(inReference))
                {
                    continue;
                }

                return draw.id;
            }

            return Draw::UnknownId;
        }

        const DrawTexture& DrawTextureResource::getDraw(const Draw::Reference& inReference)
        {
            const Draw::Id id = findId(inReference);

            if (id <= Draw::UnknownId)
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

        Draw::Id DrawTextureResource::add(const DrawTextureData& inData)
        {
            Draw::Id id = findId(inData);

            if (id > Draw::UnknownId)
            {
                return id;
            }

            DrawTexture draw;
            draw.id        = m_draws.size();
            draw.reference = inData.reference;
            draw.image     = inData.image;

            m_draws.push_back(draw);

            ImageVendor vendor = inData.image.getVendor();

            addHash(
                &vendor,
                sizeof(ImageVendor),
                inData.image.getPixels(),
                inData.image.getSize() * sizeof(Image::Pixel),
                draw.id
            );

            return draw.id;
        }

        void DrawTextureResource::reset()
        {
            m_draws.clear();

            clearHashes();
        }
    }
}