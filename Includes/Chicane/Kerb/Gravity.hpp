#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Gravity/Planet.hpp"

namespace Chicane
{
    namespace Kerb
    {
        struct CHICANE_KERB Gravity
        {
        public:
            // Body gravity factor (multiplies world gravity).
            static constexpr float FactorNone = 0.0f;
            static constexpr float FactorFull = 1.0f;

            // Surface gravitational acceleration magnitudes (m/s^2).
            static constexpr float Zero    = 0.0f;
            static constexpr float Mercury = 3.70f;
            static constexpr float Venus   = 8.87f;
            static constexpr float Earth   = 9.81f;
            static constexpr float Moon    = 1.62f;
            static constexpr float Mars    = 3.71f;
            static constexpr float Jupiter = 24.79f;
            static constexpr float Saturn  = 10.44f;
            static constexpr float Uranus  = 8.69f;
            static constexpr float Neptune = 11.15f;
            static constexpr float Pluto   = 0.62f;

        public:
            static constexpr float of(Planet inPlanet)
            {
                switch (inPlanet)
                {
                case Planet::Mercury:
                    return Mercury;

                case Planet::Venus:
                    return Venus;

                case Planet::Moon:
                    return Moon;

                case Planet::Mars:
                    return Mars;

                case Planet::Jupiter:
                    return Jupiter;

                case Planet::Saturn:
                    return Saturn;

                case Planet::Uranus:
                    return Uranus;

                case Planet::Neptune:
                    return Neptune;

                case Planet::Pluto:
                    return Pluto;

                case Planet::Earth:
                default:
                    return Earth;
                }
            }

            static Vec3 down(float inAcceleration) { return Vec3::Up() * -inAcceleration; }
        };
    }
}
