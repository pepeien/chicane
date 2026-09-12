#pragma once

#include <cstdint>

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeCylinder : public SceneTraceShape
    {
    public:
        static constexpr inline std::uint32_t DEFAULT_SEGEMENT_COUNT = 16;

    public:
        explicit SceneTraceShapeCylinder(float inRadius, std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT);
        SceneTraceShapeCylinder() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        float getRadiusAt(float inFraction) const;

    public:
        float         radius       = 0.0f;
        std::uint32_t segmentCount = DEFAULT_SEGEMENT_COUNT;
    };
}
