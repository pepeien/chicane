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
            static constexpr inline const char* VEC4_METATABLE = "Chicane.Vec4";

            static Vec4* check(lua_State* inState, int inIndex)
            {
                return static_cast<Vec4*>(luaL_checkudata(inState, inIndex, VEC4_METATABLE));
            }

            static int construct(lua_State* inState)
            {
                const int args = lua_gettop(inState);
                Vec4      value;

                if (args >= 5)
                {
                    value = Vec4(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4)),
                        static_cast<float>(luaL_checknumber(inState, 5))
                    );
                }
                else if (args >= 2 && isVec3(inState, 2))
                {
                    const float w = args >= 3 ? static_cast<float>(luaL_checknumber(inState, 3)) : 0.0f;
                    value         = Vec4(checkVec3(inState, 2), w);
                }
                else if (args >= 2 && isVec2(inState, 2))
                {
                    value = Vec4(checkVec2(inState, 2));
                }
                else if (args >= 2 && lua_isnumber(inState, 2))
                {
                    value = Vec4(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                pushVec4(inState, value);

                return 1;
            }

            static int zero(lua_State* inState)
            {
                pushVec4(inState, Vec4::sZero());

                return 1;
            }

            static int one(lua_State* inState)
            {
                pushVec4(inState, Vec4::sOne());

                return 1;
            }

            static int right(lua_State* inState)
            {
                pushVec4(inState, Vec4::sRight());

                return 1;
            }

            static int forward(lua_State* inState)
            {
                pushVec4(inState, Vec4::sForward());

                return 1;
            }

            static int up(lua_State* inState)
            {
                pushVec4(inState, Vec4::sUp());

                return 1;
            }

            static int sentinel(lua_State* inState)
            {
                pushVec4(inState, Vec4::sSentinel());

                return 1;
            }

            static int index(lua_State* inState)
            {
                Vec4*       value = check(inState, 1);
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

                if (key[0] == 'z' && key[1] == '\0')
                {
                    lua_pushnumber(inState, value->z);

                    return 1;
                }

                if (key[0] == 'w' && key[1] == '\0')
                {
                    lua_pushnumber(inState, value->w);

                    return 1;
                }

                luaL_getmetatable(inState, VEC4_METATABLE);
                lua_pushvalue(inState, 2);
                lua_rawget(inState, -2);

                return 1;
            }

            static int newIndex(lua_State* inState)
            {
                Vec4*       value = check(inState, 1);
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

                if (key[0] == 'z' && key[1] == '\0')
                {
                    value->z = next;

                    return 0;
                }

                if (key[0] == 'w' && key[1] == '\0')
                {
                    value->w = next;

                    return 0;
                }

                return luaL_error(inState, "Vec4 has no field '%s'", key);
            }

            static int toString(lua_State* inState)
            {
                Vec4* value = check(inState, 1);
                lua_pushstring(inState, value->toString().toChar());

                return 1;
            }

            static int add(lua_State* inState)
            {
                pushVec4(inState, checkVec4(inState, 1) + checkVec4(inState, 2));

                return 1;
            }

            static int sub(lua_State* inState)
            {
                pushVec4(inState, checkVec4(inState, 1) - checkVec4(inState, 2));

                return 1;
            }

            static int mul(lua_State* inState)
            {
                if (lua_isnumber(inState, 2))
                {
                    pushVec4(inState, checkVec4(inState, 1) * static_cast<float>(lua_tonumber(inState, 2)));

                    return 1;
                }

                pushVec4(inState, checkVec4(inState, 1) * checkVec4(inState, 2));

                return 1;
            }

            static int eq(lua_State* inState)
            {
                lua_pushboolean(inState, checkVec4(inState, 1) == checkVec4(inState, 2));

                return 1;
            }

            static const luaL_Reg kMethods[] = {
                {"toString", toString},
                {nullptr,    nullptr }
            };

            static const luaL_Reg kStatics[] = {
                {"Zero",     zero    },
                {"One",      one     },
                {"Right",    right   },
                {"Forward",  forward },
                {"Up",       up      },
                {"Sentinel", sentinel},
                {nullptr,    nullptr }
            };

            void bindVec4(lua_State* inState)
            {
                luaL_newmetatable(inState, VEC4_METATABLE);
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
                lua_setglobal(inState, "Vec4");
            }

            void pushVec4(lua_State* inState, const Vec4& inValue)
            {
                Vec4* box = static_cast<Vec4*>(lua_newuserdatauv(inState, sizeof(Vec4), 0));
                new (box) Vec4(inValue);
                luaL_setmetatable(inState, VEC4_METATABLE);
            }

            bool isVec4(lua_State* inState, int inIndex)
            {
                return luaL_testudata(inState, inIndex, VEC4_METATABLE) != nullptr;
            }

            Vec4 checkVec4(lua_State* inState, int inIndex)
            {
                return *check(inState, inIndex);
            }
        }
    }
}
