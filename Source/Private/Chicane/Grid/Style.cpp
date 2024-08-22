#include "Chicane/Grid/Style.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr auto FILE_EXTENSION_NAME = "decal";

        constexpr auto STYLE_ATTRIBUTE_NAME = "style";

        constexpr auto WIDTH_ATTRIBUTE_NAME  = "width";
        constexpr auto HEIGHT_ATTRIBUTE_NAME = "height";

        constexpr auto POSITION_ATTRIBUTE_NAME = "position";
        constexpr auto POSITION_TYPE_ABSOLUTE  = "absolute";
        constexpr auto POSITION_TYPE_RELATIVE  = "relative";

        constexpr char MARGIN_SEPARATOR = ' ';
        /*
        * Template 1: "`SINGLE_MARGIN`"
        * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
        * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
        * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
        */
        constexpr auto MARGIN_ATTRIBUTE_NAME        = "margin";
        constexpr auto MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
        constexpr auto MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
        constexpr auto MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
        constexpr auto MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

        constexpr auto BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";

        const Style EMPTY_STYLE = {};

        StyleSourceMap m_sources = {};
        StyleDataMap m_styles    = {};

        void setSize(
            Style& outStyle,
            const StyleSource& inData
        )
        {
            if (inData.find(HEIGHT_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.height = getSize(
                    inData.at(HEIGHT_ATTRIBUTE_NAME),
                    Direction::Vertical
                );
            }

            if (inData.find(WIDTH_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.width = getSize(
                    inData.at(WIDTH_ATTRIBUTE_NAME),
                    Direction::Horizontal
                );
            }
        }

        void setPosition(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            outStyle.position = Position::Relative;

            if (inData.find(POSITION_ATTRIBUTE_NAME) == inData.end())
            {
                return;
            }

            std::string position = Utils::trim(inData.at(POSITION_ATTRIBUTE_NAME));

            if (position.compare(POSITION_TYPE_ABSOLUTE) == 0)
            {
                outStyle.position = Position::Absolute;
            }
        }

        float calculateMargin(
            const Style& inStyle,
            const std::string& rawValue,
            Direction inDirection
        )
        {
            float result = getSize(rawValue, inDirection);

            if (rawValue.compare(AUTO_SIZE_UNIT) == 0)
            {
                result -= inDirection == Direction::Horizontal ? inStyle.width : inStyle.height;
            }

            return result;
        }

        void setMargin(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty())
            {
                return;
            }

            if (
                inData.find(MARGIN_ATTRIBUTE_NAME) == inData.end() ||
                inData.at(MARGIN_ATTRIBUTE_NAME).empty()
            )
            {
                if (inData.find(MARGIN_TOP_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.top = calculateMargin(
                        outStyle,
                        inData.at(MARGIN_TOP_ATTRIBUTE_NAME),
                        Direction::Vertical
                    );
                }

                if (inData.find(MARGIN_BOTTOM_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.bottom = calculateMargin(
                        outStyle,
                        inData.at(MARGIN_BOTTOM_ATTRIBUTE_NAME),
                        Direction::Vertical
                    );
                }

                if (inData.find(MARGIN_LEFT_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.left = calculateMargin(
                        outStyle,
                        inData.at(MARGIN_LEFT_ATTRIBUTE_NAME),
                        Direction::Horizontal
                    );
                }

                if (inData.find(MARGIN_RIGHT_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.right =calculateMargin(
                        outStyle,
                        inData.at(MARGIN_RIGHT_ATTRIBUTE_NAME),
                        Direction::Horizontal
                    );
                }

                return;
            }

            std::string oneline = inData.at(MARGIN_ATTRIBUTE_NAME);

            std::vector<std::string> splittedOneline = Utils::split(
                Utils::trim(oneline),
                MARGIN_SEPARATOR
            );

            if (splittedOneline.size() == 1) // `SINGLE_MARGIN`
            {
                std::string margin = splittedOneline.at(0);

                float verticalMargin = calculateMargin(
                    outStyle,
                    margin,
                    Direction::Vertical
                );
                float horizontalMargin = calculateMargin(
                    outStyle,
                    margin,
                    Direction::Horizontal
                );

                outStyle.margin.top    = verticalMargin;
                outStyle.margin.right  = horizontalMargin;
                outStyle.margin.bottom = verticalMargin;
                outStyle.margin.left   = horizontalMargin;
            }

            if (splittedOneline.size() == 2) // `VERTICAL_MARGIN` `HORIZONTAL_MARGIN`
            {
                float verticalMargin = calculateMargin(
                    outStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                float horizontalMargin = calculateMargin(
                    outStyle,
                    splittedOneline.at(1),
                    Direction::Horizontal
                );

                outStyle.margin.top    = verticalMargin;
                outStyle.margin.bottom = verticalMargin;
                outStyle.margin.right  = horizontalMargin;
                outStyle.margin.left   = horizontalMargin;
            }

            if (splittedOneline.size() == 3) // `TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`
            {
                float topMargin = calculateMargin(
                    outStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                float bottomMargin = calculateMargin(
                    outStyle,
                    splittedOneline.at(1),
                    Direction::Vertical
                );
                float horizontalMargin = calculateMargin(
                    outStyle,
                    splittedOneline.at(2),
                    Direction::Horizontal
                );

                outStyle.margin.top    = topMargin;
                outStyle.margin.bottom = bottomMargin;
                outStyle.margin.right  = horizontalMargin;
                outStyle.margin.left   = horizontalMargin;
            }

            if (splittedOneline.size() >= 4) // `TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`
            {
                outStyle.margin.top = calculateMargin(
                    outStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                outStyle.margin.right = calculateMargin(
                    outStyle,
                    splittedOneline.at(1),
                    Direction::Horizontal
                );
                outStyle.margin.bottom = calculateMargin(
                    outStyle,
                    splittedOneline.at(2),
                    Direction::Vertical
                );
                outStyle.margin.left = calculateMargin(
                    outStyle,
                    splittedOneline.at(3),
                    Direction::Horizontal
                );
            }
        }

        void setBackgroundColor(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.backgroundColor = "transparent";

                return;
            }

            outStyle.backgroundColor = Utils::trim(inData.at(BACKGROUND_COLOR_ATTRIBUTE_NAME));
        }

        Style parseStyle(const StyleSource& inSource)
        {
            Style result {};

            if (inSource.empty())
            {
                return result;
            }

            setSize(result, inSource);
            setPosition(result, inSource);
            setMargin(result, inSource);
            setBackgroundColor(result, inSource);

            return result;
        }

        StyleSource extractStyleSource(const std::string& inData)
        {
            std::vector<std::string> blocks = Utils::split(
                inData,
                ';'
            );
    
            StyleSource result {};

            for (const std::string& block : blocks)
            {
                std::vector<std::string> splittedBlock = Utils::split(
                    block,
                    ':'
                );

                const std::string& key   = Utils::trim(splittedBlock.at(0));
                const std::string& value = Utils::trim(splittedBlock.at(1));

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

        StyleSourceMap extractStyleFromString(const std::string& inData)
        {
            StyleSourceMap result {};

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

            std::vector<std::string> styles = Utils::split(
                data,
                '}'
            );

            for (const std::string& style : styles)
            {
                std::vector<std::string> splittedStyle = Utils::split(
                    style,
                    '{'
                );

                std::string identifier = Utils::trim(splittedStyle.at(0));
                std::string source     = Utils::trim(splittedStyle.at(1));

                if (result.find(identifier) != result.end())
                {
                    continue;
                }

                result.insert(
                    std::make_pair(
                        identifier,
                        extractStyleSource(source)
                    )
                );
            }

            return result;
        }

        StyleSourceMap extractStyleFromFile(const std::string& inFilePath)
        {
            if (inFilePath.empty())
            {
                return {};
            }

            std::vector<std::string> splittedFilePath = Utils::split(
                inFilePath,
                '\\'
            );
            std::vector<std::string> splittedFilename = Utils::split(
                splittedFilePath.back(),
                '.'
            );
            std::string fileExtension = splittedFilename.back();

            if (fileExtension.compare(FILE_EXTENSION_NAME) != 0)
            {
                return {};
            }

            std::vector<char> data = FileSystem::readFile(inFilePath);

            return extractStyleFromString(
                std::string(
                    data.begin(),
                    data.end()
                )
            );
        }

        void addStyle(const std::string& inFilePath)
        {
            m_sources = extractStyleFromFile(inFilePath);

            // TODO Implement property based overwrite
            for (const auto& [id, source] : m_sources)
            {
                Style style = parseStyle(source);

                if (m_styles.find(id) != m_styles.end())
                {
                    m_styles[id] = style;

                    continue;
                }

                m_styles.insert(
                    std::make_pair(
                        id,
                        style
                    )
                );
            }
        }

        void addStyle(const pugi::xml_node& inNode)
        {
            std::string styleLocation = getAttribute(STYLE_ATTRIBUTE_NAME, inNode).as_string();

            addStyle(styleLocation);
        }

        const Style& getStyle(const pugi::xml_node& inNode)
        {
            std::string id = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();

            if (id.empty())
            {
                return EMPTY_STYLE;
            }

            id = "#" + id;

            if (m_styles.empty() || m_styles.find(id) == m_styles.end())
            {
                return EMPTY_STYLE;
            }

            Style& style              = m_styles.at(id);
            const StyleSource& source = m_sources.at(id);

            setSize(style, source);
            setPosition(style, source);
            setMargin(style, source);

            return style;
        }
    }
}