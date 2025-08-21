#include "Chicane/Runtime/Scene/Transformable.hpp"

namespace Chicane
{
    Transformable::Transformable()
        : m_transform({}),
        m_bounds()
    {}

    const SpatialTransform& Transformable::getTransform() const
    {
        return m_transform;
    }

    const Vec3& Transformable::getTranslation() const
    {
        return m_transform.getTranslation();
    }

    const Vec3& Transformable::getRotation() const
    {
        return m_transform.getRotation();
    }

    const Vec3& Transformable::getScale() const
    {
        return m_transform.getScale();
    }

    const Vec3& Transformable::getForward() const
    {
        return m_transform.getForward();
    }

    const Vec3& Transformable::getRight() const
    {
        return m_transform.getRight();
    }

    const Vec3& Transformable::getUp() const
    {
        return m_transform.getUp();
    }

    const Transform& Transformable::getRelative() const
    {
        return m_transform.getRelativeTransform();
    }

    void Transformable::setRelative(const Transform& inTransform)
    {
        m_transform.setRelativeTransform(inTransform);

        refresh();
    }

    const Vec3& Transformable::getRelativeTranslation() const
    {
        return m_transform.getRelativeTranslation();
    }

    void Transformable::addRelativeTranslation(float inTranslation)
    {
        addRelativeTranslation(Vec3(inTranslation));
    }

    void Transformable::addRelativeTranslation(float inX, float inY, float inZ)
    {
        addRelativeTranslation(Vec3(inX, inY, inZ));
    }

    void Transformable::addRelativeTranslation(const Vec3& inTranslation)
    {
        m_transform.addRelativeTranslation(inTranslation);

        refresh();
    }

    void Transformable::setRelativeTranslation(float inTranslation)
    {
        setRelativeTranslation(Vec3(inTranslation));
    }

    void Transformable::setRelativeTranslation(float inX, float inY, float inZ)
    {
        setRelativeTranslation(Vec3(inX, inY, inZ));
    }

    void Transformable::setRelativeTranslation(const Vec3& inTranslation)
    {
        m_transform.setRelativeTranslation(inTranslation);

        refresh();
    }

    const Vec3& Transformable::getRelativeRotation() const
    {
        return m_transform.getRelativeRotation();
    }

    void Transformable::addRelativeRotation(float inRotation)
    {
        addRelativeRotation(Vec3(inRotation));
    }

