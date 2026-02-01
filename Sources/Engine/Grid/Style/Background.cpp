#include "Chicane/Grid/Style/Background.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleBackground::StyleBackground()
            : color(Color::toRgba(Color::TEXT_COLOR_TRANSPARENT)),
              image("")
        {}

        void StyleBackground::refresh()
        {
            color.refresh();
            image.refresh();
        }

        void StyleBackground::setProperties(const StyleSource::Map& inProperties)
        {
            if (inProperties.find(Style::BACKGROUND_COLOR_ATTRIBUTE_NAME) != inProperties.end())
            {
                color.setRaw(inProperties.at(Style::BACKGROUND_COLOR_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::BACKGROUND_IMAGE_ATTRIBUTE_NAME) != inProperties.end())
            {
                image.setRaw(inProperties.at(Style::BACKGROUND_IMAGE_ATTRIBUTE_NAME));
            }
        }

        void StyleBackground::parseWith(ColorParser inColorParser, ImageParser inImageParser)
        {
            color.parseWith(inColorParser);
            image.parseWith(inImageParser);
        }
    }
}