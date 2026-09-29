#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Background/Gradient/Stop.hpp"
#include "Chicane/Grid/Style/Background/Gradient/Type.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleGradient
        {
        public:
            using ColorParser = std::function<Color::Rgba(const String&)>;
            using List        = std::vector<StyleGradient>;

            static constexpr inline std::uint32_t MAX_STOPS = 8;

        public:
            static bool sIsDeclaration(const String& inValue);
            static bool sIsActive(const List& inLayers);
            static StyleGradient sParse(const String& inValue, const ColorParser& inParseColor);
            static List sParseList(const String& inValue, const ColorParser& inParseColor);

        public:
            bool isActive() const;

        public:
            StyleGradientType              type = StyleGradientType::None;
            Vec2                           axis = Vec2(0.0f, 1.0f);
            std::vector<StyleGradientStop> stops;
        };
    }
}
