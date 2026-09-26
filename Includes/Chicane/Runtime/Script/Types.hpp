#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Response.hpp"

struct lua_State;

namespace Chicane
{
    class Actor;
    class Component;

    namespace Types
    {
        CHICANE_RUNTIME void bind(lua_State* inState);

        CHICANE_RUNTIME void bindActor(lua_State* inState);
        CHICANE_RUNTIME void pushActor(lua_State* inState, Actor* inActor);
        CHICANE_RUNTIME bool isActor(lua_State* inState, int inIndex);
        CHICANE_RUNTIME Actor* checkActor(lua_State* inState, int inIndex);

        CHICANE_RUNTIME void bindComponent(lua_State* inState);
        CHICANE_RUNTIME void pushComponent(lua_State* inState, Component* inComponent);
        CHICANE_RUNTIME bool isComponent(lua_State* inState, int inIndex);
        CHICANE_RUNTIME Component* checkComponent(lua_State* inState, int inIndex);

        CHICANE_RUNTIME void bindTraceRequest(lua_State* inState);
        CHICANE_RUNTIME void pushTraceRequest(lua_State* inState, const SceneTraceRequest& inValue);
        CHICANE_RUNTIME bool isTraceRequest(lua_State* inState, int inIndex);
        CHICANE_RUNTIME SceneTraceRequest checkTraceRequest(lua_State* inState, int inIndex);

        CHICANE_RUNTIME void bindTrace(lua_State* inState);
        CHICANE_RUNTIME void pushTrace(lua_State* inState, const SceneTraceResponse& inValue);
        CHICANE_RUNTIME bool isTrace(lua_State* inState, int inIndex);
        CHICANE_RUNTIME SceneTraceResponse checkTrace(lua_State* inState, int inIndex);
    }
}