    void Transformable::addRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        addRelativeRotation(Vec3(inPitch, inRoll, inYaw));
    }

    void Transformable::addRelativeRotation(const Vec3& inRotation)
    {
        m_transform.addRelativeRotation(inRotation);

        refresh();
    }

    void Transformable::setRelativeRotation(float inRotation)
    {
        setRelativeRotation(Vec3(inRotation));
    }

    void Transformable::setRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        setRelativeRotation(Vec3(inPitch, inRoll, inYaw));
    }

    void Transformable::setRelativeRotation(const Vec3& inRotation)
    {
        m_transform.setRelativeRotation(inRotation);

        refresh();
    }

    const Vec3& Transformable::getRelativeScale() const
    {
        return m_transform.getRelativeScale();
    }

    void Transformable::addRelativeScale(float inScale)
    {
        addRelativeScale(Vec3(inScale));
    }

    void Transformable::addRelativeScale(float inX, float inY, float inZ)
    {
        addRelativeScale(Vec3(inX, inY, inZ));
    }

    void Transformable::addRelativeScale(const Vec3& inScale)
    {
        m_transform.addRelativeScale(inScale);

        refresh();
    }

    void Transformable::setRelativeScale(float inScale)
    {
        setRelativeScale(Vec3(inScale));
    }

    void Transformable::setRelativeScale(float inX, float inY, float inZ)
    {
        setRelativeScale(Vec3(inX, inY, inZ));
    }

    void Transformable::setRelativeScale(const Vec3& inScale)
    {
        m_transform.setRelativeScale(inScale);

        refresh();
    }

    const Transform& Transformable::getAbsolute() const
    {
        return m_transform.getAbsoluteTransform();
    }

    void Transformable::setAbsolute(const Transform& inTransform)
    {
        m_transform.setAbsoluteTransform(inTransform);

        refresh();
    }

    const Vec3& Transformable::getAbsoluteTranslation() const
    {
        return m_transform.getAbsoluteTranslation();
    }

    void Transformable::addAbsoluteTranslation(float inTranslation)
    {
        addAbsoluteTranslation(Vec3(inTranslation));
    }

    void Transformable::addAbsoluteTranslation(float inX, float inY, float inZ)
    {
        addAbsoluteTranslation(Vec3(inX, inY, inZ));
    }

    void Transformable::addAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_transform.addAbsoluteTranslation(inTranslation);

        refresh();
    }

    void Transformable::setAbsoluteTranslation(float inTranslation)
    {
        setAbsoluteTranslation(Vec3(inTranslation));
    }

    void Transformable::setAbsoluteTranslation(float inX, float inY, float inZ)
    {
        setAbsoluteTranslation(Vec3(inX, inY, inZ));
    }

    void Transformable::setAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_transform.setAbsoluteTranslation(inTranslation);

        refresh();
    }

    const Vec3& Transformable::getAbsoluteRotation() const
    {
        return m_transform.getAbsoluteRotation();
    }

    void Transformable::addAbsoluteRotation(float inRotation)
    {
        addAbsoluteRotation(Vec3(inRotation));
    }

    void Transformable::addAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        addAbsoluteRotation(Vec3(inPitch, inRoll, inYaw));
    }

    void Transformable::addAbsoluteRotation(const Vec3& inRotation)
    {
        m_transform.addAbsoluteRotation(inRotation);

        refresh();
    }

    void Transformable::setAbsoluteRotation(float inRotation)
    {
        setAbsoluteRotation(Vec3(inRotation));
    }

    void Transformable::setAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        setAbsoluteRotation(Vec3(inPitch, inRoll, inYaw));
    }

    void Transformable::setAbsoluteRotation(const Vec3& inRotation)
    {
        m_transform.setAbsoluteRotation(inRotation);

        refresh();
    }

    const Vec3& Transformable::getAbsoluteScale() const
    {
        return m_transform.getAbsoluteScale();
    }

    void Transformable::addAbsoluteScale(float inScale)
    {
        addAbsoluteScale(Vec3(inScale));
    }

    void Transformable::addAbsoluteScale(float inX, float inY, float inZ)
    {
        addAbsoluteScale(Vec3(inX, inY, inZ));
    }

    void Transformable::addAbsoluteScale(const Vec3& inScale)
    {
        m_transform.addAbsoluteScale(inScale);

        refresh();
    }

    void Transformable::setAbsoluteScale(float inScale)
    {
        setAbsoluteScale(Vec3(inScale));
    }

    void Transformable::setAbsoluteScale(float inX, float inY, float inZ)
    {
        setAbsoluteScale(Vec3(inX, inY, inZ));
    }

    void Transformable::setAbsoluteScale(const Vec3& inScale)
    {
        m_transform.setAbsoluteScale(inScale);

        refresh();
    }

    const Bounds& Transformable::getBounds() const
    {
        return m_bounds;
    }

    void Transformable::addBounds(const Bounds& inBounds)
    {
        m_bounds.add(inBounds);
        m_bounds.update(m_transform);
    }

    void Transformable::setBounds(const Bounds& inBounds)
    {
        m_bounds.set(inBounds);
        m_bounds.update(m_transform);
    }

    const Vec3& Transformable::getTop() const
    {
        return m_bounds.getTop();
    }

    const Vec3& Transformable::getCenter() const
    {
        return m_bounds.getCenter();
    }

    const Vec3& Transformable::getBottom() const
    {
        return m_bounds.getBottom();
    }

    void Transformable::refresh()
    {
        m_bounds.update(m_transform);

        emmitChanges();
    }
}