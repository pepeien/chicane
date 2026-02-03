#include "Chicane/Core/Transformable.hpp"

namespace Chicane
{
    Transformable::Transformable()
        : Changeable(),
          m_transform({}),
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

    const Rotator& Transformable::getRotation() const
    {
        return m_transform.getRotation();
    }

    const Vec3& Transformable::getScale() const
    {
        return m_transform.getScale();
    }

    const Vec3& Transformable::getForward() const
    {
        return m_transform.getRotation().getForward();
    }

    const Vec3& Transformable::getRight() const
    {
        return m_transform.getRotation().getRight();
    }

    const Vec3& Transformable::getUp() const
    {
        return m_transform.getRotation().getUp();
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

    const Rotator& Transformable::getRelativeRotation() const
    {
        return m_transform.getRelativeRotation();
    }

    void Transformable::addRelativeRotation(float inRotation)
    {
        addRelativeRotation(Rotator(inRotation));
    }

    void Transformable::addRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        addRelativeRotation(Rotator(inPitch, inRoll, inYaw));
    }

    void Transformable::addRelativeRotation(const Vec3& inAngles)
    {
        addRelativeRotation(Rotator(inAngles));
    }

    void Transformable::addRelativeRotation(const Rotator& inRotation)
    {
        m_transform.addRelativeRotation(inRotation);

        refresh();
    }

    void Transformable::setRelativeRotation(float inRotation)
    {
        setRelativeRotation(Rotator(inRotation));
    }

    void Transformable::setRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        setRelativeRotation(Rotator(inPitch, inRoll, inYaw));
    }

    void Transformable::setRelativeRotation(const Vec3& inAngles)
    {
        setRelativeRotation(Rotator(inAngles));
    }

    void Transformable::setRelativeRotation(const Rotator& inRotation)
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

    const Rotator& Transformable::getAbsoluteRotation() const
    {
        return m_transform.getAbsoluteRotation();
    }

    void Transformable::addAbsoluteRotation(float inRotation)
    {
        addAbsoluteRotation(Rotator(inRotation));
    }

    void Transformable::addAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        addAbsoluteRotation(Rotator(inPitch, inRoll, inYaw));
    }

    void Transformable::addAbsoluteRotation(const Vec3& inAngles)
    {
        addAbsoluteRotation(Rotator(inAngles));
    }

    void Transformable::addAbsoluteRotation(const Rotator& inRotation)
    {
        m_transform.addAbsoluteRotation(inRotation);

        refresh();
    }

    void Transformable::setAbsoluteRotation(float inAngle)
    {
        setAbsoluteRotation(Rotator(inAngle));
    }

    void Transformable::setAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        setAbsoluteRotation(Rotator(inPitch, inRoll, inYaw));
    }

    void Transformable::setAbsoluteRotation(const Vec3& inAngles)
    {
        setAbsoluteRotation(Rotator(inAngles));
    }

    void Transformable::setAbsoluteRotation(const Rotator& inRotation)
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

    const Bounds3D& Transformable::getBounds() const
    {
        return m_bounds;
    }

    void Transformable::addBounds(const Bounds3D& inBounds)
    {
        m_bounds.add(inBounds);
        m_bounds.update(m_transform);
    }

    void Transformable::setBounds(const Bounds3D& inBounds)
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

        onTransform();

        emmitChanges();
    }
}