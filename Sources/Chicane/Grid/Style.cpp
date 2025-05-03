#include "Grid/Style.hpp"

#include "Grid/Essential.hpp"

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

        Style::Style(const pugi::xml_node& inNode)
            : Style(
                std::filesystem::path(
                    XML::getAttribute(STYLE_ATTRIBUTE_NAME, inNode).as_string()
                )
            )
        {}

        Style::Style(const std::filesystem::path& inPath)
        {
            if (inPath.empty())
            {
                return;
            }

            std::vector<std::string> splittedFilePath = String::split(
                inPath.string(),
                FileSystem::SEPARATOR
            );
            std::vector<std::string> splittedFilename = String::split(
                splittedFilePath.back(),
                '.'
            );
            std::string fileExtension = splittedFilename.back();

            if (!String::areEquals(fileExtension, FILE_EXTENSION_NAME))
            {
                return;
            }

            std::vector<char> data = FileSystem::readFile(inPath.string());

            parseData(
                std::string(
                    data.begin(),
                    data.end()
                )
            );
        }

        Style::Style(const std::string& inData)
        {
            parseData(inData);
        }

        Style::Style(const Source& inSource)
            : m_source(inSource)
        {
            update();
        }

        Style::Style()
            : display(Display::Visible),
            height(0.0f),
            width(0.0f),
            position(Position::Relative),
            horizontalAlignment(Alignment::Start),
            verticalAlignment(Alignment::Start),
            gap({}),
            margin({}),
            listDirection(ListDirection::Column),
            backgroundColor(BACKGROUND_COLOR_TRANSPARENT),
            foregroundColor("#FFFFFF"),
            m_source({})
        {}
    
        void Style::update()
        {
            updateVisibility();
            updateSize();
            updatePosition();
            updateMargin();
            updateGap();
            updateAlignment();
            updateForegroundColor();
            updateBackgroundColor();
            updateListDirection();
        }

        void Style::parseData(const std::string& inData)
        {
            m_source.clear();

            std::vector<std::string> blocks = String::split(
                inData,
                ';'
            );

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

                if (m_source.find(key) != m_source.end())
                {
                    m_source[key] = value;

                    continue;
                }

                m_source.insert(
                    std::make_pair(
                        key,
                        value
                    )
                );
            }

            update();
        }

        void Style::updateVisibility()
        {
            if (m_source.find(DISPLAY_ATTRIBUTE_NAME) == m_source.end())
            {
                display = Style::Display::Visible;

                return;
            }

            std::string rawDisplay = m_source.at(DISPLAY_ATTRIBUTE_NAME);

            if (String::areEquals(rawDisplay, "none"))
            {
                display = Style::Display::None;

                return;
            }

            if (String::areEquals(rawDisplay, "hidden"))
            {
                display = Style::Display::Hidden;

                return;
            }

            display = Style::Display::Visible;
        }

        void Style::updateSize()
        {
            if (m_source.find(HEIGHT_ATTRIBUTE_NAME) != m_source.end())
            {
                height = getSize(
                    m_source.at(HEIGHT_ATTRIBUTE_NAME),
                    Style::Direction::Vertical,
                    position
                );
            }

            if (m_source.find(WIDTH_ATTRIBUTE_NAME) != m_source.end())
            {
                width = getSize(
                    m_source.at(WIDTH_ATTRIBUTE_NAME),
                    Style::Direction::Horizontal,
                    position
                );
            }
        }

        void Style::updatePosition()
        {
            position = Style::Position::Relative;

            if (m_source.find(POSITION_ATTRIBUTE_NAME) == m_source.end())
            {
                return;
            }

            std::string rawPosition = String::trim(m_source.at(POSITION_ATTRIBUTE_NAME));

            if (String::areEquals(rawPosition, POSITION_TYPE_ABSOLUTE))
            {
                position = Style::Position::Absolute;
            }
        }

        void Style::updateMargin()
        {
            updateDirectionalSize(
                margin,
                MARGIN_ATTRIBUTE_NAME,
                MARGIN_TOP_ATTRIBUTE_NAME,
                MARGIN_BOTTOM_ATTRIBUTE_NAME,
                MARGIN_LEFT_ATTRIBUTE_NAME,
                MARGIN_RIGHT_ATTRIBUTE_NAME
            );
        }

        void Style::updateGap()
        {
            updateDirectionalSize(
                gap,
                GAP_ATTRIBUTE_NAME,
                GAP_TOP_ATTRIBUTE_NAME,
                GAP_BOTTOM_ATTRIBUTE_NAME,
                GAP_LEFT_ATTRIBUTE_NAME,
                GAP_RIGHT_ATTRIBUTE_NAME
            );
        }

        void Style::updateAlignment()
        {
            if (m_source.empty() || m_source.find(ALIGNMENT_ATTRIBUTE_NAME) == m_source.end())
            {
                horizontalAlignment = Style::Alignment::Start;
                verticalAlignment   = Style::Alignment::Start;

                return;
            }

            std::vector<std::string> splittedOneline = String::split(
                String::trim(m_source.at(ALIGNMENT_ATTRIBUTE_NAME)),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1)
            {
                horizontalAlignment = getAlignment(splittedOneline.at(0));
                verticalAlignment   = horizontalAlignment;

                return;
            }

            horizontalAlignment = getAlignment(splittedOneline.at(0));
            verticalAlignment   = getAlignment(splittedOneline.at(1));
        }

        void Style::updateForegroundColor()
        {
            if (m_source.empty() || m_source.find(FOREGROUND_COLOR_ATTRIBUTE_NAME) == m_source.end())
            {
                backgroundColor = FOREGROUND_DEFAULT_COLOR;

                return;
            }

            foregroundColor = String::trim(m_source.at(FOREGROUND_COLOR_ATTRIBUTE_NAME));
        }

        void Style::updateBackgroundColor()
        {
            if (m_source.empty() || m_source.find(BACKGROUND_COLOR_ATTRIBUTE_NAME) == m_source.end())
            {
                backgroundColor = BACKGROUND_COLOR_TRANSPARENT;

                return;
            }

            backgroundColor = String::trim(m_source.at(BACKGROUND_COLOR_ATTRIBUTE_NAME));
        }

        void Style::updateListDirection()
        {
            if (m_source.empty() || m_source.find(LIST_DIRECTION_ATTRIBUTE_NAME) == m_source.end())
            {
                listDirection = Style::ListDirection::Row;

                return;
            }

            std::string listDirectionRaw = String::trim(m_source.at(LIST_DIRECTION_ATTRIBUTE_NAME));
            std::transform(
                listDirectionRaw.begin(),
                listDirectionRaw.end(),
                listDirectionRaw.begin(),
                ::toupper
            );

            if (String::areEquals(listDirectionRaw, LIST_DIRECTION_COLUMN))
            {
                listDirection = Style::ListDirection::Column;

                return;
            }

            listDirection = Style::ListDirection::Row;
        }

        float Style::getDirectionalSize(const std::string& inValue, Style::Direction inDirection)
        {
            float result = getSize(inValue, inDirection, position);

            if (String::areEquals(inValue, AUTO_SIZE_UNIT))
            {
                result -= inDirection == Style::Direction::Horizontal ? width : height;
            }

            return result;
        }

        void Style::updateDirectionalSize(
            Style::DirectionalSize& outValue,
            const std::string& inOnelineAttributeName,
            const std::string& inTopAttributeName,
            const std::string& inBottomAttributeName,
            const std::string& inLeftAttributeName,
            const std::string& inRightAttributeName
        )
        {
            if (m_source.empty())
            {
                return;
            }

            if (
                m_source.find(inOnelineAttributeName) == m_source.end() ||
                m_source.at(inOnelineAttributeName).empty()
            )
            {
                if (m_source.find(inTopAttributeName) != m_source.end())
                {
                    outValue.top = getDirectionalSize(
                        m_source.at(inTopAttributeName),
                        Style::Direction::Vertical
                    );
                }

                if (m_source.find(inBottomAttributeName) != m_source.end())
                {
                    outValue.bottom = getDirectionalSize(
                        m_source.at(inBottomAttributeName),
                        Style::Direction::Vertical
                    );
                }

                if (m_source.find(inLeftAttributeName) != m_source.end())
                {
                    outValue.left = getDirectionalSize(
                        m_source.at(inLeftAttributeName),
                        Style::Direction::Horizontal
                    );
                }

                if (m_source.find(inRightAttributeName) != m_source.end())
                {
                    outValue.right = getDirectionalSize(
                        m_source.at(inRightAttributeName),
                        Style::Direction::Horizontal
                    );
                }

                return;
            }

            std::string oneline = m_source.at(inOnelineAttributeName);

            std::vector<std::string> splittedOneline = String::split(
                String::trim(oneline),
                ONELINE_SEPARATOR
            );

            if (splittedOneline.size() == 1) // SINGLE
            {
                std::string value = splittedOneline.at(0);

                float vertical = getDirectionalSize(
                    value,
                    Style::Direction::Vertical
                );
                float horizontal = getDirectionalSize(
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
                float vertical = getDirectionalSize(
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                float horizontal = getDirectionalSize(
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
                float top = getDirectionalSize(
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                float bottom = getDirectionalSize(
                    splittedOneline.at(1),
                    Style::Direction::Vertical
                );
                float horizontal = getDirectionalSize(
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
                outValue.top = getDirectionalSize(
                    splittedOneline.at(0),
                    Style::Direction::Vertical
                );
                outValue.right = getDirectionalSize(
                    splittedOneline.at(1),
                    Style::Direction::Horizontal
                );
                outValue.bottom = getDirectionalSize(
                    splittedOneline.at(2),
                    Style::Direction::Vertical
                );
                outValue.left = getDirectionalSize(
                    splittedOneline.at(3),
                    Style::Direction::Horizontal
                );
            }
        }

        Style::Alignment Style::getAlignment(const std::string& inValue)
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
    }
}