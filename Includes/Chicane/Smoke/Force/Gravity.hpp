#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Type = (Manual), Alias = (Force::Gravity))
        class CHICANE_SMOKE ForceGravity : public Module
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Force::Gravity";

            // Attributes
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME = "value";
            static constexpr inline const char* Z_ATTRIBUTE_NAME     = "z";

            // Values
            static constexpr inline const float Z_DEFAULT_VALUE = -9.8f;

        public:
            ForceGravity();

            CH_CONSTRUCTOR()
            ForceGravity(const XmlNode& inNode);

        public:
            void refresh() override;

        public:
            void tick(float inDeltaTime, Particle::List& outParticles, const PlayInfo& inPlay) override;

        public:
            CH_FIELD()
            Vec3 value;
        };
    }
}
