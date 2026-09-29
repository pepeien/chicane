#include "Chicane/Core/Script/Types.hpp"

#include <new>

#include "Chicane/Core/String.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Script
    {
        namespace Types
        {
            static constexpr inline const char* COLOR_METATABLE = "Chicane.Color";

            static Color::Rgba* check(lua_State* inState, int inIndex)
            {
                return static_cast<Color::Rgba*>(luaL_checkudata(inState, inIndex, COLOR_METATABLE));
            }

            static std::uint8_t channel(lua_State* inState, int inIndex, std::uint8_t inFallback)
            {
                if (lua_isnoneornil(inState, inIndex))
                {
                    return inFallback;
                }

                const lua_Integer value = luaL_checkinteger(inState, inIndex);
                if (value < 0)
                {
                    return 0;
                }

                if (value > 255)
                {
                    return 255;
                }

                return static_cast<std::uint8_t>(value);
            }

            static int construct(lua_State* inState)
            {
                pushColor(
                    inState,
                    Color::Rgba(
                        channel(inState, 2, 0),
                        channel(inState, 3, 0),
                        channel(inState, 4, 0),
                        channel(inState, 5, 255)
                    )
                );

                return 1;
            }

            static int fromHex(lua_State* inState)
            {
                pushColor(inState, Color::toRgba(luaL_checkstring(inState, 1)));

                return 1;
            }

            static int index(lua_State* inState)
            {
                Color::Rgba* value = check(inState, 1);
                const char*  key   = luaL_checkstring(inState, 2);

                if (key[0] == 'r' && key[1] == '\0')
                {
                    lua_pushinteger(inState, value->r);

                    return 1;
                }

                if (key[0] == 'g' && key[1] == '\0')
                {
                    lua_pushinteger(inState, value->g);

                    return 1;
                }

                if (key[0] == 'b' && key[1] == '\0')
                {
                    lua_pushinteger(inState, value->b);

                    return 1;
                }

                if (key[0] == 'a' && key[1] == '\0')
                {
                    lua_pushinteger(inState, value->a);

                    return 1;
                }

                luaL_getmetatable(inState, COLOR_METATABLE);
                lua_pushvalue(inState, 2);
                lua_rawget(inState, -2);

                return 1;
            }

            static int newIndex(lua_State* inState)
            {
                Color::Rgba*       value = check(inState, 1);
                const char*        key   = luaL_checkstring(inState, 2);
                const std::uint8_t next  = channel(inState, 3, 0);

                if (key[0] == 'r' && key[1] == '\0')
                {
                    value->r = next;

                    return 0;
                }

                if (key[0] == 'g' && key[1] == '\0')
                {
                    value->g = next;

                    return 0;
                }

                if (key[0] == 'b' && key[1] == '\0')
                {
                    value->b = next;

                    return 0;
                }

                if (key[0] == 'a' && key[1] == '\0')
                {
                    value->a = next;

                    return 0;
                }

                return luaL_error(inState, "Color has no field '%s'", key);
            }

            static int isVisible(lua_State* inState)
            {
                lua_pushboolean(inState, Color::isVisible(*check(inState, 1)));

                return 1;
            }

            static const luaL_Reg kMethods[] = {
                {"isVisible", isVisible},
                {nullptr,     nullptr  }
            };

            static const luaL_Reg kStatics[] = {
                {"fromHex", fromHex},
                {nullptr,   nullptr}
            };

            void bindColor(lua_State* inState)
            {
                luaL_newmetatable(inState, COLOR_METATABLE);
                luaL_setfuncs(inState, kMethods, 0);
                lua_pushcfunction(inState, index);
                lua_setfield(inState, -2, "__index");
                lua_pushcfunction(inState, newIndex);
                lua_setfield(inState, -2, "__newindex");
                lua_pop(inState, 1);

                lua_newtable(inState);
                luaL_setfuncs(inState, kStatics, 0);
                lua_newtable(inState);
                lua_pushcfunction(inState, construct);
                lua_setfield(inState, -2, "__call");
                lua_setmetatable(inState, -2);
                lua_setglobal(inState, "Color");
            }

            void pushColor(lua_State* inState, const Color::Rgba& inValue)
            {
                Color::Rgba* box = static_cast<Color::Rgba*>(lua_newuserdatauv(inState, sizeof(Color::Rgba), 0));
                new (box) Color::Rgba(inValue);
                luaL_setmetatable(inState, COLOR_METATABLE);
            }

            bool isColor(lua_State* inState, int inIndex)
            {
                return luaL_testudata(inState, inIndex, COLOR_METATABLE) != nullptr;
            }

            Color::Rgba checkColor(lua_State* inState, int inIndex)
            {
                return *check(inState, inIndex);
            }
        }
    }
}
