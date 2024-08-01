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

        typedef std::unordered_map<std::string, Style> StyleMap;

        StyleMap parseStyleData(const std::string& inRawData);
        StyleMap parseStyleData(const std::vector<char>& inRawData);
        StyleMap parseStyleFile(const std::string& inFilePath);
        Style getStyle(const pugi::xml_node& inNode);

        void addStyle(const pugi::xml_node& inNode);
    }
}