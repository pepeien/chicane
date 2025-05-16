#include "Chicane/Grid/Style/Corners.hpp"

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
                        top = String::trim(inSource.at(inTopAttributeName));
                    }

                    if (inSource.find(inBottomAttributeName) != inSource.end())
                    {
                        bottom = String::trim(inSource.at(inBottomAttributeName));
                    }

                    if (inSource.find(inLeftAttributeName) != inSource.end())
                    {
                        left = String::trim(inSource.at(inLeftAttributeName));
                    }

                    if (inSource.find(inRightAttributeName) != inSource.end())
                    {
                        right = String::trim(inSource.at(inRightAttributeName));
                    }

                    return;
                }

                const std::string& oneline = inSource.at(inOnelineAttributeName);
    
                std::vector<std::string> splittedOneline = {};

                std::uint32_t start = 0;
                std::uint32_t end   = 0;

                std::uint32_t parathesisCount = 0;

                for (std::uint32_t i = 0; i < oneline.size(); i++)
                {
                    const char character = oneline.at(i);

                    end++;

                    if (character == FUNCTION_PARAMS_OPENING)
                    {
                        parathesisCount++;

                        continue;
                    }

                    if (character == FUNCTION_PARAMS_CLOSING)
                    {
                        parathesisCount--;

                        continue;
                    }

                    if (character != ONELINE_SEPARATOR || parathesisCount > 0)
                    {
                        continue;
                    }

                    std::string block = oneline.substr(start, end - start);

                    start = end;

                    if (block.empty())
                    {
                        continue;
                    }

                    splittedOneline.push_back(String::trim(block));
                }

                splittedOneline.push_back(oneline.substr(start, end - start));

                if (splittedOneline.size() == 1) // SINGLE
                {
                    std::string value = String::trim(splittedOneline.at(0));
    
                    top    = value;
                    right  = value;
                    bottom = value;
                    left   = value;

                    return;
                }
    
                if (splittedOneline.size() == 2) // VERTICAL HORIZONTAL
                {
                    const std::string& vertical   = String::trim(splittedOneline.at(0));
                    const std::string& horizontal = String::trim(splittedOneline.at(1));
    
                    top    = vertical;
                    bottom = vertical;
                    right  = horizontal;
                    left   = horizontal;

                    return;
                }
    
                if (splittedOneline.size() == 3) // TOP BOTTOM HORIZONTAL
                {
                    const std::string& horizontal = String::trim(splittedOneline.at(2));
    
                    top    = String::trim(splittedOneline.at(0));
                    bottom = String::trim(splittedOneline.at(1));
                    right  = horizontal;
                    left   = horizontal;

                    return;
                }
    
                if (splittedOneline.size() >= 4) // TOP RIGHT BOTTOM LEFT
                {
                    top    = String::trim(splittedOneline.at(0));
                    right  = String::trim(splittedOneline.at(1));
                    bottom = String::trim(splittedOneline.at(2));
                    left   = String::trim(splittedOneline.at(3));

                    return;
                }
            }
        }
    }
}