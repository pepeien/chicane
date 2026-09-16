#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Range.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CHICANE_SMOKE float randomFloat(float inFrom, float inTo);
        CHICANE_SMOKE float randomFloat();
        CHICANE_SMOKE Vec3 randomDirection();

        CHICANE_SMOKE Range parseRange(const String& inValue, const Range& inFallback);
        CHICANE_SMOKE Range parseRange(const XmlNode& inNode, const char* inName, const Range& inFallback);
    }
}
