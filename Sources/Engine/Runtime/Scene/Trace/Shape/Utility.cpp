#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include "Chicane/Core/Time.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    namespace SceneTraceShapeUtility
    {
        float clamp(float inValue, float inMin, float inMax)
        {
            return std::max(inMin, std::min(inMax, inValue));
        }

        float lengthSquared(const Vec3& inValue)
        {
            return inValue.dot(inValue);
        }

        float axisLength(const Vec3& inOrigin, const Vec3& inDestination)
        {
            return std::sqrt(lengthSquared(inDestination - inOrigin));
        }

        Vec3 closestPointOnSegment(const Vec3& inPoint, const Vec3& inStart, const Vec3& inEnd, float& outFraction)
        {
            const Vec3  delta  = inEnd - inStart;
            const float length = lengthSquared(delta);
            if (length <= FLT_EPSILON)
            {
                outFraction = 0.0f;

                return inStart;
            }

            outFraction = clamp((inPoint - inStart).dot(delta) / length, 0.0f, 1.0f);

            return inStart + delta * outFraction;
        }

        float distancePointToBounds(const Vec3& inPoint, const Bounds3D& inBounds)
        {
            const Vec3& min = inBounds.getMin().transformed;
            const Vec3& max = inBounds.getMax().transformed;

            const float dx = std::max(std::max(min.x - inPoint.x, 0.0f), inPoint.x - max.x);
            const float dy = std::max(std::max(min.y - inPoint.y, 0.0f), inPoint.y - max.y);
            const float dz = std::max(std::max(min.z - inPoint.z, 0.0f), inPoint.z - max.z);

            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        Vec3 closestPointOnBounds(const Vec3& inPoint, const Bounds3D& inBounds)
        {
            const Vec3& min = inBounds.getMin().transformed;
            const Vec3& max = inBounds.getMax().transformed;

            return Vec3(clamp(inPoint.x, min.x, max.x), clamp(inPoint.y, min.y, max.y), clamp(inPoint.z, min.z, max.z));
        }

        bool buildAxisBasis(const Vec3& inDirection, Vec3& outRight, Vec3& outUp)
        {
            const float length = std::sqrt(lengthSquared(inDirection));
            if (length <= FLT_EPSILON)
            {
                return false;
            }

            const Vec3 axis = inDirection / length;
            const Vec3 hint = std::abs(axis.z) < 0.999f ? Vec3::Up() : Vec3::Right();

            outRight                = hint.cross(axis);
            const float rightLength = std::sqrt(lengthSquared(outRight));
            if (rightLength <= FLT_EPSILON)
            {
                return false;
            }

            outRight = outRight / rightLength;
            outUp    = axis.cross(outRight);

            return true;
        }

        bool intersectsRadial(
            const Vec3&     inOrigin,
            const Vec3&     inDestination,
            const Bounds3D& inBounds,
            float           inRadiusAtFraction,
            float&          outEnter
        )
        {
            const Vec3 center   = inBounds.getCenter();
            float      fraction = 0.0f;
            const Vec3 closest  = closestPointOnSegment(center, inOrigin, inDestination, fraction);
            if (distancePointToBounds(closest, inBounds) > inRadiusAtFraction)
            {
                return false;
            }

            outEnter = fraction;

            return true;
        }

        void fillResponse(
            SceneTraceResponse& outResponse,
            const Vec3&         inOrigin,
            const Vec3&         inDestination,
            const Bounds3D&     inBounds,
            float               inEnter
        )
        {
            const Vec3  delta  = inDestination - inOrigin;
            const float length = std::sqrt(lengthSquared(delta));

            outResponse.timestamp   = Time::Clock::now();
            outResponse.traceStart  = inOrigin;
            outResponse.traceEnd    = inDestination;
            outResponse.distance    = inEnter * length;
            outResponse.location    = inOrigin + delta * inEnter;
            outResponse.impactPoint = closestPointOnBounds(outResponse.location, inBounds);
        }
    }
}
