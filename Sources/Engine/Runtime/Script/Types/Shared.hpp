#pragma once

#include "Chicane/Core/Script/Types.hpp"
#include "Chicane/Runtime/Script/Types.hpp"

struct lua_State;

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Types
    {
        static constexpr inline const char* ACTOR_MT      = "Chicane.Runtime.Actor";
        static constexpr inline const char* APAWN_MT      = "Chicane.Runtime.APawn";
        static constexpr inline const char* ACHARACTER_MT = "Chicane.Runtime.ACharacter";
        static constexpr inline const char* ACAMERA_MT    = "Chicane.Runtime.ACamera";
        static constexpr inline const char* ALIGHT_MT     = "Chicane.Runtime.ALight";
        static constexpr inline const char* ASKY_MT       = "Chicane.Runtime.ASky";
        static constexpr inline const char* ASOUND_MT     = "Chicane.Runtime.ASound";

        static constexpr inline const char* COMPONENT_MT = "Chicane.Runtime.Component";
        static constexpr inline const char* CVIEW_MT     = "Chicane.Runtime.CView";
        static constexpr inline const char* CCAMERA_MT   = "Chicane.Runtime.CCamera";
        static constexpr inline const char* CLIGHT_MT    = "Chicane.Runtime.CLight";
        static constexpr inline const char* CMESH_MT     = "Chicane.Runtime.CMesh";
        static constexpr inline const char* CPHYSICS_MT  = "Chicane.Runtime.CPhysics";
        static constexpr inline const char* CSOUND_MT    = "Chicane.Runtime.CSound";

        struct ActorBox
        {
            Actor* actor;
        };

        struct ComponentBox
        {
            Component* component;
        };

        int reflectedIndex(lua_State* inState);
        int reflectedNewIndex(lua_State* inState);

        inline void bindMetatable(
            lua_State* inState, const char* inName, const luaL_Reg* inMethods, const char* inParent = nullptr
        )
        {
            luaL_newmetatable(inState, inName);
            luaL_setfuncs(inState, inMethods, 0);
            lua_pushcfunction(inState, reflectedIndex);
            lua_setfield(inState, -2, "__index");
            lua_pushcfunction(inState, reflectedNewIndex);
            lua_setfield(inState, -2, "__newindex");
            if (inParent)
            {
                luaL_getmetatable(inState, inParent);
                lua_setmetatable(inState, -2);
            }

            lua_pop(inState, 1);
        }

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

        void bindAPawn(lua_State* inState);
        void bindACharacter(lua_State* inState);
        void bindACamera(lua_State* inState);
        void bindALight(lua_State* inState);
        void bindASky(lua_State* inState);
        void bindASound(lua_State* inState);

        void bindCView(lua_State* inState);
        void bindCCamera(lua_State* inState);
        void bindCLight(lua_State* inState);
        void bindCMesh(lua_State* inState);
        void bindCPhysics(lua_State* inState);
        void bindCSound(lua_State* inState);
    }
}
