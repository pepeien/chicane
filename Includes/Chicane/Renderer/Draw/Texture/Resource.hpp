#pragma once

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawTextureResource : DrawResource<ImageVendor, Image::Pixel>
        {
        public:
            bool isEmpty() const;

            const DrawTexture::List& getDraws() const;

            Draw::Id findId(const DrawTextureData& inData);
            Draw::Id findId(const Draw::Reference& inReference);

            const DrawTexture& getDraw(const Draw::Reference& inReference);
            const DrawTexture& getDraw(Draw::Id inId);

            Draw::Id add(const DrawTextureData& inData);

            void reset();

        private:
            DrawTexture::List m_draws = {};
        };
    }
}