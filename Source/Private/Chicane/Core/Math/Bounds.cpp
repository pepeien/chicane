#include "Chicane/Core/Math/Bounds.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;

namespace Chicane
{
    Vec<3, float> Bounds::extentFromVertices(const std::vector<Vertex::Instance>& inVertices)
    {
        Vec<3, float> result {};

        float xMin = FLT_MAX;
        float xMax = -FLT_MAX;

        float yMin = FLT_MAX;
        float yMax = -FLT_MAX;

        float zMin = FLT_MAX;
        float zMax = -FLT_MAX;

        for (const Vertex::Instance& vertex : inVertices)
        {
            const Vec<3, float>& position = vertex.position;

            if (position.x < xMin)
            {
                xMin = position.x;
            }

            if (position.x > xMax)
            {
                xMax = position.x;
            }

            if (position.y < yMin)
            {
                yMin = position.y;
            }

            if (position.y > yMax)
            {
                yMax = position.y;
            }

            if (position.z < zMin)
            {
                zMin = position.z;
            }

            if (position.z > zMax)
            {
                zMax = position.z;
            }
        }

        result.x = std::abs(xMax) + std::abs(xMin);
        result.y = std::abs(yMax) + std::abs(yMin);
        result.z = std::abs(zMax) + std::abs(zMin);

        return result;
    }

    Bounds::Bounds(const std::vector<Vertex::Instance>& inVertices)
    {
        extent = Bounds::extentFromVertices(inVertices);
    }

    bool Bounds::contains(const Bounds& inBounds) const
    {
        const Vec<3, float> frontOrigin = inBounds.origin - inBounds.extent;
        Vec<3, float> frontPoint        = frontOrigin;

        const Vec<3, float> backOrigin = inBounds.origin + inBounds.extent;
        Vec<3, float> backPoint        = backOrigin;

        while (frontPoint.z < backPoint.z)
        {
            frontPoint.x = frontOrigin.x;
            frontPoint.y = frontOrigin.y;

            backPoint.x = backOrigin.x;
            backPoint.y = backOrigin.y;

            while (frontPoint.y < backPoint.y)
            {
                frontPoint.x = frontOrigin.x;
                backPoint.x  = backOrigin.x;

                while (frontPoint.x < backPoint.x)
                {
                    if (contains(backPoint) || contains(frontPoint))
                    {
                        return true;
                    }

                    frontPoint.x += BOUND_SCAN_STEP_SIZE;
                    backPoint.x  -= BOUND_SCAN_STEP_SIZE;
                }

                frontPoint.y += BOUND_SCAN_STEP_SIZE;
                backPoint.y  -= BOUND_SCAN_STEP_SIZE;
            }

            frontPoint.z += BOUND_SCAN_STEP_SIZE;
            backPoint.z  -= BOUND_SCAN_STEP_SIZE;
        }

        return false;
    }

    bool Bounds::contains(const Vec<3, float>& inPoint) const
    {
        const Vec<3, float> end = origin + extent;

        const bool bIsWithinX = inPoint.x >= origin.x && inPoint.x <= end.x;
        const bool bIsWithinY = inPoint.y >= origin.y && inPoint.y <= end.y;
        const bool bIsWithinZ = inPoint.z >= origin.z && inPoint.z <= end.z;

        return bIsWithinX && bIsWithinY && bIsWithinZ;
    }
}