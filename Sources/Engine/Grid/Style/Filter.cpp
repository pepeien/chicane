#include "Chicane/Grid/Style/Filter.hpp"

#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleFilter::StyleFilter()
            : blur(0.0f)
        {}

        void StyleFilter::refresh()
        {
            blur.refresh();
        }

        void StyleFilter::setProperties(const StyleRuleset::Properties& inProperties, const String& inAttributeName)
        {
            if (inProperties.find(inAttributeName) != inProperties.end())
            {
                blur.setRaw(inProperties.at(inAttributeName));
            }
        }

        void StyleFilter::parseWith(BlurParser inBlurParser)
        {
            blur.parseWith(inBlurParser);
        }
    }
}
