#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        static constexpr const char* BACKGROUND_COLOR_TRANSPARENT = "transparent";
        static constexpr const char* HEX_COLOR_TRANSPARENT        = "#00000000";

        struct CHICANE Style
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

            struct CHICANE DirectionalSize
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
            Style(const pugi::xml_node& inNode);
            Style(const std::filesystem::path& inPath);
            Style(const std::string& inData);
            Style(const Source& inSource);
            Style();

        public:
            void update();

        private:
            void parseData(const std::string& inData);

            void updateVisibility();
            void updateSize();
            void updatePosition();
            void updateMargin();
            void updateGap();
            void updateAlignment();
            void updateForegroundColor();
            void updateBackgroundColor();
            void updateListDirection();

            float getDirectionalSize(const std::string& inValue, Direction inDirection);
            void updateDirectionalSize(
                DirectionalSize& outValue,
                const std::string& inOnelineAttributeName,
                const std::string& inTopAttributeName,
                const std::string& inBottomAttributeName,
                const std::string& inLeftAttributeName,
                const std::string& inRightAttributeName
            );

            Alignment getAlignment(const std::string& inValue);

        public:
            // Visiblity
            Display         display;

            // Size
            float           height;
            float           width;

            // Positioning
            Position        position;
            Alignment       horizontalAlignment;
            Alignment       verticalAlignment;
            DirectionalSize gap;
            DirectionalSize margin;

            // Layout
            ListDirection   listDirection;

            // Color
            std::string     backgroundColor;
            std::string     foregroundColor;

        private:
            Source m_source = {};
        };
    }
}