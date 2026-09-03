#include "Chicane/Grid/Style/File.hpp"

#include <algorithm>
#include <cstdlib>
#include <regex>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleFile::StyleFile()
            : m_imports({}),
              m_variables({}),
              m_rulesets({}),
              m_keyframes({})
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

        const String& StyleFile::getVariable(const String& inName) const
        {
            if (m_variables.find(inName) == m_variables.end())
            {
                return String::empty();
            }

            return m_variables.at(inName);
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
            if (inValue.selectors.size() == 1)
            {
                String name;
                float  offset = 0.0f;

                if (parseKeyframesSelector(inValue.selectors.at(0), name, offset))
                {
                    if (!name.isEmpty() && !inValue.properties.empty())
                    {
                        StyleKeyframe keyframe;
                        keyframe.offset     = offset;
                        keyframe.properties = inValue.properties;

                        StyleKeyframe::List& frames = m_keyframes[name];
                        frames.push_back(keyframe);

                        std::sort(
                            frames.begin(),
                            frames.end(),
                            [](const StyleKeyframe& inLeft, const StyleKeyframe& inRight)
                            { return inLeft.offset < inRight.offset; }
                        );
                    }

                    return;
                }
            }

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

            StyleRuleset ruleset = inValue;
            if (ruleset.compiled.empty())
            {
                ruleset.compileSelectors();
            }

            m_rulesets.push_back(ruleset);
        }

        bool StyleFile::hasKeyframes(const String& inName) const
        {
            return m_keyframes.find(inName) != m_keyframes.end();
        }

        const StyleKeyframe::List& StyleFile::getKeyframes(const String& inName) const
        {
            const auto found = m_keyframes.find(inName);

            if (found == m_keyframes.end())
            {
                static const StyleKeyframe::List empty;

                return empty;
            }

            return found->second;
        }

        bool StyleFile::parseKeyframesSelector(const String& inSelector, String& outName, float& outOffset) const
        {
            const String selector = inSelector.trim();

            if (!selector.startsWith(Style::KEYFRAMES_KEYWORD))
            {
                return false;
            }

            const String keyword = Style::KEYFRAMES_KEYWORD;
            const String rest    = selector.substr(keyword.size()).trim();

            if (rest.isEmpty())
            {
                return true;
            }

            const std::vector<String> tokens = splitOneliner(rest);

            if (tokens.empty())
            {
                return true;
            }

            outOffset = -1.0f;

            const String last = tokens.back().trim().toLower();

            if (last.equals(Style::KEYFRAMES_FROM_KEYWORD))
            {
                outOffset = 0.0f;
            }
            else if (last.equals(Style::KEYFRAMES_TO_KEYWORD))
            {
                outOffset = 1.0f;
            }
            else if (last.endsWith("%"))
            {
                const String number = last.substr(0, last.size() - 1).trim();

                if (!number.isEmpty() && !number.isNaN())
                {
                    outOffset = static_cast<float>(std::strtod(number.toChar(), nullptr) / 100.0);
                }
            }

            const std::size_t nameCount = outOffset < 0.0f ? tokens.size() : tokens.size() - 1;

            outName = "";

            for (std::size_t i = 0; i < nameCount; i++)
            {
                if (!outName.isEmpty())
                {
                    outName.append(' ');
                }

                outName.append(tokens.at(i));
            }

            outName = outName.trim();

            return true;
        }

        StyleImport::List StyleFile::extractImports(const String& inValue)
        {
            StyleImport::List result;

            for (const String& statement : inValue.split('\n', '\r'))
            {
                const String trimmedStatement = statement.trim();

                if (!trimmedStatement.startsWith(Style::IMPORT_KEYWORD))
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
            int       depth = 0;

            for (const String& statement : inValue.split('\n', '\r'))
            {
                const String trimmedStatement = statement.trim();

                if (depth == 0 && trimmedStatement.startsWith(Style::VARIABLE_KEYWORD))
                {
                    std::vector<String> splittedStatement = trimmedStatement.split(':');

                    if (splittedStatement.size() >= 2)
                    {
                        String name  = splittedStatement.at(0).trim();
                        String value = splittedStatement.at(1).trim();

                        if (value.endsWith(';'))
                        {
                            name.popFront();
                            value.popBack();

                            result[name] = value;
                        }
                    }
                }

                for (char character : statement)
                {
                    if (character == Style::RULESET_OPENING)
                    {
                        depth++;
                    }
                    else if (character == Style::RULESET_CLOSING && depth > 0)
                    {
                        depth--;
                    }
                }
            }

            return result;
        }

        StyleRuleset::List StyleFile::extractRulesets(const String& inValue, const String& inSelector)
        {
            StyleRuleset::List result;

            const size_t size   = inValue.size();
            size_t       cursor = 0;

            while (cursor < size)
            {
                while (cursor < size && std::isspace(inValue.at(cursor)))
                {
                    cursor++;
                }

                if (cursor >= size)
                {
                    break;
                }

                const std::size_t start = cursor;

                const std::size_t nextBrace = inValue.firstOf(Style::RULESET_OPENING, cursor);
                const std::size_t nextSemi  = inValue.firstOf(Style::COMMAND_ENDING, cursor);

                if (nextBrace == String::npos)
                {
                    break;
                }

                if (nextSemi != String::npos && nextSemi < nextBrace)
                {
                    cursor = nextSemi + 1;

                    continue;
                }

                const String selector = inValue.substr(start, nextBrace - start).trim();

                cursor = nextBrace + 1;

                const std::size_t blockStart = cursor;
                int               depth      = 1;

                while (cursor < size && depth > 0)
                {
                    if (inValue.at(cursor) == Style::RULESET_OPENING)
                    {
                        depth++;
                    }
                    else if (inValue.at(cursor) == Style::RULESET_CLOSING)
                    {
                        depth--;
                    }

                    cursor++;
                }

                if (depth != 0)
                {
                    break;
                }

                const std::size_t blockEnd = cursor - 1;

                const String blockContent = inValue.substr(blockStart, blockEnd - blockStart);

                for (const String& currentSelector : selector.split(Style::SELECTOR_SEPARATOR_COMMA))
                {
                    String trimmedSelector = currentSelector.trim();
                    if (trimmedSelector.isEmpty())
                    {
                        continue;
                    }

                    String resolvedSelector;

                    if (trimmedSelector.startsWith(Style::SELECTOR_INHERITANCE))
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

                    const StyleRuleset::List inherited = extractRulesets(blockContent, resolvedSelector);
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
                if (character == Style::RULESET_OPENING)
                {
                    depth++;

                    continue;
                }

                if (character == Style::RULESET_CLOSING)
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