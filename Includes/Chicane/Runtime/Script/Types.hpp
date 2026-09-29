#pragma once

#include "Chicane/Runtime.hpp"

struct lua_State;

namespace Chicane
{
    class Actor;
    class Component;
    class Object;

    namespace Types
    {
        CHICANE_RUNTIME void bind(lua_State* inState);

        CHICANE_RUNTIME void pushObject(lua_State* inState, Object* inObject);
        CHICANE_RUNTIME bool isObject(lua_State* inState, int inIndex);
        CHICANE_RUNTIME Object* checkObject(lua_State* inState, int inIndex);

        CHICANE_RUNTIME void pushActor(lua_State* inState, Actor* inActor);
        CHICANE_RUNTIME bool isActor(lua_State* inState, int inIndex);
        CHICANE_RUNTIME Actor* checkActor(lua_State* inState, int inIndex);

        CHICANE_RUNTIME void pushComponent(lua_State* inState, Component* inComponent);
        CHICANE_RUNTIME bool isComponent(lua_State* inState, int inIndex);
        CHICANE_RUNTIME Component* checkComponent(lua_State* inState, int inIndex);
    }
}
