#pragma once

#include "Chicane/Grid/Style/Alignment.hpp"
#include "Chicane/Grid/Style/Corners.hpp"
#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Display.hpp"
#include "Chicane/Grid/Style/Essential.hpp"
#include "Chicane/Grid/Style/Flex.hpp"
#include "Chicane/Grid/Style/Font.hpp"
#include "Chicane/Grid/Style/Position.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        namespace Style
        {
            struct CHICANE_GRID Instance : public Changeable
            {
            public:
                static Source::List parseSources(const pugi::xml_node& inNode);
                static Source::List parseSources(const FileSystem::Path& inPath);
                static Source::List parseSources(const String& inData);

                static Properties parseSource(const String& inData);

            public:
                Instance(const Properties& inProperties, Component* inParent);
                Instance();

            public:
                bool isDisplay(Display inValue) const;
                bool isPosition(Position inValue) const;

                bool hasProperties() const;
                void setProperties(const Properties& inProperties);

                bool hasParent() const;
                void setParent(Component* inComponent);

                void refresh();

            private:
                void refreshDisplay();
                void refreshSize();
                void refreshPosition();
                void refreshAlignment();
                void refreshMargin();
                void refreshForegroundColor();
                void refreshBackgroundColor();
                void refreshFont();

                Color::Rgba parseColor(const String& inValue) const;
                float parseSize(const String& inValue, Direction inDirection) const;
                String parseText(const String& inValue) const;

                float parseCalculation(const String& inValue, Style::Direction inDirection) const;

                float parsePercentage(const String& inValue, Style::Direction inDirection) const;
                float parsePercentage(float inValue, Style::Direction inDirection) const;

                float parseViewportHeight(const String& inValue) const;
                float parseViewportHeight(float inValue) const;

                float parseViewportWidth(const String& inValue) const;
                float parseViewportWidth(float inValue) const;

                float parsePixel(const String& inValue) const;

                float parseNumber(const String& inValue, const String& inUnit) const;
                float parseNumber(const String& inValue) const;

            public:
                // Visiblity
                Display     display;

                // Size
                float       width;
                float       height;

                // Flex
                Flex        flex;

                // Positioning
                Position    position;
                Alignment   align;
                Alignment   justify;
                Corners     margin;

                // Color
                Color::Rgba backgroundColor;
                Color::Rgba foregroundColor;

                // Text
                Font        font;

            private:
                Properties m_properties;

                Component* m_parent;
            };
        }
    }
}