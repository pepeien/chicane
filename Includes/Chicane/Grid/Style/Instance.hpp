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
                static Sources parseSources(const pugi::xml_node& inNode);
                static Sources parseSources(const std::filesystem::path& inPath);
                static Sources parseSources(const std::string& inData);

                static Properties parseSource(const std::string& inData);

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

                Color::Rgba parseColor(const std::string& inValue) const;
                float parseSize(const std::string& inValue, Direction inDirection) const;
                std::string parseText(const std::string& inValue) const;

                float parseCalculation(const std::string& inValue, Style::Direction inDirection) const;

                float parsePercentage(const std::string& inValue, Style::Direction inDirection) const;
                float parsePercentage(float inValue, Style::Direction inDirection) const;

                float parseViewportHeight(const std::string& inValue) const;
                float parseViewportHeight(float inValue) const;

                float parseViewportWidth(const std::string& inValue) const;
                float parseViewportWidth(float inValue) const;

                float parsePixel(const std::string& inValue) const;

                float parseNumber(const std::string& inValue, const std::string& inUnit) const;
                float parseNumber(const std::string& inValue) const;

                template<typename T>
                void setProperty(T& outProperty, T inNewValue)
                {
                    if (outProperty == inNewValue)
                    {
                        return;
                    }

                    outProperty = inNewValue;

                    emmitChanges();
                }

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