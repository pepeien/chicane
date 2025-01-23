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
        // Translation
        const Vec<3, float>& getTranslation() const;

        void addTranslation(float inTranslation);
        void addTranslation(float inX, float inY, float inZ);
        void addTranslation(const Vec<3, float>& inTranslation);

        void setTranslation(float inTranslation);
        void setTranslation(float inX, float inY, float inZ);
        void setTranslation(const Vec<3, float>& inTranslation);

        // Rotation
        const Vec<3, float>& getRotation() const;

        void addYaw(float inDegrees);
        void setYaw(float inDegrees);

        void addRoll(float inDegrees);
        void setRoll(float inDegrees);

        void addPitch(float inDegrees);
        void setPitch(float inDegrees);

        void addRotation(float inRotation);
        void addRotation(float inYaw, float inRoll, float inPitch);
        void addRotation(const Vec<3, float>& inRotation);

        void setRotation(float inRotation);
        void setRotation(float inYaw, float inRoll, float inPitch);
        void setRotation(const Vec<3, float>& inRotation);

        // Scaling
        const Vec<3, float>& getScale() const;

        void addScale(float inScale);
        void addScale(float inX, float inY, float inZ);
        void addScale(const Vec<3, float>& inScale);

        void setScale(float inScale);
        void setScale(float inX, float inY, float inZ);
        void setScale(const Vec<3, float>& inScale);

        // Transform
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
            std::function<void (const Transform&)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    protected:
        void refreshCoordinates();
        void refreshOrientation();
        void refreshDirections();
        void refreshPosition();

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