#include "Chicane/Grid/Style/Corners.hpp"

#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Corners::Corners(
                const std::string& inOnelineAttributeName,
                const std::string& inTopAttributeName,
                const std::string& inBottomAttributeName,
                const std::string& inLeftAttributeName,
                const std::string& inRightAttributeName
            )
                : top(0.0f),
                bottom(0.0f),
                left(0.0f),
                right(0.0f),
                m_onelineAttributeName(inOnelineAttributeName),
                m_topAttributeName(inTopAttributeName),
                m_bottomAttributeName(inBottomAttributeName),
                m_leftAttributeName(inLeftAttributeName),
                m_rightAttributeName(inRightAttributeName)
            {}

            bool Corners::refresh(
                const Properties &inSource,
                std::function<float (const std::string&, Direction)> inCalculator
            )
            {
                if (inSource.empty())
                {
                    return false;
                }

                std::string topValue    = Style::CORNER_DEFAULT_VALUE;
                std::string rightValue  = Style::CORNER_DEFAULT_VALUE;
                std::string bottomValue = Style::CORNER_DEFAULT_VALUE;
                std::string leftValue   = Style::CORNER_DEFAULT_VALUE;

                if (
                    inSource.find(m_onelineAttributeName) == inSource.end() ||
                    inSource.at(m_onelineAttributeName).empty()
                )
                {
                    if (inSource.find(m_topAttributeName) != inSource.end())
                    {
                        topValue  = String::trim(inSource.at(m_topAttributeName));
                    }

                    if (inSource.find(m_rightAttributeName) != inSource.end())
                    {
                        rightValue  = String::trim(inSource.at(m_rightAttributeName));
                    }

                    if (inSource.find(m_bottomAttributeName) != inSource.end())
                    {
                        bottomValue  = String::trim(inSource.at(m_bottomAttributeName));
                    }

                    if (inSource.find(m_leftAttributeName) != inSource.end())
                    {
                        leftValue  = String::trim(inSource.at(m_leftAttributeName));
                    }
                }
                else
                {
                    const std::vector<std::string> values = splitOneliner(
                        inSource.at(m_onelineAttributeName)
                    );
    
                    if (values.size() == 1) // SINGLE
                    {
                        std::string value = String::trim(values.at(0));
        
                        topValue     = value;
                        rightValue   = value;
                        bottomValue  = value;
                        leftValue    = value;
                    }
        
                    if (values.size() == 2) // VERTICAL HORIZONTAL
                    {
                        const std::string& vertical   = String::trim(values.at(0));
                        const std::string& horizontal = String::trim(values.at(1));
        
                        topValue     = vertical;
                        bottomValue  = vertical;
                        rightValue   = horizontal;
                        leftValue    = horizontal;
                    }
        
                    if (values.size() == 3) // TOP BOTTOM HORIZONTAL
                    {
                        const std::string& horizontal = String::trim(values.at(2));
        
                        topValue     = String::trim(values.at(0));
                        bottomValue  = String::trim(values.at(1));
                        rightValue   = horizontal;
                        leftValue    = horizontal;
                    }
        
                    if (values.size() >= 4) // TOP RIGHT BOTTOM LEFT
                    {
                        topValue     = String::trim(values.at(0));
                        rightValue   = String::trim(values.at(1));
                        bottomValue  = String::trim(values.at(2));
                        leftValue    = String::trim(values.at(3));
                    }
                }

                const float lastTop = top;
                top = inCalculator(topValue, Direction::Vertical);

                const float lastRight = right;
                right = inCalculator(rightValue, Direction::Horizontal);

                const float lastBottom = bottom;
                bottom = inCalculator(bottomValue, Direction::Vertical);

                const float lastLeft = left;
                left = inCalculator(leftValue, Direction::Horizontal);

                if (
                    String::areEquals(leftValue, Style::AUTO_SIZE_UNIT) &&
                    String::areEquals(rightValue, Style::AUTO_SIZE_UNIT)
                )
                {
                    right *= 0.5f;
                }

                if (
                    String::areEquals(topValue, Style::AUTO_SIZE_UNIT) &&
                    String::areEquals(bottomValue, Style::AUTO_SIZE_UNIT)
                )
                {
                    bottom *= 0.5f;
                }

                if (
                    (lastTop    != top) ||
                    (lastRight  != right) ||
                    (lastBottom != bottom) ||
                    (lastLeft   != left)
                )
                {
                    return true;
                }

                return false;
            }
        }
    }
}