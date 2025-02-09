#include "Chicane/Grid/Style.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

const Chicane::Grid::Style EMPTY_STYLE = {};

namespace Chicane
{
    namespace Grid
    {
        static constexpr const char* FILE_EXTENSION_NAME = "decal";

        static constexpr const char* STYLE_ATTRIBUTE_NAME = "style";

        static constexpr const char* DISPLAY_ATTRIBUTE_NAME = "display";

        static constexpr const char* WIDTH_ATTRIBUTE_NAME  = "width";
        static constexpr const char* HEIGHT_ATTRIBUTE_NAME = "height";

        static constexpr const char* POSITION_ATTRIBUTE_NAME = "position";
        static constexpr const char* POSITION_TYPE_ABSOLUTE  = "absolute";
        static constexpr const char* POSITION_TYPE_RELATIVE  = "relative";

        constexpr char ONELINE_SEPARATOR = ' ';

        /*
        * Template 1: "`SINGLE_MARGIN`"
        * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
        * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
        * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
        */
        static constexpr const char* MARGIN_ATTRIBUTE_NAME        = "margin";
        static constexpr const char* MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
        static constexpr const char* MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
        static constexpr const char* MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
        static constexpr const char* MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

        /*
        * Template 1: "`SINGLE_GAP`"
        * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
        * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
        * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
        */
        static constexpr const char* GAP_ATTRIBUTE_NAME        = "gap";
        static constexpr const char* GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
        static constexpr const char* GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
        static constexpr const char* GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
        static constexpr const char* GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

        static constexpr const char* LIST_DIRECTION_ATTRIBUTE_NAME = "list-direction";
        static constexpr const char* LIST_DIRECTION_COLUMN         = "COLUMN";
        static constexpr const char* LIST_DIRECTION_ROW            = "ROW";

        static constexpr const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
        static constexpr const char* FOREGROUND_DEFAULT_COLOR        = "#FFFFFF";

        static constexpr const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";

        /*
        * Template 1: "`SINGLE_ALIGNMENT`"
        * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
        */
        static constexpr const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
        static constexpr const char* ALIGNMENT_START          = "START";
        static constexpr const char* ALIGNMENT_CENTER         = "CENTER";
        static constexpr const char* ALIGNMENT_END            = "END";

        Style::Sources m_sources = {};
        Style::Styles m_styles   = {};

        static const Style::Sources EMPTY_SOURCE_MAP = {};

