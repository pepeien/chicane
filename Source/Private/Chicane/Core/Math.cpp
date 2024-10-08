#include "Chicane/Core/Math.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

constexpr float BOUND_SCAN_STEP_SIZE = 0.5f;
constexpr float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
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

    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
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

        Vec<3, float> point = inOrigin;

        const Vec<3, float> direction = glm::normalize(
            inDestination - inOrigin
        );

        const bool bIsXPositive = inDestination.x >= inOrigin.x;
        const bool bIsYPositive = inDestination.y >= inOrigin.y;
        const bool bIsZPositive = inDestination.z >= inOrigin.z;

        bool bHasXReachedDestination = false;
        bool bHasYReachedDestination = false;
        bool bHasZReachedDestination = false;

        std::vector<Actor*> result {};
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

            point.x += bHasXReachedDestination ? 0.0f : direction.x * LINE_TRACE_STEP_SIZE;
            point.y += bHasYReachedDestination ? 0.0f : direction.y * LINE_TRACE_STEP_SIZE;
            point.z += bHasZReachedDestination ? 0.0f : direction.z * LINE_TRACE_STEP_SIZE;

            bHasXReachedDestination = bIsXPositive ? (point.x >= inDestination.x) : (point.x <= inDestination.x);
            bHasYReachedDestination = bIsYPositive ? (point.y >= inDestination.y) : (point.y <= inDestination.y);
            bHasZReachedDestination = bIsZPositive ? (point.z >= inDestination.z) : (point.z <= inDestination.z);
        }

        return result;
    }
}