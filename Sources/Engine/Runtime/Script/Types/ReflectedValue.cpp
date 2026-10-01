#include "ReflectedValue.hpp"

#include <exception>
#include <new>
#include <typeindex>
#include <unordered_set>

#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

#include "Value.hpp"

namespace Chicane
{
    namespace Types
    {
        static int reflectedValueIndex(lua_State* inState);
        static int reflectedValueNewIndex(lua_State* inState);
        static int collectValue(lua_State* inState);
        static int callValueMethod(lua_State* inState);
        static int callStaticMethod(lua_State* inState);

        std::size_t reflectedValueDataOffset()
        {
            constexpr std::size_t header = sizeof(ReflectedValueBox);
            constexpr std::size_t align  = alignof(std::max_align_t);

            return (header + align - 1) / align * align;
        }

        void* reflectedValueData(ReflectedValueBox* inBox)
        {
            return reinterpret_cast<char*>(inBox) + reflectedValueDataOffset();
        }

        static void* sOwnerSentinel()
        {
            static int sentinel = 0;

            return &sentinel;
        }

        static Object* objectFrom(ReflectedValueBox* inBox)
        {
            if (!inBox || inBox->destroy)
            {
                return nullptr;
            }

            Object* object = *static_cast<Object**>(reflectedValueData(inBox));
            if (!object || !Script::Handle::contains(object))
            {
                return nullptr;
            }

            return object;
        }

        static void ensurePointerMetatable(lua_State* inState, const char* inName)
        {
            if (luaL_getmetatable(inState, inName) != LUA_TNIL)
            {
                lua_pop(inState, 1);

                return;
            }

            lua_pop(inState, 1);

            luaL_newmetatable(inState, inName);
            lua_pushcfunction(inState, reflectedIndex);
            lua_setfield(inState, -2, "__index");
            lua_pushcfunction(inState, reflectedNewIndex);
            lua_setfield(inState, -2, "__newindex");
            lua_pushlightuserdata(inState, sOwnerSentinel());
            lua_setfield(inState, -2, OWNER_FIELD);
            lua_pushcfunction(inState, collectValue);
            lua_setfield(inState, -2, "__gc");
            lua_pop(inState, 1);
        }

        static String shortNameOf(const String& inName)
        {
            const std::size_t split = inName.lastOf(':');
            if (split == String::npos)
            {
                return inName;
            }

            return inName.substr(split + 1);
        }

        static int pushValueMethod(lua_State* inState, int inInstanceIndex, const ReflectionTypeMethodInfo* inMethod)
        {
            lua_pushvalue(inState, inInstanceIndex);
            lua_pushlightuserdata(inState, const_cast<ReflectionTypeMethodInfo*>(inMethod));
            lua_pushcclosure(inState, callValueMethod, 2);

            return 1;
        }

        static int callValueMethod(lua_State* inState)
        {
            const ReflectedValueBox* box =
                static_cast<const ReflectedValueBox*>(lua_touserdata(inState, lua_upvalueindex(1)));
            const auto* method =
                static_cast<const ReflectionTypeMethodInfo*>(lua_touserdata(inState, lua_upvalueindex(2)));
            if (!box || !box->type || !method)
            {
                return luaL_error(inState, "reflected method is no longer valid");
            }

            int cursor = 1;
            if (lua_gettop(inState) >= 1 && lua_type(inState, 1) == LUA_TUSERDATA)
            {
                cursor = 2;
            }

            ReflectionTypeMethodInfo::Params params;
            params.reserve(method->paramTypes.size());
            for (std::size_t i = 0; i < method->paramTypes.size(); i++)
            {
                std::any value;
                int      consumed = 1;
                if (!readReflectedValue(inState, cursor, method->paramTypes[i], value, consumed))
                {
                    return luaL_error(
                        inState,
                        "invalid argument %d for %s",
                        static_cast<int>(i) + 1,
                        method->getName().toChar()
                    );
                }

                cursor += consumed;
                params.push_back(std::move(value));
            }

            try
            {
                return pushReflectedValue(
                    inState,
                    *method,
                    method->invoke(reflectedValueData(const_cast<ReflectedValueBox*>(box)), params)
                );
            }
            catch (const std::exception& error)
            {
                return luaL_error(inState, "%s failed: %s", method->getName().toChar(), error.what());
            }
        }

        static int callStaticMethod(lua_State* inState)
        {
            const auto* method =
                static_cast<const ReflectionTypeMethodInfo*>(lua_touserdata(inState, lua_upvalueindex(1)));
            if (!method)
            {
                return luaL_error(inState, "reflected method is no longer valid");
            }

            int                              cursor = 1;
            ReflectionTypeMethodInfo::Params params;
            params.reserve(method->paramTypes.size());
            for (std::size_t i = 0; i < method->paramTypes.size(); i++)
            {
                std::any value;
                int      consumed = 1;
                if (!readReflectedValue(inState, cursor, method->paramTypes[i], value, consumed))
                {
                    return luaL_error(
                        inState,
                        "invalid argument %d for %s",
                        static_cast<int>(i) + 1,
                        method->getName().toChar()
                    );
                }

                cursor += consumed;
                params.push_back(std::move(value));
            }

            try
            {
                return pushReflectedValue(inState, *method, method->invoke(nullptr, params));
            }
            catch (const std::exception& error)
            {
                return luaL_error(inState, "%s failed: %s", method->getName().toChar(), error.what());
            }
        }

