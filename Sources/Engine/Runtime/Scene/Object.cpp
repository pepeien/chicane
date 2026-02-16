#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    Object::Object()
        : Transformable(),
          m_bCanCollide(false),
          m_bCanTick(false),
          m_id("")
    {}

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
        m_id = inId;
    }

    void Object::setScene(Scene* inScene)
    {
        m_scene = inScene;

        if (m_scene)
        {
            onLoad();
        }
        else
        {
            onUnload();
        }
    }
}