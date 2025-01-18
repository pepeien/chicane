#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Grid
    {
        static constexpr const char* BACKGROUND_COLOR_TRANSPARENT = "transparent";
        static constexpr const char* HEX_COLOR_TRANSPARENT        = "#00000000";

        struct Style
        {
        public:
            enum class Alignment : std::uint8_t
            {
                Start,
                Center,
                End
            };

            enum class Direction : std::uint8_t
            {
                Vertical,
                Horizontal
            };

            enum class Display : std::uint8_t
            {
                Visible, // Visible and occupies space on the overall layout
                Hidden,  // Visibly hidden but still occupies space on the overall layout
                None     // Visibly hidden and doens't occupiy space on the overall layout
            };

            enum class ListDirection : std::uint8_t
            {
                Column,
                Row
            };

            enum class Position : std::uint8_t
            {
                Absolute, // Start from (0, 0) a.k.a Top Left corner
                Relative // Continue from the current ImGui's cursor position
            };

            struct DirectionalSize
            {
            public:
                float top    = 0.0f;
                float bottom = 0.0f;
                float left   = 0.0f;
                float right  = 0.0f;
            };

            typedef std::unordered_map<std::string, std::string> Source;
            typedef std::unordered_map<std::string, Source> Sources;
            typedef std::unordered_map<std::string, Style> Styles;

        public:
            static const Style& empty();

            static Sources extractStyleFromString(const std::string& inRawData);
            static Sources extractStyleFromFile(const std::string& inFilePath);

            static void addStyle(const pugi::xml_node& inNode);
            static const Style& getStyle(const pugi::xml_node& inNode);

        public:
            // Visiblity
            Display         display              = Display::Visible;

            // Size
            float           height               = 0.0f;
            float           width                = 0.0f;

            // Positioning
            Position        position             = Position::Relative;
            Alignment       horizontalAlignment  = Alignment::Start;
            Alignment       verticalAlignment    = Alignment::Start;
            DirectionalSize gap                  = {};
            DirectionalSize margin               = {};

            // Layout
            ListDirection   listDirection        = ListDirection::Column;

            // Color
            std::string     backgroundColor      = BACKGROUND_COLOR_TRANSPARENT;
            std::string     foregroundColor      = "#FFFFFF";
        };
    }
}