        static void bindStaticMethods(lua_State* inState, const ReflectionTypeInfo* inType)
        {
            if (!inType)
            {
                return;
            }

            bool bHasStatics = false;
            for (const ReflectionTypeMethodInfo& method : inType->methods)
            {
                if (!method.bIsStatic)
                {
                    continue;
                }

                lua_pushlightuserdata(inState, const_cast<ReflectionTypeMethodInfo*>(&method));
                lua_pushcclosure(inState, callStaticMethod, 1);
                lua_setfield(inState, -2, method.getName().toChar());
                bHasStatics = true;
            }

            if (!bHasStatics || inType->getNames().empty())
            {
                return;
            }

            const String global = shortNameOf(inType->getNames().back());
            lua_pushvalue(inState, -1);
            lua_setglobal(inState, global.toChar());
        }

        void ensureValueMetatable(lua_State* inState, const char* inName, const ReflectionTypeInfo* inType)
        {
            if (luaL_getmetatable(inState, inName) != LUA_TNIL)
            {
                lua_pop(inState, 1);

                return;
            }

            lua_pop(inState, 1);

            luaL_newmetatable(inState, inName);
            lua_pushcfunction(inState, reflectedValueIndex);
            lua_setfield(inState, -2, "__index");
            lua_pushcfunction(inState, reflectedValueNewIndex);
            lua_setfield(inState, -2, "__newindex");
            lua_pushcfunction(inState, collectValue);
            lua_setfield(inState, -2, "__gc");
            bindStaticMethods(inState, inType);
            lua_pop(inState, 1);
        }

        ReflectedValueBox* testValueBox(lua_State* inState, int inIndex, const char* inName)
        {
            return static_cast<ReflectedValueBox*>(luaL_testudata(inState, inIndex, inName));
        }

        void bindReflectedStatics(lua_State* inState)
        {
            ReflectionTypeRegistry&             registry = ReflectionTypeRegistry::sInstance();
            std::unordered_set<std::type_index> seen;
            for (const auto& [name, copy] : registry.getAll())
            {
                if (!copy.typeIndex.has_value() || !seen.insert(copy.typeIndex.value()).second)
                {
                    continue;
                }

                const ReflectionTypeInfo* type = registry.find(copy.typeIndex.value());
                if (!type || type->getNames().empty())
                {
                    continue;
                }

                bool bIsObjectType = false;
                for (const String& typeName : type->getNames())
                {
                    const String tail = shortNameOf(typeName);
                    if (tail.equals("Object") || tail.equals("Actor") || tail.equals("Component"))
                    {
                        bIsObjectType = true;

                        break;
                    }
                }

                if (bIsObjectType)
                {
                    continue;
                }

                bool bHasStatics = false;
                for (const ReflectionTypeMethodInfo& method : type->methods)
                {
                    if (method.bIsStatic)
                    {
                        bHasStatics = true;

                        break;
                    }
                }

                if (!bHasStatics)
                {
                    continue;
                }

                ensureValueMetatable(inState, type->getNames().front().toChar(), type);
            }
        }

        Object* liveObject(lua_State* inState, int inIndex)
        {
            if (lua_type(inState, inIndex) != LUA_TUSERDATA || !lua_getmetatable(inState, inIndex))
            {
                return nullptr;
            }

            lua_pushstring(inState, OWNER_FIELD);
            lua_rawget(inState, -2);
            const bool bIsOwned = lua_touserdata(inState, -1) == sOwnerSentinel();
            lua_pop(inState, 2);
            if (!bIsOwned)
            {
                return nullptr;
            }

            return objectFrom(static_cast<ReflectedValueBox*>(lua_touserdata(inState, inIndex)));
        }

        void pushObject(lua_State* inState, Object* inObject)
        {
            if (!inObject)
            {
                lua_pushnil(inState);

                return;
            }

            const ReflectionTypeInfo* type      = ReflectionTypeRegistry::sInstance().find(typeid(*inObject));
            const char*               metatable = valueMetatableOf(type);
            if (!metatable)
            {
                metatable = OBJECT_MT;
            }

            ensurePointerMetatable(inState, metatable);

            void*              memory = lua_newuserdatauv(inState, reflectedValueDataOffset() + sizeof(Object*), 0);
            ReflectedValueBox* box    = static_cast<ReflectedValueBox*>(memory);
            box->type                 = type;
            box->destroy              = nullptr;
            *static_cast<Object**>(reflectedValueData(box)) = inObject;
            luaL_setmetatable(inState, metatable);
        }

