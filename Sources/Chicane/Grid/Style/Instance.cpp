#include "Grid/Style/Instance.hpp"

#include "Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Sources Instance::parseSources(const pugi::xml_node &inNode)
            {
                return Instance::parseSources(
                    std::filesystem::path(
                        XML::getAttribute(STYLE_ATTRIBUTE_NAME, inNode).as_string()
                    )
                );
            }

            Sources Instance::parseSources(const std::filesystem::path &inFilePath)
            {
                if (inFilePath.empty())
                {
                    return {};
                }

                std::vector<std::string> splittedFilePath = String::split(
                    inFilePath.string(),
                    FileSystem::SEPARATOR
                );
                std::vector<std::string> splittedFilename = String::split(
                    splittedFilePath.back(),
                    '.'
                );
                std::string fileExtension = splittedFilename.back();

                if (!String::areEquals(fileExtension, FILE_EXTENSION_NAME))
                {
                    return {};
                }

                std::vector<char> data = FileSystem::readFile(inFilePath);

                return Instance::parseSources(std::string(data.begin(), data.end()));
            }

            Sources Instance::parseSources(const std::string &inData)
            {
                std::string data = inData;
                data.erase(
                    std::remove(
                        data.begin(),
                        data.end(),
                        '\n'
                    ),
                    data.cend()
                );
                data.erase(
                    std::remove(
                        data.begin(),
                        data.end(),
                        '\r'
                    ),
                    data.cend()
                );

                std::vector<std::string> styles = String::split(
                    data,
                    '}'
                );

                Sources result = {};

                for (const std::string &style : styles)
                {
                    std::vector<std::string> splittedStyle = String::split(style, '{');

                    Source source = {};
                    source.selectors  = String::split(splittedStyle.at(0), SELECTOR_SEPARATOR);
                    source.properties = Instance::parseSource(String::trim(splittedStyle.at(1)));

                    result.push_back(source);
                }

                return result;
            }

            Properties Instance::parseSource(const std::string &inData)
            {
                std::vector<std::string> blocks = String::split(
                    inData,
                    ';'
                );

                Properties result = {};

                for (const std::string &block : blocks)
                {
                    std::vector<std::string> splittedBlock = String::split(
                        block,
                        ':'
                    );

                    std::string key = String::trim(splittedBlock.at(0));
                    std::string value = String::trim(splittedBlock.at(1));

                    if (result.find(key) != result.end())
                    {
                        result[key] = value;

                        continue;
                    }

                    result.insert(
                        std::make_pair(
                            key,
                            value
                        )
                    );
                }

                return result;
            }

            void Instance::update(const Properties &inSource)
            {
                updateVisibility(inSource);
                updateSize(inSource);
                updatePosition(inSource);
                updateMargin(inSource);
                updateGap(inSource);
                updateAlignment(inSource);
                updateForegroundColor(inSource);
                updateBackgroundColor(inSource);
                updateListDirection(inSource);
            }

            void Instance::updateVisibility(const Properties &inSource)
            {
                if (inSource.find(DISPLAY_ATTRIBUTE_NAME) == inSource.end())
                {
                    display = Display::Visible;

                    return;
                }

                std::string rawDisplay = inSource.at(DISPLAY_ATTRIBUTE_NAME);

                if (String::areEquals(rawDisplay, "none"))
                {
                    display = Display::None;

                    return;
                }

                if (String::areEquals(rawDisplay, "hidden"))
                {
                    display = Display::Hidden;

                    return;
                }

                display = Display::Visible;
            }

            void Instance::updateSize(const Properties &inSource)
            {
                if (inSource.find(HEIGHT_ATTRIBUTE_NAME) != inSource.end())
                {
                    height = inSource.at(HEIGHT_ATTRIBUTE_NAME);
                }

                if (inSource.find(WIDTH_ATTRIBUTE_NAME) != inSource.end())
                {
                    width = inSource.at(WIDTH_ATTRIBUTE_NAME);
                }
            }

            void Instance::updatePosition(const Properties &inSource)
            {
                if (inSource.find(POSITION_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                std::string rawPosition = String::trim(inSource.at(POSITION_ATTRIBUTE_NAME));

                if (String::areEquals(rawPosition, POSITION_TYPE_ABSOLUTE))
                {
                    position = Position::Absolute;
                }
            }

            void Instance::updateMargin(const Properties &inSource)
            {
                updateDirectionalSize(
                    margin,
                    inSource,
                    MARGIN_ATTRIBUTE_NAME,
                    MARGIN_TOP_ATTRIBUTE_NAME,
                    MARGIN_BOTTOM_ATTRIBUTE_NAME,
                    MARGIN_LEFT_ATTRIBUTE_NAME,
                    MARGIN_RIGHT_ATTRIBUTE_NAME
                );
            }

            void Instance::updateGap(const Properties &inSource)
            {
                updateDirectionalSize(
                    gap,
                    inSource,
                    GAP_ATTRIBUTE_NAME,
                    GAP_TOP_ATTRIBUTE_NAME,
                    GAP_BOTTOM_ATTRIBUTE_NAME,
                    GAP_LEFT_ATTRIBUTE_NAME,
                    GAP_RIGHT_ATTRIBUTE_NAME
                );
            }

            void Instance::updateAlignment(const Properties &inSource)
            {
                if (inSource.find(ALIGNMENT_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                std::vector<std::string> splittedOneline = String::split(
                    String::trim(inSource.at(ALIGNMENT_ATTRIBUTE_NAME)),
                    ONELINE_SEPARATOR
                );

                if (splittedOneline.size() == 1)
                {
                    horizontalAlignment = getAlignment(splittedOneline.at(0));
                    verticalAlignment = horizontalAlignment;

                    return;
                }

                horizontalAlignment = getAlignment(splittedOneline.at(0));
                verticalAlignment = getAlignment(splittedOneline.at(1));
            }

            void Instance::updateForegroundColor(const Properties &inSource)
            {
                if (inSource.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                foregroundColor = String::trim(inSource.at(FOREGROUND_COLOR_ATTRIBUTE_NAME));
            }

            void Instance::updateBackgroundColor(const Properties &inSource)
            {
                if (inSource.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                backgroundColor = String::trim(inSource.at(BACKGROUND_COLOR_ATTRIBUTE_NAME));
            }

            void Instance::updateListDirection(const Properties &inSource)
            {
                if (inSource.find(LIST_DIRECTION_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                std::string listDirectionRaw = String::trim(inSource.at(LIST_DIRECTION_ATTRIBUTE_NAME));
                std::transform(
                    listDirectionRaw.begin(),
                    listDirectionRaw.end(),
                    listDirectionRaw.begin(),
                    ::toupper);

                if (String::areEquals(listDirectionRaw, LIST_DIRECTION_COLUMN))
                {
                    listDirection = ListDirection::Column;

                    return;
                }

                listDirection = ListDirection::Row;
            }

            void Instance::updateDirectionalSize(
                DirectionalSize &outValue,
                const Properties &inSource,
                const std::string &inOnelineAttributeName,
                const std::string &inTopAttributeName,
                const std::string &inBottomAttributeName,
                const std::string &inLeftAttributeName,
                const std::string &inRightAttributeName)
            {
                if (inSource.empty())
                {
                    return;
                }

                if (
                    inSource.find(inOnelineAttributeName) == inSource.end() ||
                    inSource.at(inOnelineAttributeName).empty()
                )
                {
                    if (inSource.find(inTopAttributeName) != inSource.end())
                    {
                        outValue.top = inSource.at(inTopAttributeName);
                    }

                    if (inSource.find(inBottomAttributeName) != inSource.end())
                    {
                        outValue.bottom = inSource.at(inBottomAttributeName);
                    }

                    if (inSource.find(inLeftAttributeName) != inSource.end())
                    {
                        outValue.left = inSource.at(inLeftAttributeName);
                    }

                    if (inSource.find(inRightAttributeName) != inSource.end())
                    {
                        outValue.right = inSource.at(inRightAttributeName);
                    }

                    return;
                }

                const std::string& oneline = inSource.at(inOnelineAttributeName);

                std::vector<std::string> splittedOneline = String::split(
                    inSource.at(inOnelineAttributeName),
                    ONELINE_SEPARATOR
                );

                if (String::contains(oneline, CALCULATION_KEYWORD))
                {
                    const std::uint32_t calculationStart = std::distance(
                        splittedOneline.begin(),
                        std::find_if(
                            splittedOneline.begin(),
                            splittedOneline.end(),
                            [](const std::string& value)
                            {
                                return String::contains(value, CALCULATION_KEYWORD);
                            }
                        )
                    );
                    const std::uint32_t calculationEnd = std::distance(
                        std::find_if(
                            splittedOneline.rbegin(),
                            splittedOneline.rend(),
                            [](const std::string& value)
                            {
                                return String::contains(value, FUNCTION_PARAMS_CLOSING);
                            }
                        ),
                        splittedOneline.rend()
                    );

                    std::vector<std::string> replacement = {};

                    for (std::uint32_t i = 0; i < calculationStart && i < splittedOneline.size(); i++)
                    {
                        replacement.push_back(splittedOneline.at(i));
                    }

                    replacement.push_back(
                        String::join(
                            splittedOneline,
                            "",
                            calculationStart,
                            calculationEnd - 1
                        )
                    );

                    for (std::uint32_t i = calculationEnd; i < splittedOneline.size(); i++)
                    {
                        replacement.push_back(splittedOneline.at(i));
                    }

                    splittedOneline = replacement;
                }

                if (splittedOneline.size() == 1) // SINGLE
                {
                    std::string value = splittedOneline.at(0);

                    outValue.top    = value;
                    outValue.right  = value;
                    outValue.bottom = value;
                    outValue.left   = value;
                }

                if (splittedOneline.size() == 2) // VERTICAL HORIZONTAL
                {
                    const std::string& vertical   = splittedOneline.at(0);
                    const std::string& horizontal = splittedOneline.at(1);

                    outValue.top    = vertical;
                    outValue.bottom = vertical;
                    outValue.right  = horizontal;
                    outValue.left   = horizontal;
                }

                if (splittedOneline.size() == 3) // TOP BOTTOM HORIZONTAL
                {
                    const std::string& top        = splittedOneline.at(0);
                    const std::string& bottom     = splittedOneline.at(1);
                    const std::string& horizontal = splittedOneline.at(2);

                    outValue.top    = top;
                    outValue.bottom = bottom;
                    outValue.right  = horizontal;
                    outValue.left   = horizontal;
                }

                if (splittedOneline.size() >= 4) // TOP RIGHT BOTTOM LEFT
                {
                    outValue.top    = splittedOneline.at(0);
                    outValue.right  = splittedOneline.at(1);
                    outValue.bottom = splittedOneline.at(2);
                    outValue.left   = splittedOneline.at(3);
                }
            }

            Alignment Instance::getAlignment(const std::string &inValue)
            {
                if (inValue.empty())
                {
                    return Alignment::Start;
                }

                std::string value = String::trim(inValue);
                std::transform(
                    value.begin(),
                    value.end(),
                    value.begin(),
                    ::toupper
                );

                if (String::areEquals(value, ALIGNMENT_CENTER))
                {
                    return Alignment::Center;
                }

                if (String::areEquals(value, ALIGNMENT_END))
                {
                    return Alignment::End;
                }

                return Alignment::Start;
            }
        }
    }
}