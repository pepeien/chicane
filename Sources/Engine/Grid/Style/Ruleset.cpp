#include "Chicane/Grid/Style/Ruleset.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        String StyleRuleset::normalizeData(const String& inValue)
        {
            String result = inValue;
            result.erase('\t');
            result.erase('\n');
            result.erase('\r');
            // Remove online comments
            result.regexReplace(std::regex(R"(/\*[\s\S]*?\*/)"), "");
            // Remove multiline comments
            result.regexReplace(std::regex(R"(//.*?$)", std::regex_constants::multiline), "");

            return result;
        }

        StyleRuleset::StyleRuleset()
            : selectors({}),
              properties({})
        {}

        bool StyleRuleset::isEmpty() const
        {
            return selectors.empty() || properties.empty();
        }

        void StyleRuleset::addSelectors(const String& inValue)
        {
            for (const String& selector : inValue.split(Style::SELECTOR_SEPARATOR))
            {
                selectors.push_back(selector.trim());
            }
        }

        void StyleRuleset::addProperties(const String& inValue)
        {
            for (const String& property : inValue.trim().split(';'))
            {
                const std::vector<String> splittedProperty = property.trim().split(':');

                if (splittedProperty.size() < 2)
                {
                    continue;
                }

                const String key   = splittedProperty.at(0).trim();
                const String value = splittedProperty.at(1).trim();

                if (key.isEmpty() || value.isEmpty())
                {
                    continue;
                }

                properties[key] = value;
            }
        }

        void StyleRuleset::addProperties(const Properties& inProperties)
        {
            for (const auto& [name, value] : inProperties)
            {
                properties[name] = value;
            }
        }
    }
}