#include "Chicane/Runtime/Scene/Object.reflected.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    Object::Object()
        : Transformable(),
          m_bCanCollide(false),
          m_bCanTick(false),
          m_id(""),
          m_scene(nullptr)
    {}

    Object::~Object()
    {
        if (m_scene)
        {
            m_scene->removeSpatial(this);
        }
    }

    void Object::onRefresh()
    {
        Transformable::onRefresh();

        if (m_scene)
        {
            m_scene->updateSpatial(this);
        }
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

        const std::size_t split = type->name.lastOf(':');
        if (split == String::npos)
        {
            return type->name;
        }

        return type->name.substr(split + 1);
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

        if (m_scene)
        {
            m_scene->updateSpatial(this);
        }
    }
}