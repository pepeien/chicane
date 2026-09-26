#pragma once

#include "Chicane/Grid.hpp"

struct lua_State;

namespace Chicane
{
    namespace Grid
    {
        class Component;

        namespace Types
        {
            CHICANE_GRID void bind(lua_State* inState);

            CHICANE_GRID void bindComponent(lua_State* inState);
            CHICANE_GRID void pushComponent(lua_State* inState, Component* inComponent);
            CHICANE_GRID bool isComponent(lua_State* inState, int inIndex);
            CHICANE_GRID Component* checkComponent(lua_State* inState, int inIndex);
        }
    }
}
