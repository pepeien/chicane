#pragma once

namespace Chicane
{
    template <typename T>
    T* Object::createDefaultComponent(const String& inId)
    {
        for (Object* attachment : m_attachments)
        {
            if (!attachment || !attachment->isNative() || !attachment->getId().equals(inId))
            {
                continue;
            }

            if (T* typed = dynamic_cast<T*>(attachment))
            {
                return typed;
            }
        }

        Scene* scene = getScene();
        if (!scene)
        {
            return nullptr;
        }

        T* component = scene->createComponent<T>();
        if (!component)
        {
            return nullptr;
        }

        component->setOrigin(ObjectOrigin::Native);
        component->setId(inId);
        component->attachTo(this);

        return component;
    }
}
