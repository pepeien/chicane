#include "Chicane/Grid/Style/File.hpp"

#include <regex>

namespace Chicane
{
    namespace Grid
    {
        StyleFile::StyleFile()
            : m_imports({}),
              m_variables({}),
              m_rulesets({})
        {}

        void StyleFile::parse(const FileSystem::Path& inLocation)
        {
            parse(FileSystem::readString(inLocation));
        }

        void StyleFile::parse(const String& inValue)
        {
            addImport(inValue);
            addVariable(inValue);
            addRuleset(inValue);
        }

        const StyleImport::List& StyleFile::getImports() const
        {
            return m_imports;
        }

        void StyleFile::addImport(const String& inValue)
        {
            addImport(extractImports(inValue));
        }

        void StyleFile::addImport(const StyleImport::List& inValue)
        {
            for (const StyleImport& import : inValue)
            {
                addImport(import);
            }
        }

        void StyleFile::addImport(const StyleImport& inValue)
        {
            m_imports.push_back(inValue);
        }

        const StyleFile::Variables& StyleFile::getVariables() const
        {
            return m_variables;
        }

        void StyleFile::addVariable(const String& inValue)
        {
            addVariable(extractVariables(inValue));
        }

        void StyleFile::addVariable(const Variables& inValue)
        {
            for (const auto& [name, value] : inValue)
            {
                addVariable(name, value);
            }
        }

        void StyleFile::addVariable(const String& inName, const String& inValue)
        {
            if (inName.isEmpty() || inValue.isEmpty())
            {
                return;
            }

            m_variables[inName] = inValue;
        }
        const StyleRuleset::List& StyleFile::getRulesets() const
        {
            return m_rulesets;
        }

        void StyleFile::addRuleset(const String& inValue)
        {
            addRuleset(extractRulesets(StyleRuleset::normalizeData(inValue)));
        }

        void StyleFile::addRuleset(const StyleRuleset::List& inValue)
        {
            for (const StyleRuleset& ruleset : inValue)
            {
                addRuleset(ruleset);
            }
        }

        void StyleFile::addRuleset(const StyleRuleset& inValue)
        {
            bool bWasDuplicateFound = false;
            for (StyleRuleset& ruleset : m_rulesets)
            {
                if (ruleset == inValue)
                {
                    bWasDuplicateFound = true;

                    ruleset.addProperties(inValue.properties);
                }
            }

            if (bWasDuplicateFound)
            {
                return;
            }

            m_rulesets.push_back(inValue);
        }

        StyleImport::List StyleFile::extractImports(const String& inValue)
        {
            StyleImport::List result;

            for (const String& statement : inValue.split('\n', '\r'))
            {
                const String trimmedStatement = statement.trim();

                if (!trimmedStatement.startsWith(StyleImport::KEYWORD))
                {
                    continue;
                }

                result.emplace_back(trimmedStatement);
            }

            return result;
        }

        StyleFile::Variables StyleFile::extractVariables(const String& inValue)
        {
            Variables result;

            for (const String& statement : inValue.split('\n', '\r'))
            {
                const String trimmedStatement = statement.trim();

                if (!trimmedStatement.startsWith(VARIABLE_KEYWORD))
                {
                    continue;
                }

                std::vector<String> splittedStatement = trimmedStatement.split(':');

                if (splittedStatement.size() < 2)
                {
                    continue;
                }

                String name  = splittedStatement.at(0).trim();
                String value = splittedStatement.at(1).trim();

                if (!value.endsWith(';'))
                {
                    continue;
                }

                name.popFront();
                value.popBack();

                result[name] = value;
            }

            return result;
        }

        StyleRuleset::List StyleFile::extractRulesets(const String& inValue, const String& inSelector)
        {
            StyleRuleset::List result;

            std::size_t cursor = 0;
            while (cursor < inValue.size())
            {
                while ((cursor < inValue.size()) && (inValue.at(cursor) != StyleRuleset::INCLUSIVE_SELECTOR &&
                                                     inValue.at(cursor) != StyleRuleset::ID_SELECTOR &&
                                                     inValue.at(cursor) != StyleRuleset::CLASS_SELECTOR &&
                                                     inValue.at(cursor) != StyleRuleset::SELECTOR_INHERITANCE_KEYWORD))
                {
                    cursor++;
                }

                size_t start = cursor;
                while ((cursor < inValue.size()) && (inValue.at(cursor) != StyleRuleset::OPENING_KEYWORD))
                {
                    if (inValue.at(cursor) == COMMAND_ENDING)
                    {
                        start = cursor + 1;
                    }

                    cursor++;
                }

                if (cursor >= inValue.size())
                {
                    break;
                }

                String selector = inValue.substr(start, cursor - start);

                size_t blockStart = cursor + 1;
                int    depth      = 1;

                cursor++;
                while (cursor < inValue.size() && depth > 0)
                {
                    if (inValue.at(cursor) == StyleRuleset::OPENING_KEYWORD)
                    {
                        depth++;
                    }
                    else if (inValue.at(cursor) == StyleRuleset::CLOSING_KEYWORD)
                    {
                        depth--;
                    }

                    cursor++;
                }

                size_t blockEnd = cursor - 1;

                const String blockContent = inValue.substr(blockStart, blockEnd - blockStart);

                for (const String& currentSelector : selector.split(StyleRuleset::SELECTOR_SEPARATOR_COMMA_KEYWORD))
                {
                    String trimmedSelector = currentSelector.trim();

                    String resolvedSelector;
                    if (trimmedSelector.startsWith(StyleRuleset::SELECTOR_INHERITANCE_KEYWORD))
                    {
                        resolvedSelector = inSelector;
                        resolvedSelector.append(trimmedSelector.substr(1));
                    }
                    else if (!inSelector.isEmpty())
                    {
                        resolvedSelector = inSelector;
                        resolvedSelector.append(' ');
                        resolvedSelector.append(trimmedSelector);
                    }
                    else
                    {
                        resolvedSelector = trimmedSelector;
                    }
                    resolvedSelector = resolvedSelector.trim();

                    StyleRuleset ruleset;
                    ruleset.addSelectors(resolvedSelector);
                    ruleset.addProperties(stripNestedRulesets(blockContent));
                    result.push_back(ruleset);

                    StyleRuleset::List inherited = extractRulesets(blockContent, resolvedSelector);
                    result.insert(result.end(), inherited.begin(), inherited.end());
                }
            }

            return result;
        }

        String StyleFile::stripNestedRulesets(const String& inValue)
        {
            String result;

            std::uint32_t depth = 0;
            for (char character : inValue)
            {
                if (character == StyleRuleset::OPENING_KEYWORD)
                {
                    depth++;

                    continue;
                }

                if (character == StyleRuleset::CLOSING_KEYWORD)
                {
                    depth--;

                    continue;
                }

                if (depth == 0)
                {
                    result.append(character);
                }
            }

            return result;
        }
    }
}