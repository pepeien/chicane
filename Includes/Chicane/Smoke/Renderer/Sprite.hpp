#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Type = (Manual), Alias = (Renderer::Sprite))
        class CHICANE_SMOKE RendererSprite : public Module
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Renderer::Sprite";

            // Attributes
            static constexpr inline const char* BLEND_ATTRIBUTE_NAME   = "blend";
            static constexpr inline const char* TEXTURE_ATTRIBUTE_NAME = "texture";

            // Values
            static constexpr inline const char* BLEND_TYPE_ADDITIVE  = "Additive";
            static constexpr inline const float BLEND_ADDITIVE_VALUE = 1.0f;
            static constexpr inline const float BLEND_ALPHA_VALUE    = 0.0f;

        public:
            CH_CONSTRUCTOR()
            RendererSprite(const XmlNode& inNode);

            RendererSprite();

        public:
            void refresh() override;

        public:
            void collect(
                const Particle::List& inParticles, const PlayInfo& inPlay, Particle::List& outDraws
            ) const override;

        public:
            CH_FIELD()
            String blend;

            CH_FIELD()
            String texture;
        };
    }
}