        bool isObject(lua_State* inState, int inIndex)
        {
            return liveObject(inState, inIndex) != nullptr;
        }

        Object* checkObject(lua_State* inState, int inIndex)
        {
            Object* object = liveObject(inState, inIndex);
            if (!object)
            {
                luaL_error(
                    inState,
                    lua_type(inState, inIndex) == LUA_TUSERDATA ? "object is no longer valid"
                                                                : "expected a scene object"
                );

                return nullptr;
            }

            return object;
        }

        void pushActor(lua_State* inState, Actor* inActor)
        {
            pushObject(inState, inActor);
        }

        bool isActor(lua_State* inState, int inIndex)
        {
            return dynamic_cast<Actor*>(liveObject(inState, inIndex)) != nullptr;
        }

        Actor* checkActor(lua_State* inState, int inIndex)
        {
            Actor* actor = dynamic_cast<Actor*>(checkObject(inState, inIndex));
            if (!actor)
            {
                luaL_error(inState, "expected Actor");

                return nullptr;
            }

            return actor;
        }

        void pushComponent(lua_State* inState, Component* inComponent)
        {
            pushObject(inState, inComponent);
        }

        bool isComponent(lua_State* inState, int inIndex)
        {
            return dynamic_cast<Component*>(liveObject(inState, inIndex)) != nullptr;
        }

        Component* checkComponent(lua_State* inState, int inIndex)
        {
            Component* component = dynamic_cast<Component*>(checkObject(inState, inIndex));
            if (!component)
            {
                luaL_error(inState, "expected Component");

                return nullptr;
            }

            return component;
        }

        static int collectValue(lua_State* inState)
        {
            if (lua_type(inState, 1) != LUA_TUSERDATA)
            {
                return 0;
            }

            ReflectedValueBox* box = static_cast<ReflectedValueBox*>(lua_touserdata(inState, 1));
            if (!box)
            {
                return 0;
            }

            if (box->destroy)
            {
                box->destroy(reflectedValueData(box));
                box->destroy = nullptr;
            }
            else
            {
                *static_cast<Object**>(reflectedValueData(box)) = nullptr;
            }

            box->type = nullptr;

            return 0;
        }

        static int reflectedValueIndex(lua_State* inState)
        {
            if (lua_type(inState, 1) != LUA_TUSERDATA || lua_type(inState, 2) != LUA_TSTRING)
            {
                lua_pushnil(inState);

                return 1;
            }

            ReflectedValueBox* box = static_cast<ReflectedValueBox*>(lua_touserdata(inState, 1));
            if (!box || !box->type)
            {
                lua_pushnil(inState);

                return 1;
            }

            void*       instance = reflectedValueData(box);
            const char* key      = lua_tostring(inState, 2);

            if (const ReflectionTypeMethodInfo* method = box->type->findMethod(key))
            {
                if (!method->bIsStatic)
                {
                    return pushValueMethod(inState, 1, method);
                }
            }

            const ReflectionFieldAccessor accessor = box->type->resolve(key);
            if (Script::Types::pushField(inState, accessor, instance))
            {
                return 1;
            }

            if (accessor.isValid() && accessor.size == sizeof(void*) && accessor.address(instance))
            {
                void* pointee = *reinterpret_cast<void* const*>(accessor.address(instance));
                if (!pointee || !Script::Handle::contains(pointee))
                {
                    lua_pushnil(inState);

                    return 1;
                }

                pushObject(inState, static_cast<Object*>(pointee));

                return 1;
            }

            lua_pushnil(inState);

            return 1;
        }

        static int reflectedValueNewIndex(lua_State* inState)
        {
            if (lua_type(inState, 1) != LUA_TUSERDATA || lua_type(inState, 2) != LUA_TSTRING)
            {
                return luaL_error(inState, "cannot set property");
            }

            ReflectedValueBox* box = static_cast<ReflectedValueBox*>(lua_touserdata(inState, 1));
            if (!box || !box->type)
            {
                return luaL_error(inState, "type is not reflected");
            }

            void*                         instance = reflectedValueData(box);
            const char*                   key      = lua_tostring(inState, 2);
            const ReflectionFieldAccessor accessor = box->type->resolve(key);
            if (Script::Types::setField(inState, accessor, instance, 3))
            {
                return 0;
            }

            if (accessor.isValid() && accessor.size == sizeof(void*) && accessor.address(instance))
            {
                Object* value = nullptr;
                if (!lua_isnoneornil(inState, 3))
                {
                    value = checkObject(inState, 3);
                }

                *reinterpret_cast<void**>(accessor.address(instance)) = value;

                return 0;
            }

            return luaL_error(inState, "unknown property '%s'", key);
        }
    }
}
