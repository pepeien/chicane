#include "Chicane/Runtime/Scene/Object.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Drift.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    Object::Object()
        : Transformable(),
          m_bCanCollide(false),
          m_bCanTick(false),
          m_bIsTransient(false),
          m_id(""),
          m_attachments({}),
          m_scene(nullptr),
          m_bIsSpatialDirty(true)
    {}

    Object::~Object()
    {
        while (!m_attachments.empty())
        {
            m_attachments.back()->detach();
        }

        Drift::unbind(*this);

        if (m_scene)
        {
            m_scene->removeSpatial(this);
        }
    }

    void Object::onRefresh()
    {
        Transformable::onRefresh();
        markSpatialDirty();
    }

    bool Object::canTick() const
    {
        return m_bCanTick;
    }

    void Object::setCanTick(bool inCanTick)
    {
        m_bCanTick = inCanTick;
    }

    void Object::tick(float inDeltaTime)
    {
        if (!canTick())
        {
            return;
        }

        onTick(inDeltaTime);
    }

    const String& Object::getId() const
    {
        return m_id;
    }

    void Object::setId(const String& inId)
    {
        if (m_scene)
        {
            m_scene->setObjectId(this, inId);

            return;
        }

        m_id = inId;
    }

    String Object::getTypeName() const
    {
        const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(*this));
        if (!type)
        {
            return String::empty();
        }

        const String&     name  = type->getName();
        const std::size_t split = name.lastOf(':');
        if (split == String::npos)
        {
            return name;
        }

        return name.substr(split + 1);
    }

    bool Object::isTransient() const
    {
        return m_bIsTransient;
    }

    void Object::setIsTransient(bool inValue)
    {
        m_bIsTransient = inValue;
    }

    const std::vector<Component*>& Object::getAttachments() const
    {
        return m_attachments;
    }

    void Object::notifyPropertyEdited(const String& inName)
    {
        onPropertyEdited(inName);
    }

    void Object::addAttachment(Component* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        if (std::find(m_attachments.begin(), m_attachments.end(), inComponent) != m_attachments.end())
        {
            return;
        }

        m_attachments.push_back(inComponent);
    }

    void Object::removeAttachment(Component* inComponent)
    {
        auto found = std::find(m_attachments.begin(), m_attachments.end(), inComponent);
        if (found == m_attachments.end())
        {
            return;
        }

        m_attachments.erase(found);
    }

    void Object::setScene(Scene* inScene)
    {
        if (m_scene == inScene)
        {
            return;
        }

        if (m_scene)
        {
            m_scene->removeSpatial(this);
        }

        m_scene = inScene;
        markSpatialDirty();
    }

    void Object::markSpatialDirty()
    {
        m_bIsSpatialDirty.store(true, std::memory_order_relaxed);
    }

    bool Object::consumeSpatialDirty()
    {
        return m_bIsSpatialDirty.exchange(false, std::memory_order_acq_rel);
    }
}