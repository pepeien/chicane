#include "Chicane/Game.hpp"

#include "Chicane/Application.hpp"

constexpr float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
    std::vector<Actor*> traceLine(
        const Vec<3, float>& inOrigin,
        const Vec<3, float>& inDestination,
        const std::vector<Actor*>& inIgnoredActors
    )
    {
        if (!Application::hasLevel())
        {
            return {};
        }

        const std::vector<Actor*>& actors = Application::getLevel()->getActors();

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