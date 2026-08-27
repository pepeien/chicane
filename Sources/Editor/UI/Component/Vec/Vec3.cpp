#include "Editor/UI/Component/Vec/Vec3.reflected.hpp"

#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Transform/Spatial.hpp>
#include <Chicane/Grid.hpp>

namespace Editor
{
    namespace
    {
        Chicane::SpatialTransform* asSpatial(const Chicane::ReflectionFieldAccessor& inAccessor, void* inInstance)
        {
            if (!inInstance)
            {
                return nullptr;
            }

            const Chicane::String name = inAccessor.getName();
            if (!name.equals("translation", "rotation", "scale"))
            {
                return nullptr;
            }

            if (inAccessor.boundInstance != nullptr)
            {
                return reinterpret_cast<Chicane::SpatialTransform*>(const_cast<void*>(inAccessor.boundInstance));
            }

            if (!inAccessor.bNeedsDeref)
            {
                return nullptr;
            }

            void* pointee = *reinterpret_cast<void**>(static_cast<char*>(inInstance) + inAccessor.offset);
            if (!pointee)
            {
                return nullptr;
            }

            return reinterpret_cast<Chicane::SpatialTransform*>(pointee);
        }
    }

    Vec3::Vec3(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          value(Chicane::Vec3::Zero())
    {
        load("Assets/Editor/UI/Components/Vec/Vec3.grid", "Assets/Editor/UI/Components/Vec/Vec3.decal");
    }

    void Vec3::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        if (isEditing())
        {
            return;
        }

        refreshValue();
    }

    void Vec3::commit()
    {
        applyValue();
    }

    bool Vec3::isEditing() const
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->isFocused())
            {
                return true;
            }
        }

        return false;
    }

    bool Vec3::resolveBinding(Chicane::ReflectionFieldAccessor& outAccessor, void*& outInstance)
    {
        const Chicane::String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
        if (raw.isEmpty() || !isReference(raw))
        {
            return false;
        }

        const Chicane::String id =
            raw.getBetween(Chicane::Grid::REFERENCE_VALUE_OPENING, Chicane::Grid::REFERENCE_VALUE_CLOSING).trim();
        if (id.isEmpty())
        {
            return false;
        }

        for (Chicane::Grid::Component* node = this; node != nullptr; node = node->getParent())
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField(id);
            if (accessor.isValid() && (accessor.isType<Chicane::Vec3>() || accessor.isType<Chicane::Rotator>()))
            {
                outAccessor = accessor;
                outInstance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                : static_cast<void*>(node);

                return true;
            }

            if (node->isRoot())
            {
                return false;
            }
        }

        return false;
    }

    void Vec3::refreshValue()
    {
        Chicane::ReflectionFieldAccessor accessor;
        void*                            instance = nullptr;
        if (!resolveBinding(accessor, instance))
        {
            return;
        }

        if (const Chicane::Vec3* parsed = accessor.getValue<Chicane::Vec3>(instance))
        {
            value = *parsed;

            return;
        }

        if (const Chicane::Rotator* parsed = accessor.getValue<Chicane::Rotator>(instance))
        {
            value = parsed->getAngles();
        }
    }

    void Vec3::applyValue()
    {
        Chicane::ReflectionFieldAccessor accessor;
        void*                            instance = nullptr;
        if (!resolveBinding(accessor, instance))
        {
            return;
        }

        if (Chicane::SpatialTransform* spatial = asSpatial(accessor, instance))
        {
            const Chicane::String name = accessor.getName();
            if (name.equals("translation"))
            {
                spatial->setAbsoluteTranslation(value);

                return;
            }

            if (name.equals("rotation"))
            {
                spatial->setAbsoluteRotation(value);

                return;
            }

            if (name.equals("scale"))
            {
                spatial->setAbsoluteScale(value);

                return;
            }
        }

        if (accessor.isType<Chicane::Vec3>())
        {
            accessor.set<Chicane::Vec3>(instance, value);

            return;
        }

        if (Chicane::Rotator* rotator = accessor.getValue<Chicane::Rotator>(instance))
        {
            rotator->set(value);
        }
    }
}
