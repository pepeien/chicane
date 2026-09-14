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
        bool isTransformAttribute(const String& inName)
        {
            return inName.equals(TRANSLATION_ATTRIBUTE_NAME, ROTATION_ATTRIBUTE_NAME, SCALE_ATTRIBUTE_NAME);
        }

        bool isIdentity(const Object& inObject)
        {
            return inObject.getRelativeTranslation() == Vec3::Zero() &&
                   inObject.getRelativeRotation().getAngles() == Vec3::Zero() &&
                   inObject.getRelativeScale() == Vec3::One();
        }

        String formatVec3(const Vec3& inValue)
        {
            return String::sprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
        }

        Vec3 parseVec3(const String& inValue, const Vec3& inFallback)
        {
            String raw = inValue.trim();
            if (raw.startsWith("["))
            {
                raw = raw.substr(1);
            }

            if (raw.endsWith("]"))
            {
                raw = raw.substr(0, raw.size() - 1);
            }

            const std::vector<String> parts = raw.split(',');
            if (parts.size() < 3)
            {
                return inFallback;
            }

            try
            {
                return Vec3(
                    std::stof(parts.at(0).trim().toStandard()),
                    std::stof(parts.at(1).trim().toStandard()),
                    std::stof(parts.at(2).trim().toStandard())
                );
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        String typeTail(const String& inName)
        {
            const std::size_t split = inName.lastOf(':');
            if (split == String::npos)
            {
                return inName;
            }

            return inName.substr(split + 1);
        }

        const ReflectionEnumInfo* findEnum(const String& inTypeName)
        {
            ReflectionEnumRegistry& registry = ReflectionEnumRegistry::getInstance();
            if (const ReflectionEnumInfo* found = registry.find(inTypeName))
            {
                return found;
            }

            return registry.find(typeTail(inTypeName));
        }

        int readEnumValue(const void* inAddress, std::size_t inSize)
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

        void writeEnumValue(void* inAddress, std::size_t inSize, int inValue)
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

        String enumToString(const ReflectionFieldAccessor& inAccessor, const void* inInstance)
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

        bool applyEnum(const ReflectionFieldAccessor& inAccessor, void* inInstance, const String& inValue)
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

        bool applyField(Object& inObject, const String& inName, const String& inValue)
        {
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(inObject));
            if (!type)
            {
                return false;
            }

            const ReflectionFieldAccessor accessor = type->resolve(inName);
            if (!accessor.isValid() || accessor.bNeedsDeref || accessor.bIsIterable)
            {
                return false;
            }

            if (applyEnum(accessor, &inObject, inValue))
            {
                inObject.notifyPropertyEdited(inName);

                return true;
            }

            if (accessor.isType<FileSystem::Path>())
            {
                accessor.set<FileSystem::Path>(&inObject, FileSystem::Path(inValue));
            }
            else if (accessor.isType<String>())
            {
                accessor.set<String>(&inObject, inValue);
            }
            else if (accessor.isType<bool>())
            {
                accessor.set<bool>(&inObject, inValue.toBool() || inValue.equals("true", "1"));
            }
            else if (accessor.isType<float>())
            {
                accessor.set<float>(&inObject, std::stof(inValue.toStandard()));
            }
            else if (accessor.isType<Vec3>())
            {
                accessor.set<Vec3>(&inObject, parseVec3(inValue, Vec3::Zero()));
            }
            else if (accessor.isType<int>())
            {
                accessor.set<int>(&inObject, std::stoi(inValue.toStandard()));
            }
            else
            {
                return false;
            }

            inObject.notifyPropertyEdited(inName);

            return true;
        }

        void writeFields(pugi::xml_node& outNode, const Object& inObject)
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

        void writeObject(pugi::xml_node& outParent, const Object& inObject)
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

            pugi::xml_node node = outParent.append_child(tag.toChar());
            Xml::addAttribute(node, ID_ATTRIBUTE_NAME, inObject.getId());

            if (!isIdentity(inObject))
            {
                Xml::addAttribute(node, TRANSLATION_ATTRIBUTE_NAME, formatVec3(inObject.getRelativeTranslation()));
                Xml::addAttribute(
                    node,
                    ROTATION_ATTRIBUTE_NAME,
                    formatVec3(inObject.getRelativeRotation().getAngles())
                );
                Xml::addAttribute(node, SCALE_ATTRIBUTE_NAME, formatVec3(inObject.getRelativeScale()));
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

        void applyAttributes(Object& inObject, const pugi::xml_node& inNode)
        {
            const String id = Xml::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
            if (!id.isEmpty())
            {
                inObject.setId(id);
            }

            const pugi::xml_attribute translation = Xml::getAttribute(TRANSLATION_ATTRIBUTE_NAME, inNode);
            if (!translation.empty())
            {
                inObject.setRelativeTranslation(parseVec3(translation.as_string(), Vec3::Zero()));
            }

            const pugi::xml_attribute rotation = Xml::getAttribute(ROTATION_ATTRIBUTE_NAME, inNode);
            if (!rotation.empty())
            {
                inObject.setRelativeRotation(parseVec3(rotation.as_string(), Vec3::Zero()));
            }

            const pugi::xml_attribute scale = Xml::getAttribute(SCALE_ATTRIBUTE_NAME, inNode);
            if (!scale.empty())
            {
                inObject.setRelativeScale(parseVec3(scale.as_string(), Vec3::One()));
            }

            for (pugi::xml_attribute attribute : inNode.attributes())
            {
                const String name = attribute.name();
                if (name.equals(ID_ATTRIBUTE_NAME) || isTransformAttribute(name))
                {
                    continue;
                }

                applyField(inObject, name, attribute.as_string());
            }
        }

        Actor* spawnActor(Scene& inScene, const pugi::xml_node& inNode)
        {
            if (inNode.empty() || inNode.type() != pugi::node_element)
            {
                return nullptr;
            }

            const ReflectionTypeInfo* type = findType(inNode.name());
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

            for (pugi::xml_node child : inNode.children())
            {
                spawnComponent(inScene, child, actor);
            }

            return actor;
        }

        Component* spawnComponent(Scene& inScene, const pugi::xml_node& inNode, Object* inParent)
        {
            if (inNode.empty() || inNode.type() != pugi::node_element)
            {
                return nullptr;
            }

            const ReflectionTypeInfo* type = findType(inNode.name());
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

            for (pugi::xml_node child : inNode.children())
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

            const pugi::xml_document document = Xml::load(inFilepath);
            const pugi::xml_node     root     = document.first_child();
            if (root.empty() || !String(root.name()).equals(TAG_ID))
            {
                throw std::runtime_error("Track root element must be " + String(TAG_ID));
            }

            for (pugi::xml_node child : root.children())
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

            pugi::xml_document document;
            pugi::xml_node     root = document.append_child(TAG_ID);
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
