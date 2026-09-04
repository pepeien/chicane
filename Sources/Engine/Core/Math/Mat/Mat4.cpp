#include "Chicane/Core/Math/Mat/Mat4.hpp"

#include <cfloat>
#include <cmath>

#include <glm/gtc/matrix_inverse.hpp>

namespace Chicane
{

    bool Mat4::toPosition(
        const Vec3& inWorldPosition,
        const Mat4& inView,
        const Mat4& inProjection,
        const Vec2& inViewport,
        Vec2&       outPosition
    )
    {
        const Vec4 clip = inProjection * inView * Vec4(inWorldPosition, 1.0f);

        if (std::fabs(clip.w) < FLT_EPSILON)
        {
            outPosition = Vec2::Zero();

            return false;
        }

        const float inverseW = 1.0f / clip.w;
        const float ndcX     = clip.x * inverseW;
        const float ndcY     = clip.y * inverseW;

        outPosition.x = (ndcX * 0.5f + 0.5f) * inViewport.x;
        outPosition.y = (0.5f - ndcY * 0.5f) * inViewport.y;

        return clip.w > 0.0f;
    }

    Vec2 Mat4::toPosition(
        const Vec3& inWorldPosition, const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport
    )
    {
        Vec2 result = Vec2::Zero();
        toPosition(inWorldPosition, inView, inProjection, inViewport, result);

        return result;
    }

    Vec3 Mat4::getTranslation() const
    {
        return Vec3((*this)[3]);
    }

    Mat4 Mat4::inverse() const
    {
        return glm::inverse(static_cast<const glm::mat4&>(*this));
    }

    bool Mat4::toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport, Vec2& outPosition) const
    {
        return toPosition(getTranslation(), inView, inProjection, inViewport, outPosition);
    }

    Vec2 Mat4::toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport) const
    {
        return toPosition(getTranslation(), inView, inProjection, inViewport);
    }

    bool Mat4::fromPosition(
        const Vec2& inPosition,
        const Mat4& inView,
        const Mat4& inProjection,
        const Vec2& inViewport,
        Vec3&       outNear,
        Vec3&       outFar
    )
    {
        outNear = Vec3::Zero();
        outFar  = Vec3::Zero();

        if (inViewport.x <= 0.0f || inViewport.y <= 0.0f)
        {
            return false;
        }

        const float ndcX = (inPosition.x / inViewport.x) * 2.0f - 1.0f;
        const float ndcY = 1.0f - (inPosition.y / inViewport.y) * 2.0f;

        const Mat4 inverse  = (inProjection * inView).inverse();
        const Vec4 nearClip = inverse * Vec4(ndcX, ndcY, -1.0f, 1.0f);
        const Vec4 farClip  = inverse * Vec4(ndcX, ndcY, 1.0f, 1.0f);

        if (std::fabs(nearClip.w) < FLT_EPSILON || std::fabs(farClip.w) < FLT_EPSILON)
        {
            return false;
        }

        outNear = Vec3(nearClip.x / nearClip.w, nearClip.y / nearClip.w, nearClip.z / nearClip.w);
        outFar  = Vec3(farClip.x / farClip.w, farClip.y / farClip.w, farClip.z / farClip.w);

        return true;
    }
}
