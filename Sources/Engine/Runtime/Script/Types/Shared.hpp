#pragma once

#include <any>
#include <cstddef>

#include "Chicane/Core/Script/Types.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Runtime/Script/Types.hpp"

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
        // Metatable of the Object base, used when a concrete type is not reflected
        static constexpr inline const char* OBJECT_MT = "Chicane.Runtime.Object";

        // Field holding the sentinel that marks a metatable as owned by the engine
        static constexpr inline const char* OWNER_FIELD = "__chicane";

        // Registry slot of the table holding the few methods that cannot be reflected
        static constexpr inline const char* METHODS_REGISTRY = "Chicane.Runtime.Object.Methods";

        struct ObjectBox
        {
            Object* object;
        };

        // Every scene object reaches Lua as this box, with a metatable named after
        // its reflected class. Lua cannot replace a metatable on userdata, so the
        // sentinel is enough to tell an engine box from a look-alike table.
        void* sOwnerSentinel();
        ObjectBox* testObjectBox(lua_State* inState, int inIndex);

        int reflectedIndex(lua_State* inState);
        int reflectedNewIndex(lua_State* inState);

        // Marshalling of reflected values that Core cannot handle, because it does
        // not know about the scene object types
        int pushReflectedValue(lua_State* inState, const ReflectionTypeMethodInfo& inMethod, const std::any& inValue);
        bool readReflectedValue(
            lua_State*    inState,
            int           inIndex,
            const String& inTypeName,
            std::any&     outValue,
            int&          outConsumed
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