        void _setVisiblity(
            Style& outStyle,
            const Style::Source& inData
        )
        {
            if (inData.find(DISPLAY_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.display = Style::Display::Visible;

                return;
            }

            std::string display = inData.at(DISPLAY_ATTRIBUTE_NAME);

            if (String::areEquals(display, "none"))
            {
                outStyle.display = Style::Display::None;

                return;
            }

            if (String::areEquals(display, "hidden"))
            {
                outStyle.display = Style::Display::Hidden;

                return;
            }

            outStyle.display = Style::Display::Visible;
        }

        void _setSize(
            Style& outStyle,
            const Style::Source& inData
        )
        {
            if (inData.find(HEIGHT_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.height = getSize(
                    inData.at(HEIGHT_ATTRIBUTE_NAME),
                    Style::Direction::Vertical,
                    outStyle.position
                );
            }

            if (inData.find(WIDTH_ATTRIBUTE_NAME) != inData.end())
            {
                outStyle.width = getSize(
                    inData.at(WIDTH_ATTRIBUTE_NAME),
                    Style::Direction::Horizontal,
                    outStyle.position
                );
            }
        }

        void _setPosition(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            outStyle.position = Style::Position::Relative;

            if (inData.find(POSITION_ATTRIBUTE_NAME) == inData.end())
            {
                return;
            }

            std::string position = String::trim(inData.at(POSITION_ATTRIBUTE_NAME));

            if (String::areEquals(position, POSITION_TYPE_ABSOLUTE))
            {
                outStyle.position = Style::Position::Absolute;
            }
        }

        float _calculateDirectionSize(
            const Style& inStyle,
            const std::string& inRawValue,
            Style::Direction inDirection
        )
        {
            float result = getSize(inRawValue, inDirection, inStyle.position);

            if (String::areEquals(inRawValue, AUTO_SIZE_UNIT))
            {
                result -= inDirection == Style::Direction::Horizontal ? inStyle.width : inStyle.height;
            }

            return result;
        }

        void _setDirectionalSize(
            Style::DirectionalSize& outValue,
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
                    outValue.top = _calculateDirectionSize(
                        inStyle,
                        inData.at(inTopAttributeName),
                        Style::Direction::Vertical
                    );
                }

                if (inData.find(inBottomAttributeName) != inData.end())
                {
                    outValue.bottom = _calculateDirectionSize(
                        inStyle,
                        inData.at(inBottomAttributeName),
                        Style::Direction::Vertical
                    );
                }

                if (inData.find(inLeftAttributeName) != inData.end())
                {
                    outValue.left = _calculateDirectionSize(
                        inStyle,
                        inData.at(inLeftAttributeName),
                        Style::Direction::Horizontal
                    );
                }

                if (inData.find(inRightAttributeName) != inData.end())
                {
                    outValue.right = _calculateDirectionSize(
                        inStyle,
                        inData.at(inRightAttributeName),
                        Style::Direction::Horizontal
                    );
                }

                return;
            }

            std::string oneline = inData.at(inOnelineAttributeName);

            std::vector<std::string> splittedOneline = String::split(
                String::trim(oneline),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1) // SINGLE
            {
                std::string value = splittedOneline.at(0);

                float vertical = _calculateDirectionSize(
                    inStyle,
                    value,
                    Style::Direction::Vertical
                );
                float horizontal = _calculateDirectionSize(
                    inStyle,
                    value,
                    Style::Direction::Horizontal
                );

                outValue.top    = vertical;
                outValue.right  = horizontal;
                outValue.bottom = vertical;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() == 2) // VERTICAL HORIZONTAL
            {
                float vertical = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                float horizontal = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Style::Direction::Horizontal
                );

                outValue.top    = vertical;
                outValue.bottom = vertical;
                outValue.right  = horizontal;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() == 3) // TOP BOTTOM HORIZONTAL
            {
                float top = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                float bottom = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Style::Direction::Vertical
                );
                float horizontal = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(2),
                    Style::Direction::Horizontal
                );

