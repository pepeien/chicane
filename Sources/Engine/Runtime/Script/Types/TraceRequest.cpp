#include "Chicane/Runtime/Script/Types.hpp"

#include <new>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Script/Types.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Types
    {
        static constexpr inline const char* TRACE_REQUEST_METATABLE = "Chicane.Runtime.TraceRequest";

        static SceneTraceRequest* check(lua_State* inState, int inIndex)
        {
            return static_cast<SceneTraceRequest*>(luaL_checkudata(inState, inIndex, TRACE_REQUEST_METATABLE));
        }

        static int collect(lua_State* inState)
        {
            check(inState, 1)->~SceneTraceRequest();

            return 0;
        }

        static int line(lua_State* inState)
        {
            pushTraceRequest(
                inState,
                SceneTraceRequest::sLine(
                    Script::Types::checkVec3(inState, 1),
                    Script::Types::checkVec3(inState, 2),
                    static_cast<float>(luaL_optnumber(inState, 3, SceneTraceRequest::DEFAULT_CELL_SIZE))
                )
            );

            return 1;
        }

        static int rectangle(lua_State* inState)
        {
            pushTraceRequest(
                inState,
                SceneTraceRequest::sRectangle(
                    Script::Types::checkVec3(inState, 1),
                    Script::Types::checkVec3(inState, 2),
                    Vec2(
                        static_cast<float>(luaL_checknumber(inState, 3)),
                        static_cast<float>(luaL_checknumber(inState, 4))
                    ),
                    static_cast<float>(luaL_optnumber(inState, 5, SceneTraceRequest::DEFAULT_CELL_SIZE))
                )
            );

            return 1;
        }

        static int cone(lua_State* inState)
        {
            pushTraceRequest(
                inState,
                SceneTraceRequest::sCone(
                    Script::Types::checkVec3(inState, 1),
                    Script::Types::checkVec3(inState, 2),
                    static_cast<float>(luaL_checknumber(inState, 3)),
                    static_cast<float>(luaL_optnumber(inState, 4, SceneTraceRequest::DEFAULT_CELL_SIZE)),
                    static_cast<std::uint32_t>(luaL_optinteger(inState, 5, SceneTraceRequest::DEFAULT_SEGEMENT_COUNT))
                )
            );

            return 1;
        }

        static int cylinder(lua_State* inState)
        {
            pushTraceRequest(
                inState,
                SceneTraceRequest::sCylinder(
                    Script::Types::checkVec3(inState, 1),
                    Script::Types::checkVec3(inState, 2),
                    static_cast<float>(luaL_checknumber(inState, 3)),
                    static_cast<float>(luaL_optnumber(inState, 4, SceneTraceRequest::DEFAULT_CELL_SIZE)),
                    static_cast<std::uint32_t>(luaL_optinteger(inState, 5, SceneTraceRequest::DEFAULT_SEGEMENT_COUNT))
                )
            );

            return 1;
        }

        static int isValid(lua_State* inState)
        {
            lua_pushboolean(inState, check(inState, 1)->isValid());

            return 1;
        }

        static int getLength(lua_State* inState)
        {
            lua_pushnumber(inState, check(inState, 1)->getLength());

            return 1;
        }

        static int getDirection(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->getDirection());

            return 1;
        }

        static int getOrigin(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->origin);

            return 1;
        }

        static int setOrigin(lua_State* inState)
        {
            check(inState, 1)->origin = Script::Types::checkVec3(inState, 2);

            return 0;
        }

        static int getDestination(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->destination);

            return 1;
        }

        static int setDestination(lua_State* inState)
        {
            check(inState, 1)->destination = Script::Types::checkVec3(inState, 2);

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"isValid",        isValid       },
            {"getLength",      getLength     },
            {"getDirection",   getDirection  },
            {"getOrigin",      getOrigin     },
            {"setOrigin",      setOrigin     },
            {"getDestination", getDestination},
            {"setDestination", setDestination},
            {nullptr,          nullptr       }
        };

        static const luaL_Reg kStatics[] = {
            {"Line",      line     },
            {"Rectangle", rectangle},
            {"Cone",      cone     },
            {"Cylinder",  cylinder },
            {nullptr,     nullptr  }
        };

        void bindTraceRequest(lua_State* inState)
        {
            luaL_newmetatable(inState, TRACE_REQUEST_METATABLE);
            lua_pushvalue(inState, -1);
            lua_setfield(inState, -2, "__index");
            luaL_setfuncs(inState, kMethods, 0);
            lua_pushcfunction(inState, collect);
            lua_setfield(inState, -2, "__gc");
            lua_pop(inState, 1);

            lua_newtable(inState);
            luaL_setfuncs(inState, kStatics, 0);
            lua_setglobal(inState, "TraceRequest");
        }

        void pushTraceRequest(lua_State* inState, const SceneTraceRequest& inValue)
        {
            SceneTraceRequest* box =
                static_cast<SceneTraceRequest*>(lua_newuserdatauv(inState, sizeof(SceneTraceRequest), 0));
            new (box) SceneTraceRequest(inValue);
            luaL_setmetatable(inState, TRACE_REQUEST_METATABLE);
        }

        bool isTraceRequest(lua_State* inState, int inIndex)
        {
            return luaL_testudata(inState, inIndex, TRACE_REQUEST_METATABLE) != nullptr;
        }

        SceneTraceRequest checkTraceRequest(lua_State* inState, int inIndex)
        {
            return *check(inState, inIndex);
        }
    }
}
