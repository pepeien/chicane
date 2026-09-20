#include "Chicane/Runtime/Track.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Rotator.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    namespace Track
    {
        static bool isTransformAttribute(const String& inName)
        {
            return inName.equals(
                RELATIVE_TRANSLATION_ATTRIBUTE_NAME,
                RELATIVE_ROTATION_ATTRIBUTE_NAME,
                RELATIVE_SCALE_ATTRIBUTE_NAME,
                ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME,
                ABSOLUTE_ROTATION_ATTRIBUTE_NAME,
                ABSOLUTE_SCALE_ATTRIBUTE_NAME
            );
        }

        static bool isRelativeIdentity(const Object& inObject)
        {
            return inObject.getRelativeTranslation() == Vec3::Zero() &&
                   inObject.getRelativeRotation().getAngles() == Vec3::Zero() &&
                   inObject.getRelativeScale() == Vec3::One();
        }

        static bool isAbsoluteIdentity(const Object& inObject)
        {
            return inObject.getTranslation() == Vec3::Zero() && inObject.getRotation().getAngles() == Vec3::Zero() &&
                   inObject.getScale() == Vec3::One();
        }

        static String formatVec3(const Vec3& inValue)
        {
            return String::sprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
        }

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
            ReflectionEnumRegistry& registry = ReflectionEnumRegistry::getInstance();
            if (const ReflectionEnumInfo* found = registry.find(inTypeName))
            {
                return found;
            }

            return registry.find(typeTail(inTypeName));
        }

        static int readEnumValue(const void* inAddress, std::size_t inSize)
        {
            if (!inAddress)
            {
                return 0;
            }

            switch (inSize)
            {
            case 1:
                return static_cast<int>(*static_cast<const std::uint8_t*>(inAddress));

            case 2:
                return static_cast<int>(*static_cast<const std::uint16_t*>(inAddress));

            case 4:
                return *static_cast<const int*>(inAddress);

            default:
                return 0;
            }
        }

        static String enumToString(const ReflectionFieldAccessor& inAccessor, const void* inInstance)
        {
            const ReflectionEnumInfo* info = findEnum(inAccessor.typeName);
            if (!info)
            {
                return inAccessor.toString(inInstance);
            }

            const int value = readEnumValue(inAccessor.address(inInstance), inAccessor.size);
            for (const ReflectionEnumeratorInfo& enumerator : info->enumerators)
            {
                if (enumerator.value == value)
                {
                    return enumerator.name;
                }
            }

            return inAccessor.toString(inInstance);
        }

        bool applyField(Object& inObject, const String& inName, const String& inValue)
        {
            return inObject.applySerializedField(inName, inValue);
        }

        static void writeFields(XmlNode& outNode, const Object& inObject)
        {
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(inObject));
            if (!type)
            {
                return;
            }

            for (const ReflectionFieldInfo& field : type->fields)
            {
                if (field.names.empty() || field.bIsPointer || field.bIsIterable)
                {
                    continue;
                }

                const String name = field.names.front();
                if (isTransformAttribute(name))
                {
                    continue;
                }

                const ReflectionFieldAccessor accessor = type->resolve(name);
                if (!accessor.isValid())
                {
                    continue;
                }

                String value;
                if (findEnum(accessor.typeName))
                {
                    value = enumToString(accessor, &inObject);
                }
                else if (accessor.isType<FileSystem::Path>())
                {
                    const FileSystem::Path* path = accessor.getValue<FileSystem::Path>(&inObject);
                    value                        = path ? path->toString() : String::empty();
                }
                else if (accessor.isType<Vec3>())
                {
                    const Vec3* vector = accessor.getValue<Vec3>(&inObject);
                    value              = vector ? formatVec3(*vector) : String::empty();
                }
                else
                {
                    value = accessor.toString(&inObject);
                }

                if (value.isEmpty())
                {
                    continue;
                }

                Xml::addAttribute(outNode, name, value);
            }
        }

        static void writeObject(XmlNode& outParent, const Object& inObject)
        {
            if (inObject.isTransient())
            {
                return;
            }

            String tag = inObject.getTypeName();
            if (tag.isEmpty())
            {
                tag = dynamic_cast<const Actor*>(&inObject) ? Actor::TAG_ID : Component::TAG_ID;
            }

            XmlNode node = outParent.appendChild(tag.toChar());
            Xml::addAttribute(node, ID_ATTRIBUTE_NAME, inObject.getId());

            const Component* component = dynamic_cast<const Component*>(&inObject);
            const bool       bAttached = component && component->getParent();

            if (bAttached)
            {
                if (!isRelativeIdentity(inObject))
                {
                    Xml::addAttribute(
                        node,
                        RELATIVE_TRANSLATION_ATTRIBUTE_NAME,
                        formatVec3(inObject.getRelativeTranslation())
                    );
                    Xml::addAttribute(
                        node,
                        RELATIVE_ROTATION_ATTRIBUTE_NAME,
                        formatVec3(inObject.getRelativeRotation().getAngles())
                    );
                    Xml::addAttribute(node, RELATIVE_SCALE_ATTRIBUTE_NAME, formatVec3(inObject.getRelativeScale()));
                }
            }
            else if (!isAbsoluteIdentity(inObject))
            {
                Xml::addAttribute(node, ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME, formatVec3(inObject.getTranslation()));
                Xml::addAttribute(
                    node,
                    ABSOLUTE_ROTATION_ATTRIBUTE_NAME,
                    formatVec3(inObject.getRotation().getAngles())
                );
                Xml::addAttribute(node, ABSOLUTE_SCALE_ATTRIBUTE_NAME, formatVec3(inObject.getScale()));
            }

            writeFields(node, inObject);

            for (Component* attachment : inObject.getAttachments())
            {
                if (!attachment)
                {
                    continue;
                }

                writeObject(node, *attachment);
            }
        }

        const ReflectionTypeInfo* findType(const String& inTag)
        {
            if (inTag.isEmpty() || inTag.equals(TAG_ID))
            {
                return nullptr;
            }

            ReflectionTypeRegistry&   registry = ReflectionTypeRegistry::getInstance();
            const ReflectionTypeInfo* type     = registry.find(String("Chicane::") + inTag);
            if (!type)
            {
                type = registry.find(inTag);
            }

            if (!type && !inTag.isEmpty())
            {
                const String pascal = inTag.substr(0, 1).toUpper() + inTag.substr(1);
                if (!pascal.equals(inTag))
                {
                    type = registry.find(String("Chicane::") + pascal);
                }
            }

            return type;
        }

        void applyAttributes(Object& inObject, const XmlNode& inNode)
        {
            inObject.parse(inNode);
        }

        Actor* spawnActor(Scene& inScene, const XmlNode& inNode)
        {
            if (inNode.empty() || !inNode.isElement())
            {
                return nullptr;
            }

            const ReflectionTypeInfo* type = findType(inNode.getName());
            if (!type)
            {
                return nullptr;
            }

            Object* instance = type->create<Object>({});
            Actor*  actor    = dynamic_cast<Actor*>(instance);
            if (!actor)
            {
                delete instance;

                return nullptr;
            }

            applyAttributes(*actor, inNode);
            inScene.adoptActor(actor);

            for (XmlNode child : inNode.getChildren())
            {
                spawnComponent(inScene, child, actor);
            }

            return actor;
        }

        Component* spawnComponent(Scene& inScene, const XmlNode& inNode, Object* inParent)
        {
            if (inNode.empty() || !inNode.isElement())
            {
                return nullptr;
            }

            const ReflectionTypeInfo* type = findType(inNode.getName());
            if (!type)
            {
                return nullptr;
            }

            Object*    instance  = type->create<Object>({});
            Component* component = dynamic_cast<Component*>(instance);
            if (!component)
            {
                delete instance;

                return nullptr;
            }

            applyAttributes(*component, inNode);
            inScene.adoptComponent(component);

            if (inParent)
            {
                component->attachTo(inParent);
            }

            component->activate();

            for (XmlNode child : inNode.getChildren())
            {
                spawnComponent(inScene, child, component);
            }

            return component;
        }

        void open(Scene& inScene, const FileSystem::Path& inFilepath)
        {
            inScene.clearSerializable();
            inScene.setFilepath(inFilepath);

            if (inFilepath.isEmpty() || !FileSystem::exists(inFilepath))
            {
                return;
            }

            const XmlDocument document = Xml::load(inFilepath);
            const XmlNode     root     = document.getFirstChild();
            if (root.empty() || !String(root.getName()).equals(TAG_ID))
            {
                throw std::runtime_error("Track root element must be " + String(TAG_ID));
            }

            for (XmlNode child : root.getChildren())
            {
                spawnActor(inScene, child);
            }
        }

        void save(const Scene& inScene, const FileSystem::Path& inFilepath)
        {
            if (inFilepath.isEmpty())
            {
                throw std::runtime_error("The track path is empty");
            }

            XmlDocument document;
            XmlNode     root = document.appendChild(TAG_ID);
            Xml::addAttribute(root, VERSION_ATTRIBUTE_NAME, String::sprint("%u", CURRENT_VERSION));

            const String id = inFilepath.stem().toString();
            if (!id.isEmpty())
            {
                Xml::addAttribute(root, ID_ATTRIBUTE_NAME, id);
            }

            for (Actor* actor : inScene.getActors())
            {
                if (!actor)
                {
                    continue;
                }

                writeObject(root, *actor);
            }

            Xml::save(document, inFilepath);
        }
    }
}
