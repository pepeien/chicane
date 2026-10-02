#include "Chicane/Grid/Style/Ruleset.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        String StyleRuleset::sNormalizeData(const String& inValue)
        {
            String result = inValue;
            // Remove online comments
            result.regexReplace(std::regex(R"(/\*[\s\S]*?\*/)"), "");
            // Remove multiline comments
            result.regexReplace(std::regex(R"(//[^\n]*)"), "");
            // Remove white space
            result.erase('\t');
            result.erase('\n');
            result.erase('\r');

            return result;
        }

        StyleRuleset::StyleRuleset()
            : selectors({}),
              properties({}),
              compiled({})
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

            compileSelectors();
        }

        void StyleRuleset::compileSelectors()
        {
            compiled.clear();
            compiled.reserve(selectors.size());

            for (const String& selector : selectors)
            {
                StyleCompiledSelector entry;

                for (const String& rawPart : selector.split(Style::SELECTOR_SEPARATOR_SPACE))
                {
                    String part = rawPart.trim();
                    if (part.isEmpty())
                    {
                        continue;
                    }

                    StyleSelectorPart compiledPart;
                    compiledPart.status = Style::sConsumePseudoClasses(part, &compiledPart.siblings);
                    part                = part.trim();
                    if (part.isEmpty() || part.equals(Style::INCLUSIVE_SELECTOR))
                    {
                        entry.chain.push_back(compiledPart);

                        continue;
                    }

                    std::size_t cursor         = part.firstOfChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR);
                    const bool  bCursorMissing = static_cast<bool>(cursor == String::npos);

                    if (bCursorMissing)
                    {
                        compiledPart.tag = part;
                    }

                    if (!bCursorMissing)
                    {
                        if (cursor > 0)
                        {
                            compiledPart.tag = part.substr(0, cursor);
                        }

                        part = part.substr(cursor);
                    }

                    while (part.startsWithChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR))
                    {
                        const char prefix = part.at(0);

                        std::size_t next = String::npos;
                        for (std::size_t i = 1; i < part.size(); i++)
                        {
                            const char c = part.at(i);
                            if (c == Style::CLASS_SELECTOR || c == Style::ID_SELECTOR)
                            {
                                next = i;
                                break;
                            }
                        }

                        const std::size_t length = (next == String::npos) ? part.size() : next;
                        const String      token  = part.substr(0, length);

                        const bool bPrefixClass = static_cast<bool>(prefix == Style::CLASS_SELECTOR);

                        if (bPrefixClass)
                        {
                            compiledPart.classes.push_back(token);
                        }

                        const bool bPrefixId = !bPrefixClass && (prefix == Style::ID_SELECTOR);

                        if (bPrefixId)
                        {
                            compiledPart.id = token;
                        }

                        if (next == String::npos)
                        {
                            break;
                        }

                        part = part.substr(next);
                    }

                    entry.chain.push_back(compiledPart);
                }

                if (!entry.chain.empty())
                {
                    compiled.push_back(std::move(entry));
                }
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
