#include "Property.hpp"

#include "ReflectedValue.hpp"
#include "Value.hpp"

#include <exception>
#include <typeinfo>

#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Types
    {
        static Object* objectOf(lua_State* inState, int inIndex)
        {
            return liveObject(inState, inIndex);
        }

        static int callMethod(lua_State* inState)
        {
            const MethodBox* box = static_cast<const MethodBox*>(lua_touserdata(inState, lua_upvalueindex(1)));
            if (!box || !box->instance || !box->method || !Script::Handle::contains(box->instance))
            {
                return luaL_error(inState, "reflected method is no longer valid");
            }

            int cursor = 1;
            if (lua_gettop(inState) >= 1 && objectOf(inState, 1))
            {
                cursor = 2;
            }

            ReflectionTypeMethodInfo::Params params;
            params.reserve(box->method->paramTypes.size());
            for (std::size_t i = 0; i < box->method->paramTypes.size(); i++)
            {
                std::any value;
                int      consumed = 1;
                if (!readReflectedValue(inState, cursor, box->method->paramTypes[i], value, consumed))
                {
                    return luaL_error(
                        inState,
                        "invalid argument %d for %s",
                        static_cast<int>(i) + 1,
                        box->method->getName().toChar()
                    );
                }

                cursor += consumed;
                params.push_back(std::move(value));
            }

            try
            {
                return pushReflectedValue(inState, *box->method, box->method->invoke(box->instance, params));
            }
            catch (const std::exception& error)
            {
                return luaL_error(inState, "%s failed: %s", box->method->getName().toChar(), error.what());
            }
        }

        static int pushMethod(lua_State* inState, Object* inInstance, const ReflectionTypeMethodInfo* inMethod)
        {
            MethodBox* box = static_cast<MethodBox*>(lua_newuserdatauv(inState, sizeof(MethodBox), 0));
            box->instance  = inInstance;
            box->method    = inMethod;
            lua_pushcclosure(inState, callMethod, 1);

            return 1;
        }

        int reflectedIndex(lua_State* inState)
        {
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

            if (accessor.isValid() && accessor.size == sizeof(void*) && accessor.address(object))
            {
                void* pointee = *reinterpret_cast<void* const*>(accessor.address(object));
                if (!pointee)
                {
                    lua_pushnil(inState);

                    return 1;
                }

                if (Script::Handle::contains(pointee))
                {
                    pushObject(inState, static_cast<Object*>(pointee));

                    return 1;
                }
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
            if (Script::Types::setField(inState, accessor, object, 3))
            {
                object->notifyPropertyEdited(key);

                return 0;
            }

            if (accessor.isValid() && accessor.size == sizeof(void*) && accessor.address(object))
            {
                Object* value = nullptr;
                if (!lua_isnoneornil(inState, 3))
                {
                    value = checkObject(inState, 3);
                }

                *reinterpret_cast<void**>(accessor.address(object)) = value;
                object->notifyPropertyEdited(key);

                return 0;
            }

            return luaL_error(inState, "unknown property '%s'", key);
        }
    }
}
