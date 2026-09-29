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
            static constexpr inline const char* VEC2_METATABLE = "Chicane.Vec2";

            static Vec2* check(lua_State* inState, int inIndex)
            {
                return static_cast<Vec2*>(luaL_checkudata(inState, inIndex, VEC2_METATABLE));
            }

            static int construct(lua_State* inState)
            {
                const int args = lua_gettop(inState);
                Vec2      value;

                if (args >= 3)
                {
                    value = Vec2(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3))
                    );
                }
                else if (args >= 2 && lua_isnumber(inState, 2))
                {
                    value = Vec2(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                pushVec2(inState, value);

                return 1;
            }

            static int zero(lua_State* inState)
            {
                pushVec2(inState, Vec2::sZero());

                return 1;
            }

            static int one(lua_State* inState)
            {
                pushVec2(inState, Vec2::sOne());

                return 1;
            }

            static int right(lua_State* inState)
            {
                pushVec2(inState, Vec2::sRight());

                return 1;
            }

            static int up(lua_State* inState)
            {
                pushVec2(inState, Vec2::sUp());

                return 1;
            }

            static int index(lua_State* inState)
            {
                Vec2*       value = check(inState, 1);
                const char* key   = luaL_checkstring(inState, 2);

                if (key[0] == 'x' && key[1] == '\0')
                {
                    lua_pushnumber(inState, value->x);

                    return 1;
                }

                if (key[0] == 'y' && key[1] == '\0')
                {
                    lua_pushnumber(inState, value->y);

                    return 1;
                }

                luaL_getmetatable(inState, VEC2_METATABLE);
                lua_pushvalue(inState, 2);
                lua_rawget(inState, -2);

                return 1;
            }

            static int newIndex(lua_State* inState)
            {
                Vec2*       value = check(inState, 1);
                const char* key   = luaL_checkstring(inState, 2);
                const float next  = static_cast<float>(luaL_checknumber(inState, 3));

                if (key[0] == 'x' && key[1] == '\0')
                {
                    value->x = next;

                    return 0;
                }

                if (key[0] == 'y' && key[1] == '\0')
                {
                    value->y = next;

                    return 0;
                }

                return luaL_error(inState, "Vec2 has no field '%s'", key);
            }

            static int toString(lua_State* inState)
            {
                Vec2* value = check(inState, 1);
                lua_pushstring(inState, value->toString().toChar());

                return 1;
            }

            static int add(lua_State* inState)
            {
                pushVec2(inState, checkVec2(inState, 1) + checkVec2(inState, 2));

                return 1;
            }

            static int sub(lua_State* inState)
            {
                pushVec2(inState, checkVec2(inState, 1) - checkVec2(inState, 2));

                return 1;
            }

            static int mul(lua_State* inState)
            {
                if (lua_isnumber(inState, 2))
                {
                    pushVec2(inState, checkVec2(inState, 1) * static_cast<float>(lua_tonumber(inState, 2)));

                    return 1;
                }

                pushVec2(inState, checkVec2(inState, 1) * checkVec2(inState, 2));

                return 1;
            }

            static int eq(lua_State* inState)
            {
                lua_pushboolean(inState, checkVec2(inState, 1) == checkVec2(inState, 2));

                return 1;
            }

            static const luaL_Reg kMethods[] = {
                {"toString", toString},
                {nullptr,    nullptr }
            };

            static const luaL_Reg kStatics[] = {
                {"Zero",  zero   },
                {"One",   one    },
                {"Right", right  },
                {"Up",    up     },
                {nullptr, nullptr}
            };

            void bindVec2(lua_State* inState)
            {
                luaL_newmetatable(inState, VEC2_METATABLE);
                luaL_setfuncs(inState, kMethods, 0);
                lua_pushcfunction(inState, index);
                lua_setfield(inState, -2, "__index");
                lua_pushcfunction(inState, newIndex);
                lua_setfield(inState, -2, "__newindex");
                lua_pushcfunction(inState, toString);
                lua_setfield(inState, -2, "__tostring");
                lua_pushcfunction(inState, add);
                lua_setfield(inState, -2, "__add");
                lua_pushcfunction(inState, sub);
                lua_setfield(inState, -2, "__sub");
                lua_pushcfunction(inState, mul);
                lua_setfield(inState, -2, "__mul");
                lua_pushcfunction(inState, eq);
                lua_setfield(inState, -2, "__eq");
                lua_pop(inState, 1);

                lua_newtable(inState);
                luaL_setfuncs(inState, kStatics, 0);
                lua_newtable(inState);
                lua_pushcfunction(inState, construct);
                lua_setfield(inState, -2, "__call");
                lua_setmetatable(inState, -2);
                lua_setglobal(inState, "Vec2");
            }

            void pushVec2(lua_State* inState, const Vec2& inValue)
            {
                Vec2* box = static_cast<Vec2*>(lua_newuserdatauv(inState, sizeof(Vec2), 0));
                new (box) Vec2(inValue);
                luaL_setmetatable(inState, VEC2_METATABLE);
            }

            bool isVec2(lua_State* inState, int inIndex)
            {
                return luaL_testudata(inState, inIndex, VEC2_METATABLE) != nullptr;
            }

            Vec2 checkVec2(lua_State* inState, int inIndex)
            {
                return *check(inState, inIndex);
            }
        }
    }
}
