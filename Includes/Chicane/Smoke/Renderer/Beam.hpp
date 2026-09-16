#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Type = (Manual), Alias = (Renderer::Beam))
        class CHICANE_SMOKE RendererBeam : public Module
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Renderer::Beam";

            // Attributes
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME = "width";
            static constexpr inline const char* BLEND_ATTRIBUTE_NAME = "blend";
            static constexpr inline const char* COLOR_ATTRIBUTE_NAME = "color";

            // Values
            static constexpr inline const char* BLEND_TYPE_ADDITIVE  = "Additive";
            static constexpr inline const float BLEND_ADDITIVE_VALUE = 1.0f;
            static constexpr inline const float BLEND_ALPHA_VALUE    = 0.0f;

            static constexpr inline const float WIDTH_DEFAULT_VALUE = 2.5f;

            static constexpr inline const float LIFETIME_DEFAULT_VALUE = 1.0f;

            static constexpr inline const float LENGTH_MIN_VALUE = 0.0001f;

            static constexpr inline const float COLOR_R_DEFAULT_VALUE = 1.0f;
            static constexpr inline const float COLOR_G_DEFAULT_VALUE = 0.35f;
            static constexpr inline const float COLOR_B_DEFAULT_VALUE = 0.15f;
            static constexpr inline const float COLOR_A_DEFAULT_VALUE = 1.0f;

        public:
            CH_CONSTRUCTOR()
            RendererBeam(const XmlNode& inNode);

            RendererBeam();

        public:
            void refresh() override;

        public:
            void collect(
                const Particle::List& inParticles, const PlayInfo& inPlay, Particle::List& outDraws
            ) const override;

        public:
            CH_FIELD()
            float width;

            CH_FIELD()
            String blend;

            CH_FIELD()
            Vec4 color;
        };
    }
}
