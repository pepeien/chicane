#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Changeable.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_CORE Transform2D : public Changeable
    {
    public:
        Transform2D();

    public:
        const Mat3& getMatrix() const;
        void setTransform(const Transform2D& inTransform);

        // Translation
        const Vec2& getTranslation() const;
        void addTranslation(float inValue);
        void addTranslation(float inX, float inY);
        void addTranslation(const Vec2& inValue);
        void setTranslation(float inValue);
        void setTranslation(float inX, float inY);
        void setTranslation(const Vec2& inValue);

        // Rotation
        float getRotation() const;
        void addRotation(float inDegrees);
        void setRotation(float inDegrees);

        // Scale
        const Vec2& getScale() const;
        void addScale(float inValue);
        void addScale(float inX, float inY);
        void addScale(const Vec2& inValue);
        void setScale(float inValue);
        void setScale(float inX, float inY);
        void setScale(const Vec2& inValue);

        void lookAt(const Vec2& inTarget);

    protected:
        void refresh();

    public:
        CH_FIELD()
        Vec2 translation;

        CH_FIELD()
        float rotation;

        CH_FIELD()
        Vec2 scale;

    protected:
        Mat3 m_matrix;
    };
}
