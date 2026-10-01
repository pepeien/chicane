#include "Chicane/Runtime/Track.hpp"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Rotator.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
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
            return inObject.getRelativeTranslation() == Vec3::sZero() &&
                   inObject.getRelativeRotation().getAngles() == Vec3::sZero() &&
                   inObject.getRelativeScale() == Vec3::sOne();
        }

        static bool isAbsoluteIdentity(const Object& inObject)
        {
            return inObject.getTranslation() == Vec3::sZero() && inObject.getRotation().getAngles() == Vec3::sZero() &&
                   inObject.getScale() == Vec3::sOne();
        }

        static String formatVec3(const Vec3& inValue)
        {
            return String::sSprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
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
            ReflectionEnumRegistry& registry = ReflectionEnumRegistry::sInstance();
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

        static bool isLeafField(const ReflectionFieldAccessor& inAccessor)
        {
            return findEnum(inAccessor.typeName) || inAccessor.isType<FileSystem::Path>() ||
                   inAccessor.isType<String>() || inAccessor.isType<Vec3>() || inAccessor.isType<Rotator>() ||
                   inAccessor.isType<bool>() || inAccessor.isType<float>() || inAccessor.isType<int>();
        }

        static const ReflectionTypeInfo* nestedFieldType(const ReflectionFieldInfo& inField)
        {
            if (!inField.typeIndex.has_value())
            {
                return nullptr;
            }

            return ReflectionTypeRegistry::sInstance().find(inField.typeIndex.value());
        }

        static String fieldValue(const ReflectionFieldAccessor& inAccessor, const Object& inObject)
        {
            if (findEnum(inAccessor.typeName))
            {
                return enumToString(inAccessor, &inObject);
            }

            return inAccessor.toString(&inObject);
        }

        static Scene& defaultsScene()
        {
            static Scene scene;

            return scene;
        }

        static const Object* classDefaultObject(const Object& inObject)
        {
            static std::mutex                                     mutex;
            static std::unordered_map<std::type_index, Object*> cache;

            const std::type_index index(typeid(inObject));
            std::lock_guard<std::mutex> lock(mutex);

            auto found = cache.find(index);
            if (found != cache.end())
            {
                return found->second;
            }

            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(index);
            if (!type)
            {
                return nullptr;
            }

            Object* instance = nullptr;
            try
            {
                instance = type->create<Object>({});
            }
            catch (const std::exception&)
            {
                return nullptr;
            }

            if (!instance)
            {
                return nullptr;
            }

            Actor*     actor     = dynamic_cast<Actor*>(instance);
            Component* component = actor ? nullptr : dynamic_cast<Component*>(instance);
            if (actor)
            {
                defaultsScene().adoptActor(actor);
            }
            else if (component)
            {
                defaultsScene().adoptComponent(component);
            }
            else
            {
                delete instance;

                return nullptr;
            }

            cache[index] = instance;

            return instance;
        }

        static const Object* findChildById(const Object* inParent, const String& inId)
        {
            if (!inParent || inId.isEmpty())
            {
                return nullptr;
            }

            for (Object* attachment : inParent->getAttachments())
            {
                if (attachment && attachment->getId().equals(inId))
                {
                    return attachment;
                }
            }

            return nullptr;
        }

        static bool writeFields(
            XmlNode&                  outNode,
            const Object&             inObject,
            const ReflectionTypeInfo& inRoot,
            const ReflectionTypeInfo& inType,
            const String&             inPrefix,
            const Object*             inBaseline
        )
        {
            bool dirty = false;

            for (const ReflectionFieldInfo& field : inType.fields)
            {
                if (field.isTransient() || field.getNames().empty() || field.bIsPointer || field.bIsIterable)
                {
                    continue;
                }

                const String name = field.getName();
                if (inPrefix.isEmpty() && isTransformAttribute(name))
                {
                    continue;
                }

                const String                  path     = inPrefix.isEmpty() ? name : inPrefix + "." + name;
                const ReflectionFieldAccessor accessor = inRoot.resolve(path);
                if (!accessor.isValid() || accessor.bIsTransient)
                {
                    continue;
                }

                if (!isLeafField(accessor))
                {
                    if (const ReflectionTypeInfo* nested = nestedFieldType(field))
                    {
                        dirty = writeFields(outNode, inObject, inRoot, *nested, path, inBaseline) || dirty;
                    }

                    continue;
                }

                const String value = fieldValue(accessor, inObject);
                if (inBaseline)
                {
                    const ReflectionTypeInfo* baselineType =
                        ReflectionTypeRegistry::sInstance().find(typeid(*inBaseline));
                    if (baselineType)
                    {
                        const ReflectionFieldAccessor baselineAccessor = baselineType->resolve(path);
                        if (baselineAccessor.isValid() && value.equals(fieldValue(baselineAccessor, *inBaseline)))
                        {
                            continue;
                        }
                    }
                }
                else if (value.isEmpty())
                {
                    continue;
                }

                Xml::addAttribute(outNode, path, value);
                dirty = true;
            }

            return dirty;
        }

        static bool writeFields(XmlNode& outNode, const Object& inObject, const Object* inBaseline)
        {
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(inObject));
            if (!type)
            {
                return false;
            }

            return writeFields(outNode, inObject, *type, *type, {}, inBaseline);
        }

        static bool writeTransforms(XmlNode& outNode, const Object& inObject, const Object* inBaseline)
        {
            bool dirty = false;

            const auto writeAxis = [&](const char* inName, const String& inValue, const String& inBaselineValue, bool bIdentity)
            {
                if (inBaseline)
                {
                    if (inValue.equals(inBaselineValue))
                    {
                        return;
                    }
                }
                else if (bIdentity)
                {
                    return;
                }

                Xml::addAttribute(outNode, inName, inValue);
                dirty = true;
            };

            if (inObject.isAttached())
            {
                const bool   identity = isRelativeIdentity(inObject);
                const String translation = formatVec3(inObject.getRelativeTranslation());
                const String rotation    = formatVec3(inObject.getRelativeRotation().getAngles());
                const String scale       = formatVec3(inObject.getRelativeScale());
                const String baselineTranslation =
                    inBaseline ? formatVec3(inBaseline->getRelativeTranslation()) : String::sEmpty();
                const String baselineRotation =
                    inBaseline ? formatVec3(inBaseline->getRelativeRotation().getAngles()) : String::sEmpty();
                const String baselineScale = inBaseline ? formatVec3(inBaseline->getRelativeScale()) : String::sEmpty();

                writeAxis(RELATIVE_TRANSLATION_ATTRIBUTE_NAME, translation, baselineTranslation, identity);
                writeAxis(RELATIVE_ROTATION_ATTRIBUTE_NAME, rotation, baselineRotation, identity);
                writeAxis(RELATIVE_SCALE_ATTRIBUTE_NAME, scale, baselineScale, identity);
            }
            else
            {
                const bool   identity = isAbsoluteIdentity(inObject);
                const String translation = formatVec3(inObject.getTranslation());
                const String rotation    = formatVec3(inObject.getRotation().getAngles());
                const String scale       = formatVec3(inObject.getScale());
                const String baselineTranslation =
                    inBaseline ? formatVec3(inBaseline->getTranslation()) : String::sEmpty();
                const String baselineRotation =
                    inBaseline ? formatVec3(inBaseline->getRotation().getAngles()) : String::sEmpty();
                const String baselineScale = inBaseline ? formatVec3(inBaseline->getScale()) : String::sEmpty();

                writeAxis(ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME, translation, baselineTranslation, identity);
                writeAxis(ABSOLUTE_ROTATION_ATTRIBUTE_NAME, rotation, baselineRotation, identity);
                writeAxis(ABSOLUTE_SCALE_ATTRIBUTE_NAME, scale, baselineScale, identity);
            }

            return dirty;
        }

        static bool writeObject(XmlNode& outParent, const Object& inObject, const Object* inBaseline)
        {
            const ObjectOrigin origin = inObject.getOrigin();
            if (origin == ObjectOrigin::Spawned || origin == ObjectOrigin::Transient)
            {
                return false;
            }

            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(inObject));
            if (type && type->isTransient())
            {
                return false;
            }

            String tag = inObject.getTypeName();
            if (tag.isEmpty())
            {
                tag = dynamic_cast<const Actor*>(&inObject) ? Actor::TAG_ID : Component::TAG_ID;
            }

            XmlNode node = outParent.appendChild(tag.toChar());
            Xml::addAttribute(node, ID_ATTRIBUTE_NAME, inObject.getId());

            bool dirty = writeTransforms(node, inObject, inBaseline);
            dirty      = writeFields(node, inObject, inBaseline) || dirty;

            for (Object* attachment : inObject.getAttachments())
            {
                if (!attachment)
                {
                    continue;
                }

                const Object* childBaseline = nullptr;
                if (attachment->isNative())
                {
                    childBaseline = findChildById(inBaseline, attachment->getId());
                }
                else
                {
                    childBaseline = classDefaultObject(*attachment);
                }

                dirty = writeObject(node, *attachment, childBaseline) || dirty;
            }

            if (inObject.isNative() && !dirty)
            {
                outParent.removeChild(node);

                return false;
            }

            return true;
        }

        static bool writeObject(XmlNode& outParent, const Object& inObject)
        {
            return writeObject(outParent, inObject, classDefaultObject(inObject));
        }

        const ReflectionTypeInfo* findType(const String& inTag)
        {
            if (inTag.isEmpty() || inTag.equals(TAG_ID))
            {
                return nullptr;
            }

            ReflectionTypeRegistry&   registry = ReflectionTypeRegistry::sInstance();
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

        static Object* findNativeMatch(
            Object& inParent, const XmlNode& inNode, const std::unordered_set<Object*>& inConsumed
        )
        {
            const String id = inNode.getAttribute(ID_ATTRIBUTE_NAME);
            if (!id.isEmpty())
            {
                for (Object* attachment : inParent.getAttachments())
                {
                    if (!attachment || !attachment->isNative() || inConsumed.find(attachment) != inConsumed.end())
                    {
                        continue;
                    }

                    if (attachment->getId().equals(id))
                    {
                        return attachment;
                    }
                }
            }

            const ReflectionTypeInfo* type = findType(inNode.getName());
            if (!type || !type->typeIndex.has_value())
            {
                return nullptr;
            }

            Object*     unique = nullptr;
            std::size_t count  = 0;
            for (Object* attachment : inParent.getAttachments())
            {
                if (!attachment || !attachment->isNative() || inConsumed.find(attachment) != inConsumed.end())
                {
                    continue;
                }

                if (std::type_index(typeid(*attachment)) != type->typeIndex.value())
                {
                    continue;
                }

                unique = attachment;
                count++;
            }

            return count == 1 ? unique : nullptr;
        }

        static Object* spawnObject(
            Scene& inScene, const XmlNode& inNode, Object* inParent, std::unordered_set<Object*>& ioConsumed
        )
        {
            if (inNode.empty() || !inNode.isElement())
            {
                return nullptr;
            }

            if (inParent)
            {
                if (Object* native = findNativeMatch(*inParent, inNode, ioConsumed))
                {
                    ioConsumed.insert(native);

                    const String nativeId = native->getId();
                    applyAttributes(*native, inNode);
                    native->setId(nativeId);

                    std::unordered_set<Object*> childConsumed;
                    for (XmlNode child : inNode.getChildren())
                    {
                        spawnObject(inScene, child, native, childConsumed);
                    }

                    return native;
                }
            }

            const ReflectionTypeInfo* type = findType(inNode.getName());
            if (!type)
            {
                return nullptr;
            }

            Object* instance = type->create<Object>({});
            if (!instance)
            {
                return nullptr;
            }

            Actor*     actor     = dynamic_cast<Actor*>(instance);
            Component* component = actor ? nullptr : dynamic_cast<Component*>(instance);
            if (!actor && !component)
            {
                delete instance;

                return nullptr;
            }

            instance->setOrigin(ObjectOrigin::Instance);

            if (actor)
            {
                inScene.adoptActor(actor);
            }
            else
            {
                inScene.adoptComponent(component);
            }

            applyAttributes(*instance, inNode);

            if (inParent)
            {
                instance->attachTo(inParent);
            }

            std::unordered_set<Object*> childConsumed;
            for (XmlNode child : inNode.getChildren())
            {
                spawnObject(inScene, child, instance, childConsumed);
            }

            if (component)
            {
                component->activate();
            }

            return instance;
        }

        Object* spawnObject(Scene& inScene, const XmlNode& inNode, Object* inParent)
        {
            std::unordered_set<Object*> consumed;

            return spawnObject(inScene, inNode, inParent, consumed);
        }

        Actor* spawnActor(Scene& inScene, const XmlNode& inNode)
        {
            return dynamic_cast<Actor*>(spawnObject(inScene, inNode, nullptr));
        }

        Component* spawnComponent(Scene& inScene, const XmlNode& inNode, Object* inParent)
        {
            return dynamic_cast<Component*>(spawnObject(inScene, inNode, inParent));
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
                spawnObject(inScene, child, nullptr);
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
            Xml::addAttribute(root, VERSION_ATTRIBUTE_NAME, String::sSprint("%u", CURRENT_VERSION));

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
