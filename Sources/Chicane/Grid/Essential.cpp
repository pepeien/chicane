#include "Grid/Essential.hpp"

#include "Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, Vec<4, std::uint32_t>> m_rgbaColors {
            { Style::HEX_COLOR_TRANSPARENT, Vec<4, std::uint32_t>(0) }
        };
        std::unordered_map<std::string, Vec<3, std::uint32_t>> m_rgbColors {
            { Style::HEX_COLOR_TRANSPARENT, Vec<3, std::uint32_t>(0) }
        };

        Vec<4, std::uint32_t> hexToRgba(const std::string& inColor)
        {
            std::string backgroundColor = String::trim(inColor);
            std::transform(
                backgroundColor.begin(),
                backgroundColor.end(),
                backgroundColor.begin(),
                ::tolower
            );

            bool bIsTransparent = backgroundColor.empty() ||
                                  String::areEquals(backgroundColor, Style::BACKGROUND_COLOR_TRANSPARENT);
            bool bIsNotHex      = backgroundColor.size() < 7 || backgroundColor.size() > 9;

            if (bIsTransparent || bIsNotHex)
            {
                backgroundColor = Style::HEX_COLOR_TRANSPARENT;
            }

            if (m_rgbaColors.find(backgroundColor) == m_rgbaColors.end())
            {
                std::string hexColor = backgroundColor;

                backgroundColor = backgroundColor.substr(
                    1,
                    backgroundColor.size() - 1
                );
                backgroundColor = backgroundColor.size() == 6 ? backgroundColor + "FF" : backgroundColor;

                Vec<4, std::uint32_t> result = Vec<4, std::uint32_t>(0U);
                
                sscanf(
                    backgroundColor.c_str(),
                    "%02hx%02x%02hx%02hx",
                    &result.r,
                    &result.g,
                    &result.b,
                    &result.a
                );

                m_rgbaColors.insert(
                    std::make_pair(
                        hexColor,
                        result
                    )
                );

                return result;
            }

            return m_rgbaColors.at(backgroundColor);
        }

        Vec<3, std::uint32_t> hexToRgb(const std::string& inColor)
        {
            Vec<4, std::uint32_t> color = hexToRgba(inColor);

            if (m_rgbColors.find(inColor) == m_rgbColors.end())
            {
                Vec<3, std::uint32_t> result(color.x, color.y, color.z);

                m_rgbColors.insert(
                    std::make_pair(
                        inColor,
                        result
                    )
                );

                return result;
            }

            return m_rgbColors.at(inColor);
        }
    }
}