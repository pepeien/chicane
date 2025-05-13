#include "Chicane/Grid/Style/Instance.hpp"

#include "Chicane/Grid/Essential.hpp"

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
                        XML::getAttribute(ATTRIBUTE_NAME, inNode).as_string()
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
                updateDisplay(inSource);
                updateSize(inSource);
                updateFlex(inSource);
                updatePosition(inSource);
                updateMargin(inSource);
                updateGap(inSource);
                updateAlignment(inSource);
                updateForegroundColor(inSource);
                updateBackgroundColor(inSource);
            }

            void Instance::updateDisplay(const Properties &inSource)
            {
                if (inSource.find(DISPLAY_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                display = String::trim(inSource.at(DISPLAY_ATTRIBUTE_NAME));
            }

            void Instance::updateSize(const Properties &inSource)
            {
                if (inSource.find(HEIGHT_ATTRIBUTE_NAME) != inSource.end())
                {
                    height = String::trim(inSource.at(HEIGHT_ATTRIBUTE_NAME));
                }

                if (inSource.find(WIDTH_ATTRIBUTE_NAME) != inSource.end())
                {
                    width = String::trim(inSource.at(WIDTH_ATTRIBUTE_NAME));
                }
            }

            void Instance::updateFlex(const Properties &inSource)
            {
                if (inSource.find(FLEX_DIRECTION_ATTRIBUTE_NAME) != inSource.end())
                {
                    flex.direction = String::trim(inSource.at(FLEX_DIRECTION_ATTRIBUTE_NAME));
                }
            }

            void Instance::updatePosition(const Properties &inSource)
            {
                if (inSource.find(POSITION_ATTRIBUTE_NAME) == inSource.end())
                {
                    return;
                }

                position = String::trim(inSource.at(POSITION_ATTRIBUTE_NAME));
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
                    align   = splittedOneline.at(0);
                    justify = align;
                }
                else
                {
                    align   = splittedOneline.at(0);
                    justify = splittedOneline.at(1);
                }

                align   = String::trim(align);
                justify = String::trim(justify);
            }

            void Instance::updateMargin(const Properties &inSource)
            {
                margin.update(
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
                gap.update(
                    inSource,
                    GAP_ATTRIBUTE_NAME,
                    GAP_TOP_ATTRIBUTE_NAME,
                    GAP_BOTTOM_ATTRIBUTE_NAME,
                    GAP_LEFT_ATTRIBUTE_NAME,
                    GAP_RIGHT_ATTRIBUTE_NAME
                );
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
        }
    }
}