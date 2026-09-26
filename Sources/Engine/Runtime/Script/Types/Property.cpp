#include "Shared.hpp"

#include <typeinfo>

#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Types.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"
#include "Chicane/Runtime/Script/Types.hpp"

namespace Chicane
{
    namespace Types
    {
        struct MethodBox
        {
            void*                           instance;
            const ReflectionTypeMethodInfo* method;
        };

        static Actor* actorOf(lua_State* inState, int inIndex)
        {
            if (!isActor(inState, inIndex))
            {
                return nullptr;
            }

            return checkActor(inState, inIndex);
        }

        static Component* componentOf(lua_State* inState, int inIndex)
        {
            if (!isComponent(inState, inIndex))
            {
                return nullptr;
            }

            return checkComponent(inState, inIndex);
        }

        static Object* objectOf(lua_State* inState, int inIndex)
        {
            if (Actor* actor = actorOf(inState, inIndex))
            {
                return actor;
            }

            return componentOf(inState, inIndex);
        }

        static int lookupMethod(lua_State* inState)
        {
            if (!lua_getmetatable(inState, 1))
            {
                return 0;
            }

            while (true)
            {
                lua_pushvalue(inState, 2);
                lua_rawget(inState, -2);
                if (!lua_isnil(inState, -1))
                {
                    lua_remove(inState, -2);

                    return 1;
                }

                lua_pop(inState, 1);
                if (!lua_getmetatable(inState, -1))
                {
                    lua_pop(inState, 1);

                    return 0;
                }

                lua_remove(inState, -2);
            }
        }

        static int callMethod(lua_State* inState)
        {
            const MethodBox* box = static_cast<const MethodBox*>(lua_touserdata(inState, lua_upvalueindex(1)));
            if (!box || !box->instance || !box->method)
            {
                return luaL_error(inState, "reflected method is no longer valid");
            }

            int first = 1;
            if (lua_gettop(inState) >= 1 && objectOf(inState, 1))
            {
                first = 2;
            }

            ReflectionTypeMethodInfo::Params params;
            params.reserve(box->method->paramTypes.size());
            for (std::size_t i = 0; i < box->method->paramTypes.size(); i++)
            {
                std::any value;
                if (!Script::Types::readValue(inState, first + static_cast<int>(i), box->method->paramTypes[i], value))
                {
                    return luaL_error(
                        inState,
                        "invalid argument %d for %s",
                        static_cast<int>(i) + 1,
                        box->method->name.toChar()
                    );
                }

                params.push_back(std::move(value));
            }

            return Script::Types::pushValue(inState, box->method->invoke(box->instance, params));
        }

        static int pushMethod(lua_State* inState, void* inInstance, const ReflectionTypeMethodInfo* inMethod)
        {
            MethodBox* box = static_cast<MethodBox*>(lua_newuserdatauv(inState, sizeof(MethodBox), 0));
            box->instance  = inInstance;
            box->method    = inMethod;
            lua_pushcclosure(inState, callMethod, 1);

            return 1;
        }

        int reflectedIndex(lua_State* inState)
        {
            if (lookupMethod(inState))
            {
                return 1;
            }

            Object* object = objectOf(inState, 1);
            if (!object || lua_type(inState, 2) != LUA_TSTRING)
            {
                lua_pushnil(inState);

                return 1;
            }

            const char*               key  = lua_tostring(inState, 2);
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*object));
            if (!type)
            {
                lua_pushnil(inState);

                return 1;
            }

            const ReflectionFieldAccessor accessor = type->resolve(key);
            if (Script::Types::pushField(inState, accessor, object))
            {
                return 1;
            }

            if (const ReflectionTypeMethodInfo* method = type->findMethod(key))
            {
                return pushMethod(inState, object, method);
            }

            lua_pushnil(inState);

            return 1;
        }

        int reflectedNewIndex(lua_State* inState)
        {
            Object* object = objectOf(inState, 1);
            if (!object || lua_type(inState, 2) != LUA_TSTRING)
            {
                return luaL_error(inState, "cannot set property");
            }

            const char*               key  = lua_tostring(inState, 2);
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*object));
            if (!type)
            {
                return luaL_error(inState, "type is not reflected");
            }

            const ReflectionFieldAccessor accessor = type->resolve(key);
            if (!Script::Types::setField(inState, accessor, object, 3))
            {
                return luaL_error(inState, "unknown property '%s'", key);
            }

            object->notifyPropertyEdited(key);

            return 0;
        }
    }
}
