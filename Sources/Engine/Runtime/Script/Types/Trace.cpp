#include "Chicane/Runtime/Script/Types.hpp"

#include <new>

#include "Chicane/Core/Script/Types.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Types
    {
        static constexpr inline const char* TRACE_METATABLE = "Chicane.Runtime.Trace";

        static SceneTraceResponse* check(lua_State* inState, int inIndex)
        {
            return static_cast<SceneTraceResponse*>(luaL_checkudata(inState, inIndex, TRACE_METATABLE));
        }

        static int collect(lua_State* inState)
        {
            check(inState, 1)->~SceneTraceResponse();

            return 0;
        }

        static int getLocation(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->location);

            return 1;
        }

        static int getImpact(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->impact);

            return 1;
        }

        static int getDistance(lua_State* inState)
        {
            lua_pushnumber(inState, check(inState, 1)->distance);

            return 1;
        }

        static int getStart(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->start);

            return 1;
        }

        static int getEnd(lua_State* inState)
        {
            Script::Types::pushVec3(inState, check(inState, 1)->end);

            return 1;
        }

        static int getActor(lua_State* inState)
        {
            Actor* actor = check(inState, 1)->actor;
            if (!actor)
            {
                lua_pushnil(inState);

                return 1;
            }

            pushActor(inState, actor);

            return 1;
        }

        static const luaL_Reg kMethods[] = {
            {"getLocation", getLocation},
            {"getImpact",   getImpact  },
            {"getDistance", getDistance},
            {"getStart",    getStart   },
            {"getEnd",      getEnd     },
            {"getActor",    getActor   },
            {nullptr,       nullptr    }
        };

        void bindTrace(lua_State* inState)
        {
            luaL_newmetatable(inState, TRACE_METATABLE);
            lua_pushvalue(inState, -1);
            lua_setfield(inState, -2, "__index");
            luaL_setfuncs(inState, kMethods, 0);
            lua_pushcfunction(inState, collect);
            lua_setfield(inState, -2, "__gc");
            lua_pop(inState, 1);
        }

        void pushTrace(lua_State* inState, const SceneTraceResponse& inValue)
        {
            SceneTraceResponse* box =
                static_cast<SceneTraceResponse*>(lua_newuserdatauv(inState, sizeof(SceneTraceResponse), 0));
            new (box) SceneTraceResponse(inValue);
            luaL_setmetatable(inState, TRACE_METATABLE);
        }

        bool isTrace(lua_State* inState, int inIndex)
        {
            return luaL_testudata(inState, inIndex, TRACE_METATABLE) != nullptr;
        }

        SceneTraceResponse checkTrace(lua_State* inState, int inIndex)
        {
            return *check(inState, inIndex);
        }
    }
}
