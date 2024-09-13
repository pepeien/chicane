#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Vec : glm::vec<O, T, Q>
    {
        template<typename... A>
        Vec(A ...args)
            : glm::vec<O, T, Q>(args...)
        {}

        T dot(const Vec<O, T, Q>& inTarget) const
        {
            return glm::dot<O, T, Q>(*this, inTarget);
        }
    };

    
    template<std::uint32_t O, typename T, glm::qualifier Q = glm::packed_highp>
    struct Mat : glm::mat<O, O, T, Q>
    {
        template<typename... A>
        Mat(A ...args)
            : glm::mat<O, O, T, Q>(args...)
        {}
    };

    template<typename T, glm::qualifier Q = glm::packed_highp>
    struct Quat : glm::qua<T, Q>
    {
        template<typename... A>
        Quat(A ...args)
            : glm::qua<T, Q>(args...)
        {}
    };

    const Vec<2, float> Vec2Zero = Vec<2, float>(0.0f);
    const Vec<3, float> Vec3Zero = Vec<3, float>(0.0f);

    struct Transform
    {
        Vec<3, float> translation = Vec3Zero;
        Vec<3, float> rotation    = Vec3Zero;
        Vec<3, float> scale       = Vec<3, float>(1.0f);
    };

    struct Direction
    {
        Vec<3, float> forward = FORWARD_DIRECTION;
        Vec<3, float> up      = UP_DIRECTION;
        Vec<3, float> right   = RIGHT_DIRECTION;
    };

    struct Bounds
    {
    public:
        bool contains(const Bounds& inBounds) const
        {
            Vec<3, float> leftPoint  = inBounds.origin;
            Vec<3, float> rightPoint = inBounds.origin;
            rightPoint.x += inBounds.extent.x;

            for (float zLeft = leftPoint.z, zRight = rightPoint.z; zLeft <= zRight; zLeft += 0.1f, zRight -= 0.1f)
            {
                leftPoint.x = inBounds.origin.x;
                leftPoint.y = inBounds.origin.y;
                leftPoint.z = zLeft;

                rightPoint.x = leftPoint.x + inBounds.extent.x;
                rightPoint.y = leftPoint.y + inBounds.extent.y;
                rightPoint.z = zRight;

                for (float yLeft = leftPoint.y, yRight = rightPoint.y; yLeft <= yRight; yLeft += 0.1f, yRight -= 0.1f)
                {
                    leftPoint.x = inBounds.origin.x;
                    leftPoint.y = yLeft;

                    leftPoint.x = leftPoint.x + inBounds.extent.x;
                    leftPoint.y = yRight;

                    for (float xLeft = leftPoint.x, xRight = rightPoint.x; xLeft <= xRight; xLeft += 0.1f, xRight -= 0.1f)
                    {
                        leftPoint.x = xLeft;

                        rightPoint.x = xRight;

                        if (contains(leftPoint) || contains(rightPoint))
                        {
                            return true;
                        }

                        if (xLeft <= xRight)
                        {
                            break;
                        }
                    }

                    if (yLeft <= yRight)
                    {
                        break;
                    }
                }

                if (zLeft <= zRight)
                {
                    break;
                }
            }

            return false;
        }

        bool contains(const Vec<3, float>& inPoint) const
        {
            bool bIsWithinX = inPoint.x >= origin.x && inPoint.x <= (origin.x + extent.x);
            bool bIsWithinY = inPoint.y >= origin.y && inPoint.y <= (origin.y + extent.y);
            bool bIsWithinZ = inPoint.z >= origin.z && inPoint.z <= (origin.y + extent.z);

            return bIsWithinX && bIsWithinY && bIsWithinZ;
        }

    public:
        Vec<3, float> extent = Vec3Zero;

        Vec<3, float> top    = Vec3Zero;
        Vec<3, float> origin = Vec3Zero;
        Vec<3, float> center = Vec3Zero;
    };
}