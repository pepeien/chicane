#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Drift.hpp"

namespace Chicane
{
    namespace Drift
    {
        inline float mix(float inFrom, float inTo, float inTime)
        {
            return inFrom + ((inTo - inFrom) * inTime);
        }

        inline Vec2 mix(const Vec2& inFrom, const Vec2& inTo, float inTime)
        {
            return Vec2(mix(inFrom.x, inTo.x, inTime), mix(inFrom.y, inTo.y, inTime));
        }

        inline Vec3 mix(const Vec3& inFrom, const Vec3& inTo, float inTime)
        {
            return Vec3(
                mix(inFrom.x, inTo.x, inTime),
                mix(inFrom.y, inTo.y, inTime),
                mix(inFrom.z, inTo.z, inTime)
            );
        }

        inline Vec4 mix(const Vec4& inFrom, const Vec4& inTo, float inTime)
        {
            return Vec4(
                mix(inFrom.x, inTo.x, inTime),
                mix(inFrom.y, inTo.y, inTime),
                mix(inFrom.z, inTo.z, inTime),
                mix(inFrom.w, inTo.w, inTime)
            );
        }

        inline Color::Rgba mix(const Color::Rgba& inFrom, const Color::Rgba& inTo, float inTime)
        {
            auto channel = [inTime](std::uint8_t inStart, std::uint8_t inEnd) -> std::uint8_t
            {
                const float value = mix(static_cast<float>(inStart), static_cast<float>(inEnd), inTime);

                return static_cast<std::uint8_t>(std::round(std::clamp(value, 0.0f, 255.0f)));
            };

            return Color::Rgba(channel(inFrom.r, inTo.r), channel(inFrom.g, inTo.g), channel(inFrom.b, inTo.b), channel(inFrom.a, inTo.a));
        }

        CHICANE_DRIFT std::vector<float> mix(
            const std::vector<float>& inFrom, const std::vector<float>& inTo, float inTime
        );
    }
}
