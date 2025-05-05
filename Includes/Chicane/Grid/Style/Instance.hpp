#pragma once

#include "Core.hpp"
#include "Grid/Style/Alignment.hpp"
#include "Grid/Style/Direction.hpp"
#include "Grid/Style/DirectionalSize.hpp"
#include "Grid/Style/Display.hpp"
#include "Grid/Style/Essential.hpp"
#include "Grid/Style/ListDirection.hpp"
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
                void updateVisibility(const Properties& inSource);
                void updateSize(const Properties& inSource);
                void updatePosition(const Properties& inSource);
                void updateMargin(const Properties& inSource);
                void updateGap(const Properties& inSource);
                void updateAlignment(const Properties& inSource);
                void updateForegroundColor(const Properties& inSource);
                void updateBackgroundColor(const Properties& inSource);
                void updateListDirection(const Properties& inSource);

                void updateDirectionalSize(
                    DirectionalSize& outValue,
                    const Properties& inSource,
                    const std::string& inOnelineAttributeName,
                    const std::string& inTopAttributeName,
                    const std::string& inBottomAttributeName,
                    const std::string& inLeftAttributeName,
                    const std::string& inRightAttributeName
                );

                Alignment getAlignment(const std::string& inValue);

            public:
                // Visiblity
                Display         display             = Display::Visible;

                // Size
                std::string     height              = "100%";
                std::string     width               = "100%";

                // Positioning
                Position        position            = Position::Relative;
                Alignment       horizontalAlignment = Alignment::Start;
                Alignment       verticalAlignment   = Alignment::Start;
                DirectionalSize gap                 = {};
                DirectionalSize margin              = {};

                // Layout
                ListDirection   listDirection       = ListDirection::Column;

                // Color
                std::string     backgroundColor     = BACKGROUND_COLOR_TRANSPARENT;
                std::string     foregroundColor     = "#FFFFFF";
            };
        }

        typedef std::unordered_map<std::string, Style::Instance> Styles;
    }
}