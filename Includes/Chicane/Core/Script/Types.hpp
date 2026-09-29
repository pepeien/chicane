#pragma once

#include <any>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Rotator.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/String.hpp"

struct lua_State;

namespace Chicane
{
    namespace Script
    {
        namespace Types
        {
            CHICANE_CORE void bind(lua_State* inState);

            CHICANE_CORE void bindVec2(lua_State* inState);
            CHICANE_CORE void pushVec2(lua_State* inState, const Vec2& inValue);
            CHICANE_CORE bool isVec2(lua_State* inState, int inIndex);
            CHICANE_CORE Vec2 checkVec2(lua_State* inState, int inIndex);

            CHICANE_CORE void bindVec3(lua_State* inState);
            CHICANE_CORE void pushVec3(lua_State* inState, const Vec3& inValue);
            CHICANE_CORE bool isVec3(lua_State* inState, int inIndex);
            CHICANE_CORE Vec3 checkVec3(lua_State* inState, int inIndex);

            CHICANE_CORE void bindVec4(lua_State* inState);
            CHICANE_CORE void pushVec4(lua_State* inState, const Vec4& inValue);
            CHICANE_CORE bool isVec4(lua_State* inState, int inIndex);
            CHICANE_CORE Vec4 checkVec4(lua_State* inState, int inIndex);

            CHICANE_CORE void bindColor(lua_State* inState);
            CHICANE_CORE void pushColor(lua_State* inState, const Color::Rgba& inValue);
            CHICANE_CORE bool isColor(lua_State* inState, int inIndex);
            CHICANE_CORE Color::Rgba checkColor(lua_State* inState, int inIndex);

            CHICANE_CORE void bindRotator(lua_State* inState);
            CHICANE_CORE void pushRotator(lua_State* inState, const Rotator& inValue);
            CHICANE_CORE bool isRotator(lua_State* inState, int inIndex);
            CHICANE_CORE Rotator checkRotator(lua_State* inState, int inIndex);

            CHICANE_CORE bool pushField(
                lua_State* inState, const ReflectionFieldAccessor& inAccessor, const void* inInstance
            );
            CHICANE_CORE bool setField(
                lua_State* inState, const ReflectionFieldAccessor& inAccessor, void* inInstance, int inIndex
            );
            CHICANE_CORE int pushValue(lua_State* inState, const std::any& inValue);
            CHICANE_CORE bool readValue(lua_State* inState, int inIndex, const String& inTypeName, std::any& outValue);
        }
    }
}