                outValue.top    = top;
                outValue.bottom = bottom;
                outValue.right  = horizontal;
                outValue.left   = horizontal;
            }

            if (splittedOneline.size() >= 4) // TOP RIGHT BOTTOM LEFT
            {
                outValue.top = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                outValue.right = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(1),
                    Style::Direction::Horizontal
                );
                outValue.bottom = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(2),
                    Style::Direction::Vertical
                );
                outValue.left = _calculateDirectionSize(
                    inStyle,
                    splittedOneline.at(3),
                    Style::Direction::Horizontal
                );
            }
        }

        void _setMargin(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            _setDirectionalSize(
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

        void _setGap(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            _setDirectionalSize(
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

        void _setListDirection(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(LIST_DIRECTION_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.listDirection = Style::ListDirection::Row;

                return;
            }

            std::string listDirection = String::trim(inData.at(LIST_DIRECTION_ATTRIBUTE_NAME));
            std::transform(
                listDirection.begin(),
                listDirection.end(),
                listDirection.begin(),
                ::toupper
            );

            if (String::areEquals(listDirection, LIST_DIRECTION_COLUMN))
            {
                outStyle.listDirection = Style::ListDirection::Column;

                return;
            }

            outStyle.listDirection = Style::ListDirection::Row;
        }

        void _setForegroundColor(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.backgroundColor = FOREGROUND_DEFAULT_COLOR;

                return;
            }

            outStyle.foregroundColor = String::trim(inData.at(FOREGROUND_COLOR_ATTRIBUTE_NAME));
        }

        void _setBackgroundColor(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.backgroundColor = BACKGROUND_COLOR_TRANSPARENT;

                return;
            }

            outStyle.backgroundColor = String::trim(inData.at(BACKGROUND_COLOR_ATTRIBUTE_NAME));
        }

        Style::Alignment _getAlignment(const std::string& inValue)
        {
            if (inValue.empty())
            {
                return Style::Alignment::Start;
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
                return Style::Alignment::Center;
            }

            if (String::areEquals(value, ALIGNMENT_END))
            {
                return Style::Alignment::End;
            }

            return Style::Alignment::Start;
        }

        void _setAlignment(
            Style& outStyle,
            const std::unordered_map<std::string, std::string>& inData
        )
        {
            if (inData.empty() || inData.find(ALIGNMENT_ATTRIBUTE_NAME) == inData.end())
            {
                outStyle.horizontalAlignment = Style::Alignment::Start;
                outStyle.verticalAlignment   = Style::Alignment::Start;

                return;
            }

            std::vector<std::string> splittedOneline = String::split(
                String::trim(inData.at(ALIGNMENT_ATTRIBUTE_NAME)),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1)
            {
                outStyle.horizontalAlignment = _getAlignment(splittedOneline.at(0));
                outStyle.verticalAlignment   = outStyle.horizontalAlignment;

                return;
            }

            outStyle.horizontalAlignment = _getAlignment(splittedOneline.at(0));
            outStyle.verticalAlignment   = _getAlignment(splittedOneline.at(1));
        }

        Style _parseStyle(const Style::Source& inSource)
        {
            Style result = {};

            if (inSource.empty())
            {
                return result;
            }

            _setVisiblity(result, inSource);
            _setSize(result, inSource);
            _setPosition(result, inSource);
            _setMargin(result, inSource);
            _setGap(result, inSource);
            _setListDirection(result, inSource);
            _setForegroundColor(result, inSource);
            _setBackgroundColor(result, inSource);
            _setAlignment(result, inSource);

            return result;
        }

        void _addStyle(const std::string& inFilePath)
        {
            m_sources = Style::extractStyleFromFile(inFilePath);

            // TODO Implement property based overwrite
            for (const auto& [id, source] : m_sources)
            {
                Style style = _parseStyle(source);

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

        const Style& Style::empty()
        {
            return EMPTY_STYLE;
        }

        Style::Source extractStyleSource(const std::string& inData)
        {
            std::vector<std::string> blocks = String::split(
                inData,
                ';'
            );
    
            Style::Source result = {};

            for (const std::string& block : blocks)
            {
                std::vector<std::string> splittedBlock = String::split(
                    block,
                    ':'
                );

                std::string key   = String::trim(splittedBlock.at(0));
                std::string value = String::trim(splittedBlock.at(1));
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

        Style::Sources Style::extractStyleFromString(const std::string& inData)
        {
            Style::Sources result = {};

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

            for (const std::string& style : styles)
            {
                std::vector<std::string> splittedStyle = String::split(
                    style,
                    '{'
                );

                std::string identifiers = String::trim(splittedStyle.at(0));
                std::string source      = String::trim(splittedStyle.at(1));

                for (std::string rawIdentifier : String::split(identifiers, ','))
                {
                    std::string identifier = String::trim(rawIdentifier);

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
            }

            return result;
        }

        Style::Sources Style::extractStyleFromFile(const std::string& inFilePath)
        {
            if (inFilePath.empty())
            {
                return EMPTY_SOURCE_MAP;
            }

            std::vector<std::string> splittedFilePath = String::split(
                inFilePath,
                FileSystem::SEPARATOR
            );
            std::vector<std::string> splittedFilename = String::split(
                splittedFilePath.back(),
                '.'
            );
            std::string fileExtension = splittedFilename.back();

            if (!String::areEquals(fileExtension, FILE_EXTENSION_NAME))
            {
                return EMPTY_SOURCE_MAP;
            }

            std::vector<char> data = FileSystem::readFile(inFilePath);

            return extractStyleFromString(
                std::string(
                    data.begin(),
                    data.end()
                )
            );
        }

        void Style::addStyle(const pugi::xml_node& inNode)
        {
            std::string styleLocation = getAttribute(STYLE_ATTRIBUTE_NAME, inNode).as_string();

            _addStyle(styleLocation);
        }

        const Style& Style::getStyle(const pugi::xml_node& inNode)
        {
            std::string id = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();

            if (id.empty())
            {
                return Style::empty();
            }

            id = "#" + id;

            if (m_styles.empty() || m_styles.find(id) == m_styles.end())
            {
                return Style::empty();
            }

            Style& style                = m_styles.at(id);
            const Style::Source& source = m_sources.at(id);

            _setVisiblity(style, source);

            if (style.display != Style::Display::Visible)
            {
                return style;
            }

            _setSize(style, source);
            _setPosition(style, source);
            _setMargin(style, source);
            _setGap(style, source);

            return style;
        }
    }
}