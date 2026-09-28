#pragma once

#include <any>

#include "Chicane/Core/Script/Types.hpp"
#include "Chicane/Core/String.hpp"

struct lua_State;

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    struct ReflectionTypeMethodInfo;

    namespace Types
    {
        int pushReflectedValue(lua_State* inState, const ReflectionTypeMethodInfo& inMethod, const std::any& inValue);
        bool readReflectedValue(
            lua_State* inState, int inIndex, const String& inTypeName, std::any& outValue, int& outConsumed
        );

        inline Vec2 checkVec2Arg(lua_State* inState, int inIndex)
        {
            if (Script::Types::isVec2(inState, inIndex))
            {
                return Script::Types::checkVec2(inState, inIndex);
            }

            return Vec2(
                static_cast<float>(luaL_checknumber(inState, inIndex)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 1))
            );
        }

        inline Vec3 checkVec3Arg(lua_State* inState, int inIndex)
        {
            if (Script::Types::isVec3(inState, inIndex))
            {
                return Script::Types::checkVec3(inState, inIndex);
            }

            return Vec3(
                static_cast<float>(luaL_checknumber(inState, inIndex)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 1)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 2))
            );
        }

        inline Vec4 checkVec4Arg(lua_State* inState, int inIndex)
        {
            if (Script::Types::isVec4(inState, inIndex))
            {
                return Script::Types::checkVec4(inState, inIndex);
            }

            return Vec4(
                static_cast<float>(luaL_checknumber(inState, inIndex)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 1)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 2)),
                static_cast<float>(luaL_checknumber(inState, inIndex + 3))
            );
        }
    }
}
