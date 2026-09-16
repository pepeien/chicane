#pragma once

#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Module.hpp"
#include "Chicane/Smoke/Range.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Manual)
        class CHICANE_SMOKE Spawn : public Module
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Spawn";

            // Attributes
            static constexpr inline const char* SHAPE_ATTRIBUTE_NAME     = "shape";
            static constexpr inline const char* ANCHOR_ATTRIBUTE_NAME    = "anchor";
            static constexpr inline const char* ORIGIN_ATTRIBUTE_NAME    = "origin";
            static constexpr inline const char* RATE_ATTRIBUTE_NAME      = "rate";
            static constexpr inline const char* BURST_ATTRIBUTE_NAME     = "burst";
            static constexpr inline const char* LIFETIME_ATTRIBUTE_NAME  = "lifetime";
            static constexpr inline const char* SPEED_ATTRIBUTE_NAME     = "speed";
            static constexpr inline const char* SIZE_ATTRIBUTE_NAME      = "size";
            static constexpr inline const char* SIZE_END_ATTRIBUTE_NAME  = "sizeEnd";
            static constexpr inline const char* SPREAD_ATTRIBUTE_NAME    = "spread";
            static constexpr inline const char* RADIUS_ATTRIBUTE_NAME    = "radius";
            static constexpr inline const char* COLOR_ATTRIBUTE_NAME     = "color";
            static constexpr inline const char* COLOR_END_ATTRIBUTE_NAME = "colorEnd";
            static constexpr inline const char* BLEND_ATTRIBUTE_NAME     = "blend";

            // Values
            static constexpr inline const char* SHAPE_TYPE_POINT  = "Point";
            static constexpr inline const char* SHAPE_TYPE_SPHERE = "Sphere";
            static constexpr inline const char* SHAPE_TYPE_BOX    = "Box";
            static constexpr inline const char* SHAPE_TYPE_CONE   = "Cone";

            static constexpr inline const char* ANCHOR_TYPE_ORIGIN            = "Origin";
            static constexpr inline const char* ANCHOR_TYPE_DESTINATION       = "Destination";
            static constexpr inline const char* ANCHOR_TYPE_DESTINATION_LOWER = "destination";
            static constexpr inline const char* ANCHOR_TYPE_IMPACT            = "Impact";
            static constexpr inline const char* ANCHOR_TYPE_END               = "End";

            static constexpr inline const char* BLEND_TYPE_ADDITIVE       = "Additive";
            static constexpr inline const char* BLEND_TYPE_ADD            = "Add";
            static constexpr inline const char* BLEND_TYPE_ADDITIVE_LOWER = "additive";
            static constexpr inline const float BLEND_ADDITIVE_VALUE      = 1.0f;
            static constexpr inline const float BLEND_ALPHA_VALUE         = 0.0f;

            static constexpr inline const float LIFETIME_DEFAULT_VALUE = 1.0f;
            static constexpr inline const float LIFETIME_MIN_VALUE     = 0.01f;
            static constexpr inline const float SIZE_DEFAULT_VALUE     = 1.0f;
            static constexpr inline const float LENGTH_MIN_VALUE       = 0.0001f;

        public:
            Spawn();

            CH_CONSTRUCTOR()
            Spawn(const XmlNode& inNode);

        public:
            void refresh() override;

            void emit(Particle::List& outParticles, const PlayInfo& inPlay, std::uint32_t inCount);

        public:
            CH_FIELD()
            String shape;

            CH_FIELD()
            String anchor;

            CH_FIELD()
            float rate;

            CH_FIELD()
            std::uint32_t burst;

            CH_FIELD()
            Range lifetime;

            CH_FIELD()
            Range speed;

            CH_FIELD()
            Range size;

            CH_FIELD()
            Range sizeEnd;

            CH_FIELD()
            Range spread;

            CH_FIELD()
            Range radius;

            CH_FIELD()
            Vec4 color;

            CH_FIELD()
            Vec4 colorEnd;

            CH_FIELD()
            float additive;
        };
    }
}
