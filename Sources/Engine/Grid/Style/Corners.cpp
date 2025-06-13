#include "Chicane/Grid/Style/Corners.hpp"

#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Corners::Corners(
                const String& inOnelineAttributeName,
                const String& inTopAttributeName,
                const String& inBottomAttributeName,
                const String& inLeftAttributeName,
                const String& inRightAttributeName
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
                std::function<float (const String&, Direction)> inCalculator
            )
            {
                if (inSource.empty())
                {
                    return false;
                }

                String topValue    = Style::CORNER_DEFAULT_VALUE;
                String rightValue  = Style::CORNER_DEFAULT_VALUE;
                String bottomValue = Style::CORNER_DEFAULT_VALUE;
                String leftValue   = Style::CORNER_DEFAULT_VALUE;

                if (
                    inSource.find(m_onelineAttributeName) == inSource.end() ||
                    inSource.at(m_onelineAttributeName).isEmpty()
                )
                {
                    if (inSource.find(m_topAttributeName) != inSource.end())
                    {
                        topValue = inSource.at(m_topAttributeName);
                    }

                    if (inSource.find(m_rightAttributeName) != inSource.end())
                    {
                        rightValue = inSource.at(m_rightAttributeName);
                    }

                    if (inSource.find(m_bottomAttributeName) != inSource.end())
                    {
                        bottomValue = inSource.at(m_bottomAttributeName);
                    }

                    if (inSource.find(m_leftAttributeName) != inSource.end())
                    {
                        leftValue = inSource.at(m_leftAttributeName);
                    }
                }
                else
                {
                    const std::vector<String> values = splitOneliner(
                        inSource.at(m_onelineAttributeName)
                    );
    
                    if (values.size() == 1) // SINGLE
                    {
                        const String& value = values.at(0);
        
                        topValue     = value;
                        rightValue   = value;
                        bottomValue  = value;
                        leftValue    = value;
                    }
        
                    if (values.size() == 2) // VERTICAL HORIZONTAL
                    {
                        const String& vertical   = values.at(0);
                        const String& horizontal = values.at(1);
        
                        topValue     = vertical;
                        bottomValue  = vertical;
                        rightValue   = horizontal;
                        leftValue    = horizontal;
                    }
        
                    if (values.size() == 3) // TOP BOTTOM HORIZONTAL
                    {
                        const String& horizontal = values.at(2);
        
                        topValue     = values.at(0);
                        bottomValue  = values.at(1);
                        rightValue   = horizontal;
                        leftValue    = horizontal;
                    }
        
                    if (values.size() >= 4) // TOP RIGHT BOTTOM LEFT
                    {
                        topValue     = values.at(0);
                        rightValue   = values.at(1);
                        bottomValue  = values.at(2);
                        leftValue    = values.at(3);
                    }
                }

                topValue    = topValue.trim();
                rightValue  = rightValue.trim();
                bottomValue = bottomValue.trim();
                leftValue   = leftValue.trim();

                const float lastTop = top;
                top = inCalculator(topValue, Direction::Vertical);

                const float lastRight = right;
                right = inCalculator(rightValue, Direction::Horizontal);

                const float lastBottom = bottom;
                bottom = inCalculator(bottomValue, Direction::Vertical);

                const float lastLeft = left;
                left = inCalculator(leftValue, Direction::Horizontal);

                if (leftValue.equals(Style::AUTO_SIZE_UNIT) && rightValue.equals(Style::AUTO_SIZE_UNIT))
                {
                    right *= 0.5f;
                }

                if (topValue.equals(Style::AUTO_SIZE_UNIT) && bottomValue.equals(Style::AUTO_SIZE_UNIT))
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