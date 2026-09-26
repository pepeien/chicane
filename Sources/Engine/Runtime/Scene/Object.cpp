#include "Chicane/Runtime/Scene/Object.reflected.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection/Enum/Enumerator/Info.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Drift.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    static String typeTail(const String& inName)
    {
        const std::size_t split = inName.lastOf(':');
        if (split == String::npos)
        {
            return inName;
        }

        return inName.substr(split + 1);
    }

    static const ReflectionEnumInfo* findEnum(const String& inTypeName)
    {
        ReflectionEnumRegistry& registry = ReflectionEnumRegistry::sInstance();
        if (const ReflectionEnumInfo* found = registry.find(inTypeName))
        {
            return found;
        }

        return registry.find(typeTail(inTypeName));
    }

    static void writeEnumValue(void* inAddress, std::size_t inSize, int inValue)
    {
        if (!inAddress)
        {
            return;
        }

        switch (inSize)
        {
        case 1:
            *static_cast<std::uint8_t*>(inAddress) = static_cast<std::uint8_t>(inValue);
            break;

        case 2:
            *static_cast<std::uint16_t*>(inAddress) = static_cast<std::uint16_t>(inValue);
            break;

        case 4:
            *static_cast<int*>(inAddress) = inValue;
            break;

        default:
            break;
        }
    }

    static bool applyEnum(const ReflectionFieldAccessor& inAccessor, void* inInstance, const String& inValue)
    {
        const ReflectionEnumInfo* info = findEnum(inAccessor.typeName);
        if (!info)
        {
            return false;
        }

        for (const ReflectionEnumeratorInfo& enumerator : info->enumerators)
        {
            if (!enumerator.name.equals(inValue) && !typeTail(enumerator.name).equals(inValue))
            {
                continue;
            }

            writeEnumValue(inAccessor.address(inInstance), inAccessor.size, enumerator.value);

            return true;
        }

        return false;
    }

    static bool isTransformAttribute(const String& inName)
    {
        return inName.equals(
            Object::RELATIVE_TRANSLATION_ATTRIBUTE_NAME,
            Object::RELATIVE_ROTATION_ATTRIBUTE_NAME,
            Object::RELATIVE_SCALE_ATTRIBUTE_NAME,
            Object::ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME,
            Object::ABSOLUTE_ROTATION_ATTRIBUTE_NAME,
            Object::ABSOLUTE_SCALE_ATTRIBUTE_NAME
        );
    }

    Object::Object()
        : Transformable(),
          Serializable(),
          m_bCanTick(false),
          m_bCanCollide(false),
          m_bIsTransient(false),
          m_id(""),
          m_attachments({}),
          m_scene(nullptr),
          m_bIsSpatialDirty(true)
    {
        bindAttributes();
    }

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

    void Object::bindAttributes()
    {
        watchAttribute(
            ID_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setId(inValue);
                }
            }
        );

        watchAttribute(
            RELATIVE_TRANSLATION_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setRelativeTranslation(Xml::parseVec3(inValue, Vec3::sZero()));
                }
            }
        );

        watchAttribute(
            RELATIVE_ROTATION_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setRelativeRotation(Xml::parseVec3(inValue, Vec3::sZero()));
                }
            }
        );

        watchAttribute(
            RELATIVE_SCALE_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setRelativeScale(Xml::parseVec3(inValue, Vec3::sOne()));
                }
            }
        );

        watchAttribute(
            ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setAbsoluteTranslation(Xml::parseVec3(inValue, Vec3::sZero()));
                }
            }
        );

        watchAttribute(
            ABSOLUTE_ROTATION_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setAbsoluteRotation(Xml::parseVec3(inValue, Vec3::sZero()));
                }
            }
        );

        watchAttribute(
            ABSOLUTE_SCALE_ATTRIBUTE_NAME,
            [this](const String& inValue)
            {
                if (!inValue.isEmpty())
                {
                    setAbsoluteScale(Xml::parseVec3(inValue, Vec3::sOne()));
                }
            }
        );
    }

    void Object::onRefresh()
    {
        Transformable::onRefresh();

        markSpatialDirty();
    }

    void Object::onAttributeChange(const String& inName, const String& inValue)
    {
        if (inName.equals(ID_ATTRIBUTE_NAME) || isTransformAttribute(inName) || inValue.isEmpty())
        {
            return;
        }

        applySerializedField(inName, inValue);
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
        const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*this));
        if (!type)
        {
            return String::sEmpty();
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

    bool Object::applySerializedField(const String& inName, const String& inValue)
    {
        const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*this));
        if (!type)
        {
            return false;
        }

        const ReflectionFieldAccessor accessor = type->resolve(inName);
        if (!accessor.isValid() || accessor.bIsIterable || !accessor.address(this))
        {
            return false;
        }

        if (applyEnum(accessor, this, inValue))
        {
            notifyPropertyEdited(inName);

            return true;
        }

        if (accessor.isType<FileSystem::Path>())
        {
            accessor.set<FileSystem::Path>(this, FileSystem::Path(inValue));
        }
        else if (accessor.isType<String>())
        {
            accessor.set<String>(this, inValue);
        }
        else if (accessor.isType<bool>())
        {
            accessor.set<bool>(this, inValue.toBool() || inValue.equals("true", "1"));
        }
        else if (accessor.isType<float>())
        {
            accessor.set<float>(this, std::stof(inValue.toStandard()));
        }
        else if (accessor.isType<Vec3>())
        {
            accessor.set<Vec3>(this, Xml::parseVec3(inValue, Vec3::sZero()));
        }
        else if (accessor.isType<int>())
        {
            accessor.set<int>(this, std::stoi(inValue.toStandard()));
        }
        else
        {
            return false;
        }

        notifyPropertyEdited(inName);

        return true;
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
