#include "Chicane/Grid/Script/Types.hpp"

#include <any>
#include <exception>

#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Script/Types.hpp"

#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Script/Types/ComponentBox.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Grid
    {
        namespace Types
        {
            static constexpr inline const char* COMPONENT_METATABLE = "Chicane.Grid.Component";

            static Component* liveOwner(const ComponentBox* inBox)
            {
                if (!inBox || !inBox->owner || !Script::Handle::contains(inBox->owner))
                {
                    return nullptr;
                }

                return inBox->owner;
            }

            static ComponentBox* testBox(lua_State* inState, int inIndex)
            {
                return static_cast<ComponentBox*>(luaL_testudata(inState, inIndex, COMPONENT_METATABLE));
            }

            static int reflectedIndex(lua_State* inState);
            static int reflectedNewIndex(lua_State* inState);

            static void ensureMetatable(lua_State* inState)
            {
                if (luaL_getmetatable(inState, COMPONENT_METATABLE) != LUA_TNIL)
                {
                    lua_pop(inState, 1);

                    return;
                }

                lua_pop(inState, 1);

                luaL_newmetatable(inState, COMPONENT_METATABLE);
                lua_pushcfunction(inState, reflectedIndex);
                lua_setfield(inState, -2, "__index");
                lua_pushcfunction(inState, reflectedNewIndex);
                lua_setfield(inState, -2, "__newindex");
                lua_pop(inState, 1);
            }

            static void pushBox(
                lua_State* inState, void* inInstance, const ReflectionTypeInfo* inType, Component* inOwner
            )
            {
                ensureMetatable(inState);

                ComponentBox* box = static_cast<ComponentBox*>(lua_newuserdatauv(inState, sizeof(ComponentBox), 0));
                box->instance     = inInstance;
                box->type         = inType;
                box->owner        = inOwner;
                luaL_setmetatable(inState, COMPONENT_METATABLE);
            }

            static int callMethod(lua_State* inState)
            {
                const ComponentBox* box =
                    static_cast<const ComponentBox*>(lua_touserdata(inState, lua_upvalueindex(1)));
                const ReflectionTypeMethodInfo* method =
                    static_cast<const ReflectionTypeMethodInfo*>(lua_touserdata(inState, lua_upvalueindex(2)));
                if (!box || !method || !liveOwner(box) || !box->instance)
                {
                    return luaL_error(inState, "reflected method is no longer valid");
                }

                int cursor = 1;
                if (lua_gettop(inState) >= 1 && testBox(inState, 1))
                {
                    cursor = 2;
                }

                ReflectionTypeMethodInfo::Params params;
                params.reserve(method->paramTypes.size());
                for (std::size_t i = 0; i < method->paramTypes.size(); i++)
                {
                    std::any value;
                    if (!Script::Types::readValue(inState, cursor, method->paramTypes[i], value))
                    {
                        return luaL_error(
                            inState,
                            "invalid argument %d for %s",
                            static_cast<int>(i) + 1,
                            method->getName().toChar()
                        );
                    }

                    cursor += 1;
                    params.push_back(std::move(value));
                }

                try
                {
                    return Script::Types::pushValue(inState, method->invoke(box->instance, params));
                }
                catch (const std::exception& error)
                {
                    return luaL_error(inState, "%s failed: %s", method->getName().toChar(), error.what());
                }
            }

            static int pushMethod(
                lua_State* inState, const ComponentBox& inBox, const ReflectionTypeMethodInfo* inMethod
            )
            {
                ComponentBox* box = static_cast<ComponentBox*>(lua_newuserdatauv(inState, sizeof(ComponentBox), 0));
                *box              = inBox;
                lua_pushlightuserdata(inState, const_cast<ReflectionTypeMethodInfo*>(inMethod));
                lua_pushcclosure(inState, callMethod, 2);

                return 1;
            }

            static int reflectedIndex(lua_State* inState)
            {
                ComponentBox* box   = testBox(inState, 1);
                Component*    owner = liveOwner(box);
                if (!owner || !box->instance || !box->type || lua_type(inState, 2) != LUA_TSTRING)
                {
                    lua_pushnil(inState);

                    return 1;
                }

                const char* key = lua_tostring(inState, 2);
                if (const ReflectionTypeMethodInfo* method = box->type->findMethod(key))
                {
                    return pushMethod(inState, *box, method);
                }

                const ReflectionFieldAccessor accessor = box->type->resolve(key);
                if (Script::Types::pushField(inState, accessor, box->instance))
                {
                    return 1;
                }

                if (accessor.isValid() && accessor.typeIndex.has_value())
                {
                    if (const ReflectionTypeInfo* nested =
                            ReflectionTypeRegistry::sInstance().find(accessor.typeIndex.value()))
                    {
                        if (void* address = accessor.address(box->instance))
                        {
                            pushBox(inState, address, nested, owner);

                            return 1;
                        }
                    }
                }

                lua_pushnil(inState);

                return 1;
            }

            static int reflectedNewIndex(lua_State* inState)
            {
                ComponentBox* box   = testBox(inState, 1);
                Component*    owner = liveOwner(box);
                if (!owner || !box->instance || !box->type || lua_type(inState, 2) != LUA_TSTRING)
                {
                    return luaL_error(inState, "cannot set property");
                }

                const char*                   key      = lua_tostring(inState, 2);
                const ReflectionFieldAccessor accessor = box->type->resolve(key);
                if (!Script::Types::setField(inState, accessor, box->instance, 3))
                {
                    return luaL_error(inState, "unknown property '%s'", key);
                }

                return 0;
            }

            void pushComponent(lua_State* inState, Component* inComponent)
            {
                if (!inComponent)
                {
                    lua_pushnil(inState);

                    return;
                }

                const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*inComponent));
                pushBox(inState, inComponent, type, inComponent);
            }

            bool isComponent(lua_State* inState, int inIndex)
            {
                ComponentBox* box = testBox(inState, inIndex);

                return liveOwner(box) != nullptr && box->instance == box->owner;
            }

            Component* checkComponent(lua_State* inState, int inIndex)
            {
                ComponentBox* box       = testBox(inState, inIndex);
                Component*    component = liveOwner(box);
                if (!component || box->instance != box->owner)
                {
                    luaL_error(inState, "component is no longer valid");

                    return nullptr;
                }

                return component;
            }
        }
    }
}
