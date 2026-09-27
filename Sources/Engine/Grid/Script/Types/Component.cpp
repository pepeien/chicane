#include "Chicane/Grid/Script/Types.hpp"

#include <any>
#include <exception>

#include "Chicane/Core/Reflection/Type/Method.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Script/Types.hpp"

#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/Text.hpp"
#include "Chicane/Grid/Style/Display.hpp"

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

            struct ComponentBox
            {
                Component* component;
            };

            static Component* liveComponent(ComponentBox* inBox)
            {
                if (!inBox || !inBox->component || !Script::Handle::contains(inBox->component))
                {
                    return nullptr;
                }

                return inBox->component;
            }

            static int reflectedIndex(lua_State* inState)
            {
                if (lua_getmetatable(inState, 1))
                {
                    lua_pushvalue(inState, 2);
                    lua_rawget(inState, -2);
                    if (!lua_isnil(inState, -1))
                    {
                        lua_remove(inState, -2);

                        return 1;
                    }

                    lua_pop(inState, 2);
                }

                Component* component = checkComponent(inState, 1);
                if (!component || lua_type(inState, 2) != LUA_TSTRING)
                {
                    lua_pushnil(inState);

                    return 1;
                }

                const char*               key  = lua_tostring(inState, 2);
                const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*component));
                if (!type)
                {
                    lua_pushnil(inState);

                    return 1;
                }

                const ReflectionFieldAccessor accessor = type->resolve(key);
                if (Script::Types::pushField(inState, accessor, component))
                {
                    return 1;
                }

                lua_pushnil(inState);

                return 1;
            }

            static int reflectedNewIndex(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                if (!component || lua_type(inState, 2) != LUA_TSTRING)
                {
                    return luaL_error(inState, "cannot set property");
                }

                const char*               key  = lua_tostring(inState, 2);
                const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*component));
                if (!type)
                {
                    return luaL_error(inState, "type is not reflected");
                }

                const ReflectionFieldAccessor accessor = type->resolve(key);
                if (!Script::Types::setField(inState, accessor, component, 3))
                {
                    return luaL_error(inState, "unknown property '%s'", key);
                }

                return 0;
            }

            static int getClassName(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                lua_pushstring(inState, component->getClassName().toChar());

                return 1;
            }

            static int setClassName(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                component->setClassName(luaL_checkstring(inState, 2));

                return 0;
            }

            static int getText(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                if (Text* text = dynamic_cast<Text*>(component))
                {
                    lua_pushstring(inState, text->getText().toChar());

                    return 1;
                }

                lua_pushstring(inState, "");

                return 1;
            }

            static int setText(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                if (Text* text = dynamic_cast<Text*>(component))
                {
                    text->setText(luaL_checkstring(inState, 2));
                }

                return 0;
            }

            static int isVisible(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                lua_pushboolean(inState, !component->getStyle().isDisplay(StyleDisplay::None));

                return 1;
            }

            static int setVisible(lua_State* inState)
            {
                Component* component = checkComponent(inState, 1);
                component->setVisible(lua_toboolean(inState, 2) != 0);

                return 0;
            }

            static int invoke(lua_State* inState)
            {
                Component*  component = checkComponent(inState, 1);
                const char* name      = luaL_checkstring(inState, 2);

                const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*component));
                if (!type)
                {
                    return 0;
                }

                const ReflectionTypeMethodInfo* method = type->findMethod(name);
                if (!method)
                {
                    return 0;
                }

                ReflectionTypeMethod result(method);
                result.bind(component);

                // Arguments follow the method name, so the first one sits at index 3
                for (std::size_t i = 0; i < method->paramTypes.size(); i++)
                {
                    std::any value;
                    if (!Script::Types::readValue(inState, 3 + static_cast<int>(i), method->paramTypes[i], value))
                    {
                        return luaL_error(
                            inState,
                            "invalid argument %d for %s",
                            static_cast<int>(i) + 1,
                            method->name.toChar()
                        );
                    }

                    result.addParam(std::move(value));
                }

                try
                {
                    return Script::Types::pushValue(inState, result.invoke());
                }
                catch (const std::exception& error)
                {
                    return luaL_error(inState, "%s failed: %s", method->name.toChar(), error.what());
                }
            }

            static const luaL_Reg kMethods[] = {
                {"getClassName", getClassName},
                {"setClassName", setClassName},
                {"getText",      getText     },
                {"setText",      setText     },
                {"isVisible",    isVisible   },
                {"setVisible",   setVisible  },
                {"invoke",       invoke      },
                {nullptr,        nullptr     }
            };

            void bindComponent(lua_State* inState)
            {
                luaL_newmetatable(inState, COMPONENT_METATABLE);
                luaL_setfuncs(inState, kMethods, 0);
                lua_pushcfunction(inState, reflectedIndex);
                lua_setfield(inState, -2, "__index");
                lua_pushcfunction(inState, reflectedNewIndex);
                lua_setfield(inState, -2, "__newindex");
                lua_pop(inState, 1);
            }

            void pushComponent(lua_State* inState, Component* inComponent)
            {
                ComponentBox* box = static_cast<ComponentBox*>(lua_newuserdatauv(inState, sizeof(ComponentBox), 0));
                box->component    = inComponent;
                luaL_setmetatable(inState, COMPONENT_METATABLE);
            }

            bool isComponent(lua_State* inState, int inIndex)
            {
                ComponentBox* box = static_cast<ComponentBox*>(luaL_testudata(inState, inIndex, COMPONENT_METATABLE));

                return liveComponent(box) != nullptr;
            }

            Component* checkComponent(lua_State* inState, int inIndex)
            {
                ComponentBox* box = static_cast<ComponentBox*>(luaL_testudata(inState, inIndex, COMPONENT_METATABLE));
                Component*    component = liveComponent(box);
                if (!component)
                {
                    luaL_error(inState, "component is no longer valid");

                    return nullptr;
                }

                return component;
            }
        }
    }
}
