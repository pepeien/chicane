#include "Chicane/Core/Script/Types.hpp"

#include <new>

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
            static constexpr inline const char* ROTATOR_METATABLE = "Chicane.Rotator";

            static Rotator* check(lua_State* inState, int inIndex)
            {
                return static_cast<Rotator*>(luaL_checkudata(inState, inIndex, ROTATOR_METATABLE));
            }

            static int construct(lua_State* inState)
            {
                const int args = lua_gettop(inState);
                Rotator   value;

                if (args >= 2 && isVec3(inState, 2))
                {
                    value = Rotator(checkVec3(inState, 2));
                }
                else if (args >= 4)
                {
                    value = Rotator(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4))
                    );
                }
                else if (args >= 2)
                {
                    value = Rotator(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                pushRotator(inState, value);

                return 1;
            }

            static int index(lua_State* inState)
            {
                check(inState, 1);

                luaL_getmetatable(inState, ROTATOR_METATABLE);
                lua_pushvalue(inState, 2);
                lua_rawget(inState, -2);

                return 1;
            }

            static int set(lua_State* inState)
            {
                Rotator* value = check(inState, 1);

                if (isVec3(inState, 2))
                {
                    value->set(checkVec3(inState, 2));
                }
                else if (isRotator(inState, 2))
                {
                    value->set(checkRotator(inState, 2));
                }
                else if (lua_gettop(inState) >= 4)
                {
                    value->set(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4))
                    );
                }
                else
                {
                    value->set(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                return 0;
            }

            static int addLocal(lua_State* inState)
            {
                Rotator* value = check(inState, 1);

                if (isVec3(inState, 2))
                {
                    value->addLocal(checkVec3(inState, 2));
                }
                else if (lua_gettop(inState) >= 4)
                {
                    value->addLocal(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4))
                    );
                }
                else
                {
                    value->addLocal(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                return 0;
            }

            static int addWorld(lua_State* inState)
            {
                Rotator* value = check(inState, 1);

                if (isVec3(inState, 2))
                {
                    value->addWorld(checkVec3(inState, 2));
                }
                else if (lua_gettop(inState) >= 4)
                {
                    value->addWorld(
                        static_cast<float>(luaL_checknumber(inState, 2)),
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4))
                    );
                }
                else
                {
                    value->addWorld(static_cast<float>(luaL_checknumber(inState, 2)));
                }

                return 0;
            }

            static int lookAt(lua_State* inState)
            {
                check(inState, 1)->lookAt(checkVec3(inState, 2), checkVec3(inState, 3));

                return 0;
            }

            static int getAngles(lua_State* inState)
            {
                pushVec3(inState, check(inState, 1)->getAngles());

                return 1;
            }

            static int setAngles(lua_State* inState)
            {
                check(inState, 1)->setAngles(checkVec3(inState, 2));

                return 0;
            }

            static int getRight(lua_State* inState)
            {
                pushVec3(inState, check(inState, 1)->getRight());

                return 1;
            }

            static int getForward(lua_State* inState)
            {
                pushVec3(inState, check(inState, 1)->getForward());

                return 1;
            }

            static int getUp(lua_State* inState)
            {
                pushVec3(inState, check(inState, 1)->getUp());

                return 1;
            }

            static int collect(lua_State* inState)
            {
                check(inState, 1)->~Rotator();

                return 0;
            }

            static const luaL_Reg kMethods[] = {
                {"set",        set       },
                {"addLocal",   addLocal  },
                {"addWorld",   addWorld  },
                {"lookAt",     lookAt    },
                {"getAngles",  getAngles },
                {"setAngles",  setAngles },
                {"getRight",   getRight  },
                {"getForward", getForward},
                {"getUp",      getUp     },
                {nullptr,      nullptr   }
            };

            void bindRotator(lua_State* inState)
            {
                luaL_newmetatable(inState, ROTATOR_METATABLE);
                luaL_setfuncs(inState, kMethods, 0);
                lua_pushcfunction(inState, index);
                lua_setfield(inState, -2, "__index");
                lua_pushcfunction(inState, collect);
                lua_setfield(inState, -2, "__gc");
                lua_pop(inState, 1);

                lua_newtable(inState);
                lua_newtable(inState);
                lua_pushcfunction(inState, construct);
                lua_setfield(inState, -2, "__call");
                lua_setmetatable(inState, -2);
                lua_setglobal(inState, "Rotator");
            }

            void pushRotator(lua_State* inState, const Rotator& inValue)
            {
                Rotator* box = static_cast<Rotator*>(lua_newuserdatauv(inState, sizeof(Rotator), 0));
                new (box) Rotator(inValue);
                luaL_setmetatable(inState, ROTATOR_METATABLE);
            }

            bool isRotator(lua_State* inState, int inIndex)
            {
                return luaL_testudata(inState, inIndex, ROTATOR_METATABLE) != nullptr;
            }

            Rotator checkRotator(lua_State* inState, int inIndex)
            {
                return *check(inState, inIndex);
            }
        }
    }
}
