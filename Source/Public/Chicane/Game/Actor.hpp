#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    class Actor
    {
    public:
        Actor();
        virtual ~Actor() = default;

    public:
        virtual void onTick(float inDeltaTime) { return; };

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);

        const Vec<3, float>& getTranslation() const;
        const Vec<3, float>& getRotation() const;
        const Vec<3, float>& getScale() const;
        const Mat<4, float>& getPosition() const;

        void setRelativeTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        // Rotation values should be in degrees
        void setRelativeRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        void setRelativeScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(const Vec<3, float>& inScale);

    protected:
        void updateTranslation(const Vec<3, float>& inTranslation);
        void updateRotation(const Vec<3, float>& inRotation);
        void updateScale(const Vec<3, float>& inScale);

    protected:
        bool m_canTick;

        Mat<4, float> m_position;
        Transform m_transform;
    };
} 