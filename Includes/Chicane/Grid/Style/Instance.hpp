#pragma once

#include "Core.hpp"
#include "Grid/Style/Alignment.hpp"
#include "Grid/Style/Corners.hpp"
#include "Grid/Style/Direction.hpp"
#include "Grid/Style/Display.hpp"
#include "Grid/Style/Essential.hpp"
#include "Grid/Style/Flex.hpp"
#include "Grid/Style/Position.hpp"
#include "Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE Instance
            {
            public:
                static Sources parseSources(const pugi::xml_node& inNode);
                static Sources parseSources(const std::filesystem::path& inPath);
                static Sources parseSources(const std::string& inData);

                static Properties parseSource(const std::string& inData);

            public:
                void update(const Properties& inSource);

            private:
                void updateDisplay(const Properties& inSource);
                void updateSize(const Properties& inSource);
                void updateFlex(const Properties& inSource);
                void updatePosition(const Properties& inSource);
                void updateAlignment(const Properties& inSource);
                void updateMargin(const Properties& inSource);
                void updateGap(const Properties& inSource);
                void updateForegroundColor(const Properties& inSource);
                void updateBackgroundColor(const Properties& inSource);

            public:
                // Visiblity
                std::string display         = DISPLAY_TYPE_VISIBLE;

                // Size
                std::string height          = "100%";
                std::string width           = "100%";

                // Flex
                Flex        flex            =  {};

                // Positioning
                std::string position        = POSITION_TYPE_RELATIVE;
                std::string align           = ALIGNMENT_TYPE_START;
                std::string justify         = ALIGNMENT_TYPE_START;
                Corners     gap             = {};
                Corners     margin          = {};

                // Color
                std::string backgroundColor = TEXT_COLOR_TRANSPARENT;
                std::string foregroundColor = TEXT_COLOR_WHITE;
            };
        }

        typedef std::unordered_map<std::string, Style::Instance> Styles;
    }
}