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

        const Vec<3, float>& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(float inTranslation);
        void addAbsoluteTranslation(float inX, float inY, float inZ);
        void addAbsoluteTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(float inTranslation);
        void setAbsoluteTranslation(float inX, float inY, float inZ);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        // Rotation
        const Vec<3, float>& getRotation() const;

        const Vec<3, float>& getRelativeRotation() const;
        void addRelativeRotation(float inRotation);
        void addRelativeRotation(float inPitch, float inRoll, float inYaw);
        void addRelativeRotation(const Vec<3, float>& inRotation);
        void setRelativeRotation(float inRotation);
        void setRelativeRotation(float inPitch, float inRoll, float inYaw);
        void setRelativeRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getAbsoluteRotation() const;
        void addAbsoluteRotation(float inRotation);
        void addAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void addAbsoluteRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(float inRotation);
        void setAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        // Scaling
        const Vec<3, float>& getScale() const;

        const Vec<3, float>& getRelativeScale() const;
        void addRelativeScale(float inScale);
        void addRelativeScale(float inX, float inY, float inZ);
        void addRelativeScale(const Vec<3, float>& inScale);
        void setRelativeScale(float inScale);
        void setRelativeScale(float inX, float inY, float inZ);
        void setRelativeScale(const Vec<3, float>& inScale);

        const Vec<3, float>& getAbsoluteScale() const;
        void addAbsoluteScale(float inScale);
        void addAbsoluteScale(float inX, float inY, float inZ);
        void addAbsoluteScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(float inScale);
        void setAbsoluteScale(float inX, float inY, float inZ);
        void setAbsoluteScale(const Vec<3, float>& inScale);

        // Transform
        const Transform::Combined& getTransform() const;

        const Transform::Instance& getRelative() const;
        void setRelative(const Transform::Instance& inTransform);

        const Transform::Instance& getAbsolute() const;
        void setAbsolute(const Transform::Instance& inTransform);

        // Position
        const Mat<4, float>& getTransformation() const;

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
        void setBounds(const Bounds& inBounds);

        // Events
        Subscription<void*>* watchTransform(
            std::function<void (void*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    private:
        void refresh();
        void refreshBounds();

    private:
        // Transform
        Transform::Combined                m_transform;

        // Bounds
        Bounds                             m_baseBounds;
        Bounds                             m_currentBounds;

        // Events
        std::unique_ptr<Observable<void*>> m_transformObservable;
    };
}