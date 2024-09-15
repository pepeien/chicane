#include "Chicane/Core/Math.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

#include <iostream>

constexpr float BOUND_SCAN_STEP_SIZE = 0.1f;
constexpr float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
    bool Bounds::contains(const Bounds& inBounds) const
    {
        const Vec<3, float> backOrigin = inBounds.origin;
        Vec<3, float> backPoint        = backOrigin;

        const Vec<3, float> frontOrigin = inBounds.origin + inBounds.extent;
        Vec<3, float> frontPoint        = frontOrigin;

        while (backPoint.z < frontPoint.z)
        {
            backPoint.x = backOrigin.x;
            backPoint.y = backOrigin.y;

            frontPoint.x = frontOrigin.x;
            frontPoint.y = frontOrigin.y;

            while (backPoint.y < frontPoint.y)
            {
                backPoint.x  = backOrigin.x;
                frontPoint.x = frontOrigin.x;

                while (backPoint.x < frontPoint.x)
                {
                    if (contains(backPoint) || contains(frontPoint))
                    {
                        return true;
                    }

                    backPoint.x  += BOUND_SCAN_STEP_SIZE;
                    frontPoint.x -= BOUND_SCAN_STEP_SIZE;
                }

                backPoint.y  += BOUND_SCAN_STEP_SIZE;
                frontPoint.y -= BOUND_SCAN_STEP_SIZE;
            }

            backPoint.z  += BOUND_SCAN_STEP_SIZE;
            frontPoint.z -= BOUND_SCAN_STEP_SIZE;
        }

        return false;
    }

    bool Bounds::contains(const Vec<3, float>& inPoint) const
    {
        bool bIsWithinX = inPoint.x >= origin.x && inPoint.x <= (origin.x + extent.x);
        bool bIsWithinY = inPoint.y >= origin.y && inPoint.y <= (origin.y + extent.y);
        bool bIsWithinZ = inPoint.z >= origin.z && inPoint.z <= (origin.z + extent.z);

        return bIsWithinX && bIsWithinY && bIsWithinZ;
    }

    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
        const Vec<3, float>& inDirection,
        const std::vector<Actor*>& inIgnoredActors
    )
    {
        if (!hasActiveLevel())
        {
            return {};
        }

        const std::vector<Actor*>& actors = getActiveLevel()->getActors();

        if (actors.empty())
        {
            return {};
        }

        std::vector<Actor*> result {};

        const bool bIsXPositive = inDestination.x >= inOrigin.x;
        const bool bIsYPositive = inDestination.y >= inOrigin.y;
        const bool bIsZPositive = inDestination.z >= inOrigin.z;

        Vec<3, float> point = inOrigin;

        bool bHasXReachedDestination = bIsXPositive ? (point.x >= inDestination.x) : (point.x <= inDestination.x);
        bool bHasYReachedDestination = bIsYPositive ? (point.y >= inDestination.y) : (point.y <= inDestination.y);
        bool bHasZReachedDestination = bIsZPositive ? (point.z >= inDestination.z) : (point.z <= inDestination.z);

        while (!bHasXReachedDestination || !bHasYReachedDestination || !bHasZReachedDestination)
        {
            for (Actor* actor : actors)
            {
                // Checking collision first is more computational cost effective
                if (!actor->isCollidingWith(point))
                {
                    continue;
                }

                bool bWillIgnored = std::find(
                    inIgnoredActors.begin(),
                    inIgnoredActors.end(),
                    actor
                ) != inIgnoredActors.end();
                bool bWasTraced = std::find(
                    result.begin(),
                    result.end(),
                    actor
                ) != result.end();

                if (bWillIgnored || bWasTraced)
                {
                    continue;
                }

                result.push_back(actor);
            }

            bHasXReachedDestination = bIsXPositive ? (point.x >= inDestination.x) : (point.x <= inDestination.x);
            bHasYReachedDestination = bIsYPositive ? (point.y >= inDestination.y) : (point.y <= inDestination.y);
            bHasZReachedDestination = bIsZPositive ? (point.z >= inDestination.z) : (point.z <= inDestination.z);

            point.x += bHasXReachedDestination ? 0.0f : (inDirection.x * LINE_TRACE_STEP_SIZE);
            point.y += bHasYReachedDestination ? 0.0f : (inDirection.y * LINE_TRACE_STEP_SIZE);
            point.z += bHasZReachedDestination ? 0.0f : (inDirection.z * LINE_TRACE_STEP_SIZE);
        }

        return result;
    }
}