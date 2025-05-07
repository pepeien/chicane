#include "Grid/Style/Corners.hpp"

#include "Grid/Essential.hpp"
#include "Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            void Corners::update(
                const Properties &inSource,
                const std::string &inOnelineAttributeName,
                const std::string &inTopAttributeName,
                const std::string &inBottomAttributeName,
                const std::string &inLeftAttributeName,
                const std::string &inRightAttributeName
            )
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
                        top = inSource.at(inTopAttributeName);
                    }

                    if (inSource.find(inBottomAttributeName) != inSource.end())
                    {
                        bottom = inSource.at(inBottomAttributeName);
                    }

                    if (inSource.find(inLeftAttributeName) != inSource.end())
                    {
                        left = inSource.at(inLeftAttributeName);
                    }

                    if (inSource.find(inRightAttributeName) != inSource.end())
                    {
                        right = inSource.at(inRightAttributeName);
                    }
                }
                else
                {
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
    
                        top    = value;
                        right  = value;
                        bottom = value;
                        left   = value;
                    }
    
                    if (splittedOneline.size() == 2) // VERTICAL HORIZONTAL
                    {
                        const std::string& vertical   = splittedOneline.at(0);
                        const std::string& horizontal = splittedOneline.at(1);
    
                        top    = vertical;
                        bottom = vertical;
                        right  = horizontal;
                        left   = horizontal;
                    }
    
                    if (splittedOneline.size() == 3) // TOP BOTTOM HORIZONTAL
                    {
                        const std::string& horizontal = splittedOneline.at(2);
    
                        top    = splittedOneline.at(0);
                        bottom = splittedOneline.at(1);
                        right  = horizontal;
                        left   = horizontal;
                    }
    
                    if (splittedOneline.size() >= 4) // TOP RIGHT BOTTOM LEFT
                    {
                        top    = splittedOneline.at(0);
                        right  = splittedOneline.at(1);
                        bottom = splittedOneline.at(2);
                        left   = splittedOneline.at(3);
                    }
                }

                top    = String::trim(top);
                right  = String::trim(right);
                bottom = String::trim(bottom);
                left   = String::trim(left);
            }
        }
    }
}