#include "Chicane/Game/Transformable/Instance.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    Transformable::Transformable()
        : m_transform({}),
        m_baseBounds(),
        m_currentBounds(),
        m_transformObservable(std::make_unique<Observable<void*>>())
    {}

    const Vec<3, float>& Transformable::getTranslation() const
    {
        return m_transform.getTranslation();
    }

    const Vec<3, float>& Transformable::getRelativeTranslation() const
    {
        return m_transform.getRelativeTranslation();
    }

    void Transformable::addRelativeTranslation(float inTranslation)
    {
        addRelativeTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::addRelativeTranslation(float inX, float inY, float inZ)
    {
        addRelativeTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.addRelativeTranslation(inTranslation);

        refresh();
    }

    void Transformable::setRelativeTranslation(float inTranslation)
    {
        setRelativeTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::setRelativeTranslation(float inX, float inY, float inZ)
    {
        setRelativeTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.setRelativeTranslation(inTranslation);

        refresh();
    }

    const Vec<3, float>& Transformable::getAbsoluteTranslation() const
    {
        return m_transform.getAbsoluteTranslation();
    }

    void Transformable::addAbsoluteTranslation(float inTranslation)
    {
        addAbsoluteTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::addAbsoluteTranslation(float inX, float inY, float inZ)
    {
        addAbsoluteTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addAbsoluteTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.addAbsoluteTranslation(inTranslation);

        refresh();
    }

    void Transformable::setAbsoluteTranslation(float inTranslation)
    {
        setAbsoluteTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::setAbsoluteTranslation(float inX, float inY, float inZ)
    {
        setAbsoluteTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setAbsoluteTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.setAbsoluteTranslation(inTranslation);

        refresh();
    }

    const Vec<3, float>& Transformable::getRotation() const
    {
        return m_transform.getRotation();
    }

    const Vec<3, float>& Transformable::getRelativeRotation() const
    {
        return m_transform.getRelativeRotation();
    }

    void Transformable::addRelativeRotation(float inRotation)
    {
        addRelativeRotation(Vec<3, float>(inRotation));
    }

    void Transformable::addRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        addRelativeRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::addRelativeRotation(const Vec<3, float>& inRotation)
    {
        m_transform.addRelativeRotation(inRotation);

        refresh();
    }

    void Transformable::setRelativeRotation(float inRotation)
    {
        setRelativeRotation(Vec<3, float>(inRotation));
    }

    void Transformable::setRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        setRelativeRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::setRelativeRotation(const Vec<3, float>& inRotation)
    {
        m_transform.setRelativeRotation(inRotation);

        refresh();
    }

    const Vec<3, float>& Transformable::getAbsoluteRotation() const
    {
        return m_transform.getAbsoluteRotation();
    }

    void Transformable::addAbsoluteRotation(float inRotation)
    {
        addAbsoluteRotation(Vec<3, float>(inRotation));
    }

    void Transformable::addAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        addAbsoluteRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::addAbsoluteRotation(const Vec<3, float>& inRotation)
    {
        m_transform.addAbsoluteRotation(inRotation);

        refresh();
    }

    void Transformable::setAbsoluteRotation(float inRotation)
    {
        setAbsoluteRotation(Vec<3, float>(inRotation));
    }

    void Transformable::setAbsoluteRotation(float inPitch, float inRoll, float inYaw)
    {
        setAbsoluteRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::setAbsoluteRotation(const Vec<3, float>& inRotation)
    {
        m_transform.setAbsoluteRotation(inRotation);

        refresh();
    }

    const Vec<3, float>& Transformable::getScale() const
    {
        return m_transform.getScale();
    }

    const Vec<3, float>& Transformable::getRelativeScale() const
    {
        return m_transform.getRelativeScale();
    }

    void Transformable::addRelativeScale(float inScale)
    {
        addRelativeScale(Vec<3, float>(inScale));
    }

    void Transformable::addRelativeScale(float inX, float inY, float inZ)
    {
        addRelativeScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addRelativeScale(const Vec<3, float>& inScale)
    {
        m_transform.addRelativeScale(inScale);

        refresh();
    }

    void Transformable::setRelativeScale(float inScale)
    {
        setRelativeScale(Vec<3, float>(inScale));
    }

    void Transformable::setRelativeScale(float inX, float inY, float inZ)
    {
        setRelativeScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setRelativeScale(const Vec<3, float>& inScale)
    {
        m_transform.setRelativeScale(inScale);

        refresh();
    }

    const Vec<3, float>& Transformable::getAbsoluteScale() const
    {
        return m_transform.getAbsoluteScale();
    }

    void Transformable::addAbsoluteScale(float inScale)
    {
        addAbsoluteScale(Vec<3, float>(inScale));
    }

    void Transformable::addAbsoluteScale(float inX, float inY, float inZ)
    {
        addAbsoluteScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addAbsoluteScale(const Vec<3, float>& inScale)
    {
        m_transform.addAbsoluteScale(inScale);

        refresh();
    }

    void Transformable::setAbsoluteScale(float inScale)
    {
        setAbsoluteScale(Vec<3, float>(inScale));
    }

    void Transformable::setAbsoluteScale(float inX, float inY, float inZ)
    {
        setAbsoluteScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setAbsoluteScale(const Vec<3, float>& inScale)
    {
        m_transform.setAbsoluteScale(inScale);

        refresh();
    }

    const Transform::Combined& Transformable::getTransform() const
    {
        return m_transform;
    }

    const Transform::Instance& Transformable::getRelative() const
    {
        return m_transform.getRelativeTransform();
    }

    void Transformable::setRelative(const Transform::Instance& inTransform)
    {
        m_transform.setRelativeTransform(inTransform);

        refresh();
    }

    const Transform::Instance& Transformable::getAbsolute() const
    {
        return m_transform.getAbsoluteTransform();
    }

    void Transformable::setAbsolute(const Transform::Instance& inTransform)
    {
        m_transform.setAbsoluteTransform(inTransform);

        refresh();
    }

    const Mat<4, float>& Transformable::getTransformation() const
    {
        return m_transform.getTransformation();
    }

    const Quat<float>& Transformable::getOrientation() const
    {
        return m_transform.getOrientation();
    }

    const Vec<3, float>& Transformable::getForward() const
    {
        return m_transform.getForward();
    }

    const Vec<3, float>& Transformable::getRight() const
    {
        return m_transform.getRight();
    }

    const Vec<3, float>& Transformable::getUp() const
    {
        return m_transform.getUp();
    }

    const Vec<3, float>& Transformable::getTop() const
    {
        return m_currentBounds.top;
    }

    const Vec<3, float>& Transformable::getCenter() const
    {
        return m_currentBounds.center;
    }

    const Vec<3, float>& Transformable::getOrigin() const
    {
        return m_currentBounds.origin;
    }

    const Vec<3, float>& Transformable::getExtent() const
    {
        return m_currentBounds.extent;
    }

    const Bounds& Transformable::getBounds() const
    {
        return m_currentBounds;
    }

    void Transformable::setBounds(const Bounds& inBounds)
    {
        m_baseBounds = inBounds;

        refreshBounds();
    }

    Subscription<void*>* Transformable::watchTransform(
        std::function<void (void*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        return m_transformObservable->subscribe(inNext, inError, inComplete);
    }

    void Transformable::refresh()
    {
        refreshBounds();

        m_transformObservable->next(nullptr);
    }

    void Transformable::refreshBounds()
    {
        m_currentBounds.extent = m_baseBounds.extent * getScale();

        Vec<3, float> extent = m_currentBounds.extent * 0.5f;

        m_currentBounds.origin  = m_baseBounds.origin;
        m_currentBounds.origin += getTranslation();

        m_currentBounds.center    = m_currentBounds.origin;
        m_currentBounds.center.z += m_currentBounds.extent.z * 0.5f;

        m_currentBounds.top    = m_currentBounds.origin;
        m_currentBounds.top.z += m_currentBounds.extent.z;
    }
}