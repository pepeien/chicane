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
        // Transform
        const Vec<3, float>& getTranslation() const;
        void setRelativeTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getRotation() const;
        void setRelativeRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getScale() const;
        void setRelativeScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(const Vec<3, float>& inScale);

        const Transform& getTransform() const;
        const Mat<4, float>& getPosition() const;

        // Orientation
        const Quat<float>& getOrientation() const;

        const Vec<3, float>& getForward() const;
        const Vec<3, float>& getRight() const;
        const Vec<3, float>& getUp() const;

        // Bounds
        const Vec<3, float>& getTop() const;
        const Vec<3, float>& getCenter() const;
        const Vec<3, float>& getOrigin() const;

        const Vec<3, float>& getExtent() const;

        const Bounds& getBounds() const;
        void refreshBounds();
        void setBounds(const Bounds& inBounds);

        // Events
        Subscription<const Transform&>* watchTransform(
            std::function<void (const Transform&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    protected:
        void setTranslation(const Mat<4, float>& inBase, const Vec<3, float>& inTranslation);
        void setRotation(const Mat<4, float>& inBase, const Vec<3, float>& inRotation);
        void setScale(const Mat<4, float>& inBase, const Vec<3, float>& inScale);

        void refreshPosition(const Mat<4, float>& inBase);
        void refreshOrientation();

    protected:
        // Transform
        Transform                                     m_transform;
        Mat<4, float>                                 m_position;

        // Direction
        Direction                                     m_direction;
        Quat<float>                                   m_orientation;

        // Bounds
        Bounds                                        m_baseBounds;
        Bounds                                        m_currentBounds;

        // Events
        std::unique_ptr<Observable<const Transform&>> m_transformObservable;
    };
}