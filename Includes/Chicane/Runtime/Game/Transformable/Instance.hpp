#pragma once

#include "Chicane.hpp"
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
        const Transform::Combined& getTransform() const;

        const Vec<3, float>& getTranslation() const;
        const Vec<3, float>& getRotation() const;
        const Vec<3, float>& getScale() const;

        const Vec<3, float>& getForward() const;
        const Vec<3, float>& getRight() const;
        const Vec<3, float>& getUp() const;

        // Relative
        const Transform::Instance& getRelative() const;
        void setRelative(const Transform::Instance& inTransform);

        const Vec<3, float>& getRelativeTranslation() const;
        void addRelativeTranslation(float inValue);
        void addRelativeTranslation(float inX, float inY, float inZ);
        void addRelativeTranslation(const Vec<3, float>& inTranslation);
        void setRelativeTranslation(float inValue);
        void setRelativeTranslation(float inX, float inY, float inZ);
        void setRelativeTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getRelativeRotation() const;
        void addRelativeRotation(float inValue);
        void addRelativeRotation(float inPitch, float inRoll, float inYaw);
        void addRelativeRotation(const Vec<3, float>& inRotation);
        void setRelativeRotation(float inValue);
        void setRelativeRotation(float inPitch, float inRoll, float inYaw);
        void setRelativeRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getRelativeScale() const;
        void addRelativeScale(float inValue);
        void addRelativeScale(float inX, float inY, float inZ);
        void addRelativeScale(const Vec<3, float>& inScale);
        void setRelativeScale(float inValue);
        void setRelativeScale(float inX, float inY, float inZ);
        void setRelativeScale(const Vec<3, float>& inScale);

        // Absolute
        const Transform::Instance& getAbsolute() const;
        void setAbsolute(const Transform::Instance& inTransform);

        const Vec<3, float>& getAbsoluteTranslation() const;
        void addAbsoluteTranslation(float inValue);
        void addAbsoluteTranslation(float inX, float inY, float inZ);
        void addAbsoluteTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(float inValue);
        void setAbsoluteTranslation(float inX, float inY, float inZ);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getAbsoluteRotation() const;
        void addAbsoluteRotation(float inValue);
        void addAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void addAbsoluteRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(float inValue);
        void setAbsoluteRotation(float inPitch, float inRoll, float inYaw);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getAbsoluteScale() const;
        void addAbsoluteScale(float inValue);
        void addAbsoluteScale(float inX, float inY, float inZ);
        void addAbsoluteScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(float inValue);
        void setAbsoluteScale(float inX, float inY, float inZ);
        void setAbsoluteScale(const Vec<3, float>& inScale);

        // Bounds
        const Bounds& getBounds() const;
        void addBounds(const Bounds& inBounds);
        void setBounds(const Bounds& inBounds);

        const Vec<3, float>& getTop() const;
        const Vec<3, float>& getCenter() const;
        const Vec<3, float>& getBottom() const;

        // Events
        Subscription<void*>* watchTransform(
            std::function<void (void*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    private:
        void refresh();

    private:
        // Transform
        Transform::Combined                m_transform;

        // Bounds
        Bounds                             m_bounds;

        // Events
        std::unique_ptr<Observable<void*>> m_transformObservable;
    };
}