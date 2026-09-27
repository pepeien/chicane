#include "Shared.hpp"

#include <typeindex>
#include <typeinfo>

#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    namespace Types
    {
        static String stripQualifiers(const String& inName)
        {
            String name = inName.trim();
            while (name.startsWith("const "))
            {
                name = name.substr(6).trim();
            }

            while (name.endsWith("&"))
            {
                name = name.substr(0, name.size() - 1).trim();
            }

            return name;
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

        static bool isObjectTypeName(const String& inName)
        {
            const String tail = typeTail(inName);

            return tail.equals("Object") || tail.equals("Actor") || tail.equals("Component");
        }

        static bool asObject(const std::any& inValue, Object*& outObject)
        {
            if (const auto* value = std::any_cast<Object*>(&inValue))
            {
                outObject = *value;

                return true;
            }

            if (const auto* value = std::any_cast<Actor*>(&inValue))
            {
                outObject = *value;

                return true;
            }

            if (const auto* value = std::any_cast<Component*>(&inValue))
            {
                outObject = *value;

                return true;
            }

            if (const auto* value = std::any_cast<void*>(&inValue))
            {
                if (!*value)
                {
                    outObject = nullptr;

                    return true;
                }

                if (!Script::Handle::contains(*value))
                {
                    return false;
                }

                outObject = static_cast<Object*>(*value);

                return true;
            }

            return false;
        }

        static bool isObjectElement(const ReflectionFieldIterable& inIterable)
        {
            if (!inIterable.elementIndex.has_value())
            {
                return false;
            }

            const std::type_index element = inIterable.elementIndex.value();

            return element == std::type_index(typeid(Object)) || element == std::type_index(typeid(Actor)) ||
                   element == std::type_index(typeid(Component));
        }

        static bool pushObjectList(
            lua_State* inState, const ReflectionTypeMethodInfo& inMethod, const std::any& inValue
        )
        {
            if (!inMethod.isIterable() || !isObjectElement(inMethod.iterable) || !inMethod.containerResolver)
            {
                return false;
            }

            const ReflectionFieldIterable& iterable = inMethod.iterable;
            if (!iterable.sizeFunction || !iterable.atFunction)
            {
                return false;
            }

            const void* container = inMethod.containerResolver(inValue);
            if (!container)
            {
                return false;
            }

            lua_newtable(inState);

            int index = 1;
            for (std::size_t i = 0; i < iterable.sizeFunction(container); i++)
            {
                Object* element = static_cast<Object*>(const_cast<void*>(iterable.atFunction(container, i)));
                if (!element)
                {
                    continue;
                }

                pushObject(inState, element);
                lua_rawseti(inState, -2, index++);
            }

            return true;
        }

        int pushReflectedValue(lua_State* inState, const ReflectionTypeMethodInfo& inMethod, const std::any& inValue)
        {
            if (!inValue.has_value())
            {
                return 0;
            }

            if (pushObjectList(inState, inMethod, inValue))
            {
                return 1;
            }

            Object* object = nullptr;
            if (asObject(inValue, object))
            {
                if (!object)
                {
                    lua_pushnil(inState);
                }
                else
                {
                    pushObject(inState, object);
                }

                return 1;
            }

            return Script::Types::pushValue(inState, inValue);
        }

        bool readReflectedValue(
            lua_State* inState, int inIndex, const String& inTypeName, std::any& outValue, int& outConsumed
        )
        {
            const String name = stripQualifiers(inTypeName);

            outConsumed = 1;

            if (name.endsWith("*"))
            {
                const String pointee = stripQualifiers(name.substr(0, name.size() - 1));
                if (!isObjectTypeName(pointee))
                {
                    return false;
                }

                if (lua_isnoneornil(inState, inIndex))
                {
                    outValue = static_cast<void*>(nullptr);

                    return true;
                }

                Object*      object = checkObject(inState, inIndex);
                const String tail   = typeTail(pointee);

                if (tail.equals("Actor"))
                {
                    outValue = static_cast<void*>(dynamic_cast<Actor*>(object));

                    return true;
                }

                if (tail.equals("Component"))
                {
                    outValue = static_cast<void*>(dynamic_cast<Component*>(object));

                    return true;
                }

                outValue = static_cast<void*>(object);

                return true;
            }

            const String tail = typeTail(name);

            if (tail.equals("Vec2"))
            {
                outConsumed = Script::Types::isVec2(inState, inIndex) ? 1 : 2;
                outValue    = checkVec2Arg(inState, inIndex);

                return true;
            }

            if (tail.equals("Vec3"))
            {
                outConsumed = Script::Types::isVec3(inState, inIndex) ? 1 : 3;
                outValue    = checkVec3Arg(inState, inIndex);

                return true;
            }

            if (tail.equals("Vec4"))
            {
                outConsumed = Script::Types::isVec4(inState, inIndex) ? 1 : 4;
                outValue    = checkVec4Arg(inState, inIndex);

                return true;
            }

            return Script::Types::readValue(inState, inIndex, inTypeName, outValue);
        }
    }
}
