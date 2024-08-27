#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Event.hpp"

namespace Chicane
{
    class Transformable
    {
    public:
        Transformable();

    public:
        const Vec<3, float>& getTranslation() const;
        void setRelativeTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getRotation() const;
        void setRelativeRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getScale() const;
        void setRelativeScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(const Vec<3, float>& inScale);

        const Mat<4, float>& getPosition() const;

        const Vec<3, float>& getForward() const;
        const Vec<3, float>& getRight() const;
        const Vec<3, float>& getUp() const;

        void watchTransform(
            std::function<void (const Transform&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    protected:
        void updateTranslation(const Vec<3, float>& inTranslation);
        void updateRotation(const Vec<3, float>& inRotation);
        void updateScale(const Vec<3, float>& inScale);

    protected:
        Transform m_transform;
        Mat<4, float> m_position;

        Direction m_direction;
        Quat<float> m_orientation;

        std::unique_ptr<Observable<const Transform&>> m_transformObservable;
    };
}