#include "Chicane/Grid/Style/Size.hpp"

#include <algorithm>

#include "Chicane/Core/Size.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleSize::StyleSize()
            : value(0.0f),
              min(0.0f),
              max(0.0f)
        {}

        void StyleSize::refresh()
        {
            value.refresh();
            min.refresh();
            max.refresh();
        }

        void StyleSize::setProperties(
            const StyleRuleset::Properties& inProperties,
            const String&                   inValueName,
            const String&                   inMinName,
            const String&                   inMaxName
        )
        {
            if (inProperties.find(inValueName) != inProperties.end())
            {
                value.setRaw(inProperties.at(inValueName));
            }
            else
            {
                value.setRaw(Size::AUTO_KEYWORD);
            }

            if (inProperties.find(inMinName) != inProperties.end())
            {
                min.setRaw(inProperties.at(inMinName));
            }

            if (inProperties.find(inMaxName) != inProperties.end())
            {
                max.setRaw(inProperties.at(inMaxName));
            }
        }

        void StyleSize::parseWith(Parser inValueParser, Parser inLimitParser)
        {
            value.parseWith(inValueParser);
            min.parseWith(inLimitParser);
            max.parseWith(inLimitParser);
        }

        void StyleSize::copyValue(const StyleSize& inOther)
        {
            value.copyValue(inOther.value);
            min.copyValue(inOther.min);
            max.copyValue(inOther.max);
        }

        bool StyleSize::isAuto() const
        {
            return value.getRaw().isEmpty() || value.isRaw(Size::AUTO_KEYWORD);
        }

        bool StyleSize::hasMin() const
        {
            const String& raw = min.getRaw();

            return !raw.isEmpty() && !raw.equals(Size::AUTO_KEYWORD) && !raw.equals(Style::SIZE_LIMIT_TYPE_NONE);
        }

        bool StyleSize::hasMax() const
        {
            const String& raw = max.getRaw();

            return !raw.isEmpty() && !raw.equals(Size::AUTO_KEYWORD) && !raw.equals(Style::SIZE_LIMIT_TYPE_NONE);
        }

        void StyleSize::clamp(float& outValue) const
        {
            if (hasMin())
            {
                outValue = std::max(outValue, std::max(0.0f, min.get()));
            }

            if (hasMax())
            {
                outValue = std::min(outValue, std::max(0.0f, max.get()));
            }

            if (hasMin())
            {
                outValue = std::max(outValue, std::max(0.0f, min.get()));
            }
        }
    }
}
