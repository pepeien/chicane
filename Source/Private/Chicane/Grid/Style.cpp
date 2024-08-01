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

        StyleMap m_styles = {};

        void setSize(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.find(HEIGHT_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.height = getSize(inData.at(HEIGHT_ATTRIBUTE_NAME));
            }

            if (inData.find(WIDTH_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.width = getSize(inData.at(WIDTH_ATTRIBUTE_NAME));
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
                    outStyle.margin.top = getSize(inData.at(MARGIN_TOP_ATTRIBUTE_NAME));
                }

                if (inData.find(MARGIN_BOTTOM_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.bottom = getSize(inData.at(MARGIN_BOTTOM_ATTRIBUTE_NAME));
                }

                if (inData.find(MARGIN_LEFT_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.left = getSize(inData.at(MARGIN_LEFT_ATTRIBUTE_NAME));
                }

                if (inData.find(MARGIN_RIGHT_ATTRIBUTE_NAME) != inData.end())
                {
                    outStyle.margin.right = getSize(inData.at(MARGIN_RIGHT_ATTRIBUTE_NAME));
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
                float margin = getSize(splittedOneline.at(0));

                outStyle.margin.top    = margin;
                outStyle.margin.right  = margin;
                outStyle.margin.bottom = margin;
                outStyle.margin.left   = margin;
            }

            if (splittedOneline.size() == 2) // `VERTICAL_MARGIN` `HORIZONTAL_MARGIN`
            {
                float verticalMargin   = getSize(splittedOneline.at(0));
                float horizontalMargin = getSize(splittedOneline.at(1));

                outStyle.margin.top    = verticalMargin;
                outStyle.margin.bottom = verticalMargin;
                outStyle.margin.right  = horizontalMargin;
                outStyle.margin.left   = horizontalMargin;
            }

            if (splittedOneline.size() == 3) // `TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`
            {
                float topMargin        = getSize(splittedOneline.at(0));
                float bottomMargin     = getSize(splittedOneline.at(1));
                float horizontalMargin = getSize(splittedOneline.at(2));

                outStyle.margin.top    = topMargin;
                outStyle.margin.bottom = bottomMargin;
                outStyle.margin.right  = horizontalMargin;
                outStyle.margin.left   = horizontalMargin;
            }

            if (splittedOneline.size() >= 4) // `TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`
            {
                outStyle.margin.top    = getSize(splittedOneline.at(0));
                outStyle.margin.right  = getSize(splittedOneline.at(1));
                outStyle.margin.bottom = getSize(splittedOneline.at(2));
                outStyle.margin.left   = getSize(splittedOneline.at(3));
            }
        }

        Style parseStyle(const std::string& inData)
        {
            std::vector<std::string> blocks = Utils::split(
                inData,
                ';'
            );

            std::unordered_map<std::string, std::string> sourceMap {};

            for (const std::string& block : blocks)
            {
                std::vector<std::string> splittedBlock = Utils::split(
                    block,
                    ':'
                );

                const std::string& key   = Utils::trim(splittedBlock.at(0));
                const std::string& value = Utils::trim(splittedBlock.at(1));

                if (sourceMap.find(key) != sourceMap.end())
                {
                    sourceMap[key] = value;

                    continue;
                }

                sourceMap.insert(
                    std::make_pair(
                        key,
                        value
                    )
                );
            }

            Style result {};

            if (sourceMap.empty())
            {
                return result;
            }

            setSize(result,     sourceMap);
            setPosition(result, sourceMap);
            setMargin(result,   sourceMap);

            return result;
        }

        StyleMap parseStyleData(const std::string& inData)
        {
            StyleMap result {};

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
            data.erase(
                std::remove(
                    data.begin(),
                    data.end(),
                    ' '
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
                        parseStyle(source)
                    )
                );
            }

            return result;
        }

        StyleMap parseStyleData(const std::vector<char>& inData)
        {
            return parseStyleData(
                std::string(
                    inData.begin(),
                    inData.end()
                )
            );
        }

        StyleMap parseStyleFile(const std::string& inFilePath)
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

            return parseStyleData(
                FileSystem::readFile(inFilePath)
            );
        }

        Style getStyle(const pugi::xml_node& inNode)
        {
            std::string id = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();

            if (id.empty())
            {
                return {};
            }

            id = "#" + id;

            if (m_styles.empty() || m_styles.find(id) == m_styles.end())
            {
                return {};
            }

            return m_styles.at(id);
        }

        void addStyle(const std::string& inFilePath)
        {
            StyleMap styles = parseStyleFile(inFilePath);

            // TODO Implement property based overwrite
            for (const auto& [id, style] : styles)
            {
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
    }
}