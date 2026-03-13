#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Changeable.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Rotator.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_CORE Transform : public Changeable
    {
    public:
        Transform();

    public:
        const Mat4& getMatrix() const;
        void setTransform(const Transform& inTransform);

        // Translation
        const Vec3& getTranslation() const;
        void addTranslation(float inValue);
        void addTranslation(float inX, float inY, float inZ);
        void addTranslation(const Vec3& inValue);
        void setTranslation(float inValue);
        void setTranslation(float inX, float inY, float inZ);
        void setTranslation(const Vec3& inValue);

        // Rotation
        const Vec3& getForward() const;
        const Vec3& getRight() const;
        const Vec3& getUp() const;

        const Rotator& getRotation() const;

        template <typename... Args>
        inline void addLocalRotation(Args... inArgs)
        {
            rotation.addLocal(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void addWorldRotation(Args... inArgs)
        {
            rotation.addWorld(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setRotation(Args... inArgs)
        {
            rotation.set(std::forward<Args>(inArgs)...);

            refresh();
        }

        void lookAt(const Vec3& inTarget);

        // Scale
        const Vec3& getScale() const;
        void addScale(float inValue);
        void addScale(float inX, float inY, float inZ);
        void addScale(const Vec3& inValue);
        void setScale(float inValue);
        void setScale(float inX, float inY, float inZ);
        void setScale(const Vec3& inValue);

    protected:
        void refresh();

    public:
        CH_FIELD()
        Vec3 translation;

        CH_FIELD()
        Rotator rotation;

        CH_FIELD()
        Vec3 scale;

    protected:
        Mat4 m_matrix;
    };
}