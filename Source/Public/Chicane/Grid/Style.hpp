#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class Position : std::uint8_t
        {
            Absolute,
            Relative
        };

        struct Margin
        {
        public:
            float top    = 0.0f;
            float bottom = 0.0f;
            float left   = 0.0f;
            float right  = 0.0f;
        };

        struct Style
        {
        public:
            // Size
            float height = 0.0f;
            float width  = 0.0f;

            // Positioning
            Position position = Position::Relative;
            Margin margin     = {};

            // Looks
            std::string backgroundColor = "";
        };

        constexpr auto BACKGROUND_COLOR_TRANSPARENT = "transparent";

        typedef std::unordered_map<std::string, std::string> StyleSource;

        typedef std::unordered_map<std::string, StyleSource> StyleSourceMap;
        typedef std::unordered_map<std::string, Style> StyleDataMap;

        StyleSourceMap extractStyleFromString(const std::string& inRawData);
        StyleSourceMap extractStyleFromFile(const std::string& inFilePath);

        void addStyle(const pugi::xml_node& inNode);
        const Style& getStyle(const pugi::xml_node& inNode);
    }
}