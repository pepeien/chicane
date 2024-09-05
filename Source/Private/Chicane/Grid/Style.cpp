#include "Chicane/Grid/Style.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr auto FILE_EXTENSION_NAME = "decal";

        constexpr auto STYLE_ATTRIBUTE_NAME = "style";

        constexpr auto DISPLAY_ATTRIBUTE_NAME = "display";

        constexpr auto WIDTH_ATTRIBUTE_NAME  = "width";
        constexpr auto HEIGHT_ATTRIBUTE_NAME = "height";

        constexpr auto POSITION_ATTRIBUTE_NAME = "position";
        constexpr auto POSITION_TYPE_ABSOLUTE  = "absolute";
        constexpr auto POSITION_TYPE_RELATIVE  = "relative";

        constexpr char ONELINE_SEPARATOR = ' ';

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

        /*
        * Template 1: "`SINGLE_GAP`"
        * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
        * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
        * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
        */
        constexpr auto GAP_ATTRIBUTE_NAME        = "gap";
        constexpr auto GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
        constexpr auto GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
        constexpr auto GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
        constexpr auto GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

        constexpr auto LIST_DIRECTION_ATTRIBUTE_NAME = "list-direction";
        constexpr auto LIST_DIRECTION_COLUMN         = "COLUMN";
        constexpr auto LIST_DIRECTION_ROW            = "ROW";

        constexpr auto FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
        constexpr auto FOREGROUND_DEFAULT_COLOR        = "#FFFFFF";

        constexpr auto BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";

        /*
        * Template 1: "`SINGLE_ALIGNMENT`"
        * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
        */
        constexpr auto ALIGNMENT_ATTRIBUTE_NAME = "alignment";
        constexpr auto ALIGNMENT_START          = "START";
        constexpr auto ALIGNMENT_CENTER         = "CENTER";
        constexpr auto ALIGNMENT_END            = "END";

        StyleSourceMap m_sources = {};
        StyleDataMap m_styles    = {};

        void setVisiblity(
            Style& outStyle,
            const StyleSource& inData
        )
        {
            if (inData.find(DISPLAY_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.display = Display::Visible;

                return;
            }

            std::string display = inData.at(DISPLAY_ATTRIBUTE_NAME);

            if (Utils::areEquals(display, "none"))
            {
                outStyle.display = Display::None;

                return;
            }

            if (Utils::areEquals(display, "hidden"))
            {
                outStyle.display = Display::Hidden;

                return;
            }

            outStyle.display = Display::Visible;
        }

        void setSize(
            Style& outStyle,
            const StyleSource& inData
        )
        {
            if (inData.find(HEIGHT_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.height = getSize(
                    inData.at(HEIGHT_ATTRIBUTE_NAME),
                    Direction::Vertical,
                    outStyle.position
                );
            }

            if (inData.find(WIDTH_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.width = getSize(
                    inData.at(WIDTH_ATTRIBUTE_NAME),
                    Direction::Horizontal,
                    outStyle.position
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

            if (Utils::areEquals(position, POSITION_TYPE_ABSOLUTE))
            {
                outStyle.position = Position::Absolute;
            }
        }

        float calculateDirectionSize(
            const Style& inStyle,
            const std::string& inRawValue,
            Direction inDirection
        )
        {
            float result = getSize(inRawValue, inDirection, inStyle.position);

            if (Utils::areEquals(inRawValue, AUTO_SIZE_UNIT))
            {
                result -= inDirection == Direction::Horizontal ? inStyle.width : inStyle.height;
            }

            return result;
        }

        void setDirectionalSize(
            DirectionalSize& outValue,
            const Style& inStyle,
            const std::unordered_map<std::string, std::string>& inData,
            const std::string& inOnelineAttributeName,
            const std::string& inTopAttributeName,
            const std::string& inBottomAttributeName,
            const std::string& inLeftAttributeName,
            const std::string& inRightAttributeName
        )
        {
            if (inData.empty())
            {
                return;
            }

            if (
                inData.find(inOnelineAttributeName) == inData.end() ||
                inData.at(inOnelineAttributeName).empty()
            )
            {
                if (inData.find(inTopAttributeName) != inData.end())
                {
                    outValue.top = calculateDirectionSize(
                        inStyle,
                        inData.at(inTopAttributeName),
                        Direction::Vertical
                    );
                }

                if (inData.find(inBottomAttributeName) != inData.end())
                {
                    outValue.bottom = calculateDirectionSize(
                        inStyle,
                        inData.at(inBottomAttributeName),
                        Direction::Vertical
                    );
                }

                if (inData.find(inLeftAttributeName) != inData.end())
                {
                    outValue.left = calculateDirectionSize(
                        inStyle,
                        inData.at(inLeftAttributeName),
                        Direction::Horizontal
                    );
                }

                if (inData.find(inRightAttributeName) != inData.end())
                {
                    outValue.right = calculateDirectionSize(
                        inStyle,
                        inData.at(inRightAttributeName),
                        Direction::Horizontal
                    );
                }

                return;
            }

            std::string oneline = inData.at(inOnelineAttributeName);

            std::vector<std::string> splittedOneline = Utils::split(
                Utils::trim(oneline),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1) // SINGLE
            {
                std::string value = splittedOneline.at(0);

                float vertical = calculateDirectionSize(
                    inStyle,
                    value,
                    Direction::Vertical
                );
                float horizontal = calculateDirectionSize(
                    inStyle,
                    value,
                    Direction::Horizontal
                );

                outValue.top    = vertical;
                outValue.right  = horizontal;
                outValue.bottom = vertical;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() == 2) // VERTICAL HORIZONTAL
            {
                float vertical = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                float horizontal = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Direction::Horizontal
                );

                outValue.top    = vertical;
                outValue.bottom = vertical;
                outValue.right  = horizontal;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() == 3) // TOP BOTTOM HORIZONTAL
            {
                float top = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                float bottom = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Direction::Vertical
                );
                float horizontal = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(2),
                    Direction::Horizontal
                );

                outValue.top    = top;
                outValue.bottom = bottom;
                outValue.right  = horizontal;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() >= 4) // TOP RIGHT BOTTOM LEFT
            {
                outValue.top = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Direction::Vertical
                );
                outValue.right = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Direction::Horizontal
                );
                outValue.bottom = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(2),
                    Direction::Vertical
                );
                outValue.left = calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(3),
                    Direction::Horizontal
                );
            }
        }

        void setMargin(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            setDirectionalSize(
                outStyle.margin,
                outStyle,
                inData,
                MARGIN_ATTRIBUTE_NAME,
                MARGIN_TOP_ATTRIBUTE_NAME,
                MARGIN_BOTTOM_ATTRIBUTE_NAME,
                MARGIN_LEFT_ATTRIBUTE_NAME,
                MARGIN_RIGHT_ATTRIBUTE_NAME
            );
        }

        void setGap(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            setDirectionalSize(
                outStyle.gap,
                outStyle,
                inData,
                GAP_ATTRIBUTE_NAME,
                GAP_TOP_ATTRIBUTE_NAME,
                GAP_BOTTOM_ATTRIBUTE_NAME,
                GAP_LEFT_ATTRIBUTE_NAME,
                GAP_RIGHT_ATTRIBUTE_NAME
            );
        }

        void setListDirection(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(LIST_DIRECTION_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.listDirection = ListDirection::Row;

                return;
            }

            std::string listDirection = Utils::trim(inData.at(LIST_DIRECTION_ATTRIBUTE_NAME));
            std::transform(
                listDirection.begin(),
                listDirection.end(),
                listDirection.begin(),
                ::toupper
            );

            if (Utils::areEquals(listDirection, LIST_DIRECTION_COLUMN))
            {
                outStyle.listDirection = ListDirection::Column;

                return;
            }

            outStyle.listDirection = ListDirection::Row;
        }

        void setForegroundColor(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.backgroundColor = FOREGROUND_DEFAULT_COLOR;

                return;
            }

            outStyle.foregroundColor = Utils::trim(inData.at(FOREGROUND_COLOR_ATTRIBUTE_NAME));
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

        Alignment getAlignment(const std::string& inValue)
        {
            if (inValue.empty())
            {
                return Alignment::Start;
            }

            std::string value = Utils::trim(inValue);
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                ::toupper
            );

            if (Utils::areEquals(value, ALIGNMENT_CENTER))
            {
                return Alignment::Center;
            }

            if (Utils::areEquals(value, ALIGNMENT_END))
            {
                return Alignment::End;
            }

            return Alignment::Start;
        }

        void setAlignment(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(ALIGNMENT_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.horizontalAlignment = Alignment::Start;
                outStyle.verticalAlignment   = Alignment::Start;

                return;
            }

            std::vector<std::string> splittedOneline = Utils::split(
                Utils::trim(inData.at(ALIGNMENT_ATTRIBUTE_NAME)),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1)
            {
                outStyle.horizontalAlignment = getAlignment(splittedOneline.at(0));
                outStyle.verticalAlignment   = outStyle.horizontalAlignment;

                return;
            }

            outStyle.horizontalAlignment = getAlignment(splittedOneline.at(0));
            outStyle.verticalAlignment   = getAlignment(splittedOneline.at(1));
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
            setGap(result, inSource);
            setListDirection(result, inSource);
            setForegroundColor(result, inSource);
            setBackgroundColor(result, inSource);
            setAlignment(result, inSource);

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

                std::string key   = Utils::trim(splittedBlock.at(0));
                std::string value = Utils::trim(splittedBlock.at(1));
                std::transform(
                    value.begin(),
                    value.end(),
                    value.begin(),
                    ::tolower
                );

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
                FileSystem::SEPARATOR
            );
            std::vector<std::string> splittedFilename = Utils::split(
                splittedFilePath.back(),
                '.'
            );
            std::string fileExtension = splittedFilename.back();

            if (!Utils::areEquals(fileExtension, FILE_EXTENSION_NAME))
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

            setVisiblity(style, source);

            if (style.display != Display::Visible)
            {
                return style;
            }

            setSize(style, source);
            setPosition(style, source);
            setMargin(style, source);
            setGap(style, source);

            return style;
        }
    }
}