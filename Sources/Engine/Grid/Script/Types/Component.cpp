#include "Chicane/Grid/Script/Types.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

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
                result.invoke();

                return 0;
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
                lua_pushvalue(inState, -1);
                lua_setfield(inState, -2, "__index");
                luaL_setfuncs(inState, kMethods, 0);
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
                return luaL_testudata(inState, inIndex, COMPONENT_METATABLE) != nullptr;
            }

            Component* checkComponent(lua_State* inState, int inIndex)
            {
                ComponentBox* box = static_cast<ComponentBox*>(luaL_checkudata(inState, inIndex, COMPONENT_METATABLE));
                if (!box || !box->component)
                {
                    luaL_error(inState, "component is no longer valid");

                    return nullptr;
                }

                return box->component;
            }
        }
    }
}
