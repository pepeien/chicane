#pragma once

#include <cstdint>

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Range.hpp"

namespace Chicane
{
    namespace Smoke
    {
        // Values
        static constexpr inline const char* BOOLEAN_TRUE_VALUE        = "true";
        static constexpr inline const char* BOOLEAN_TRUE_ONE_VALUE    = "1";
        static constexpr inline const char* BOOLEAN_TRUE_YES_VALUE    = "yes";
        static constexpr inline const float COLOR_CHANNEL_MAX_VALUE   = 255.0f;
        static constexpr inline const float COLOR_ALPHA_DEFAULT_VALUE = 1.0f;

        CHICANE_SMOKE float randomFloat(float inFrom, float inTo);
        CHICANE_SMOKE float randomFloat();
        CHICANE_SMOKE Vec3 randomDirection();

        CHICANE_SMOKE Range parseRange(const pugi::xml_node& inNode, const char* inName, const Range& inFallback);
        CHICANE_SMOKE float parseFloat(const pugi::xml_node& inNode, const char* inName, float inFallback);
        CHICANE_SMOKE bool parseBool(const pugi::xml_node& inNode, const char* inName, bool inFallback);
        CHICANE_SMOKE std::uint32_t parseUint(
            const pugi::xml_node& inNode, const char* inName, std::uint32_t inFallback
        );
        CHICANE_SMOKE String parseString(const pugi::xml_node& inNode, const char* inName, const String& inFallback);
        CHICANE_SMOKE Vec3 parseVec3(const pugi::xml_node& inNode, const char* inName, const Vec3& inFallback);
        CHICANE_SMOKE Vec4 parseColor(const pugi::xml_node& inNode, const char* inName, const Vec4& inFallback);
    }
}
