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

        const Vec<3, float>& getRelativeTranslation() const;
        void addRelativeTranslation(float inTranslation);
        void addRelativeTranslation(float inX, float inY, float inZ);
        void addRelativeTranslation(const Vec<3, float>& inTranslation);
        void setRelativeTranslation(float inTranslation);
        void setRelativeTranslation(float inX, float inY, float inZ);
        void setRelativeTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getWorldTranslation() const;
        void addWorldTranslation(float inTranslation);
        void addWorldTranslation(float inX, float inY, float inZ);
        void addWorldTranslation(const Vec<3, float>& inTranslation);
        void setWorldTranslation(float inTranslation);
        void setWorldTranslation(float inX, float inY, float inZ);
        void setWorldTranslation(const Vec<3, float>& inTranslation);

        // Rotation
        const Vec<3, float>& getRotation() const;

        const Vec<3, float>& getRelativeRotation() const;
        void addRelativeRotation(float inRotation);
        void addRelativeRotation(float inPitch, float inRoll, float inYaw);
        void addRelativeRotation(const Vec<3, float>& inRotation);
        void setRelativeRotation(float inRotation);
        void setRelativeRotation(float inPitch, float inRoll, float inYaw);
        void setRelativeRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getWorldRotation() const;
        void addWorldRotation(float inRotation);
        void addWorldRotation(float inPitch, float inRoll, float inYaw);
        void addWorldRotation(const Vec<3, float>& inRotation);
        void setWorldRotation(float inRotation);
        void setWorldRotation(float inPitch, float inRoll, float inYaw);
        void setWorldRotation(const Vec<3, float>& inRotation);

        // Scaling
        const Vec<3, float>& getScale() const;

        const Vec<3, float>& getRelativeScale() const;
        void addRelativeScale(float inScale);
        void addRelativeScale(float inX, float inY, float inZ);
        void addRelativeScale(const Vec<3, float>& inScale);
        void setRelativeScale(float inScale);
        void setRelativeScale(float inX, float inY, float inZ);
        void setRelativeScale(const Vec<3, float>& inScale);

        const Vec<3, float>& getWorldScale() const;
        void addWorldScale(float inScale);
        void addWorldScale(float inX, float inY, float inZ);
        void addWorldScale(const Vec<3, float>& inScale);
        void setWorldScale(float inScale);
        void setWorldScale(float inX, float inY, float inZ);
        void setWorldScale(const Vec<3, float>& inScale);

        // Transform
        const Transform::Combined& getTransform() const;

        const Transform::Instance& getRelativeTransform() const;
        void setRelativeTransform(const Transform::Instance& inTransform);

        const Transform::Instance& getWorldTransform() const;
        void setWorldTransform(const Transform::Instance& inTransform);

        // Position
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
        Subscription<void*>* watchTransform(
            std::function<void (void*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    private:
        void setTranslation(const Vec<3, float>& inTranslation, Vec<3, float>& outTranslation);
        void setRotation(const Vec<3, float>& inRotation, Vec<3, float>& ouRotation);
        void setScale(const Vec<3, float>& inScale, Vec<3, float>& outScale);

        void refreshCoordinates();
        void refreshOrientation();
        void refreshDirections();
        void refreshPosition();

    private:
        // Transform
        Transform::Combined                m_transform;
        Mat<4, float>                      m_position;

        // Direction
        Direction                          m_direction;
        Quat<float>                        m_orientation;

        // Bounds
        Bounds                             m_baseBounds;
        Bounds                             m_currentBounds;

        // Events
        std::unique_ptr<Observable<void*>> m_transformObservable;
    };
}