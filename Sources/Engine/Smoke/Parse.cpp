#include "Chicane/Smoke/Parse.hpp"

#include <algorithm>
#include <cmath>
#include <random>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Smoke
    {
        static std::mt19937& rng()
        {
            static thread_local std::mt19937 instance(std::random_device {}());

            return instance;
        }

        float randomFloat(float inFrom, float inTo)
        {
            if (inFrom == inTo)
            {
                return inFrom;
            }

            std::uniform_real_distribution<float> distribution(std::min(inFrom, inTo), std::max(inFrom, inTo));

            return distribution(rng());
        }

        float randomFloat()
        {
            return randomFloat(0.0f, 1.0f);
        }

        Vec3 randomDirection()
        {
            const float z = randomFloat(-1.0f, 1.0f);
            const float a = randomFloat(0.0f, Math::TWO_PI);
            const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));

            return Vec3(r * std::cos(a), r * std::sin(a), z);
        }

        Range parseRange(const pugi::xml_node& inNode, const char* inName, const Range& inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            const std::vector<String> values = String(attribute.as_string()).split(',');
            if (values.empty())
            {
                return inFallback;
            }

            const float from = std::stof(values.at(0).trim().toStandard());
            if (values.size() < 2)
            {
                return Range(from);
            }

            return Range(from, std::stof(values.at(1).trim().toStandard()));
        }

        float parseFloat(const pugi::xml_node& inNode, const char* inName, float inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            return attribute.as_float(inFallback);
        }

        bool parseBool(const pugi::xml_node& inNode, const char* inName, bool inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            const String value = String(attribute.as_string()).trim();

            return value.equals(BOOLEAN_TRUE_VALUE, BOOLEAN_TRUE_ONE_VALUE, BOOLEAN_TRUE_YES_VALUE);
        }

        std::uint32_t parseUint(const pugi::xml_node& inNode, const char* inName, std::uint32_t inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            return static_cast<std::uint32_t>(attribute.as_uint(inFallback));
        }

        String parseString(const pugi::xml_node& inNode, const char* inName, const String& inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            return String(attribute.as_string());
        }

        Vec3 parseVec3(const pugi::xml_node& inNode, const char* inName, const Vec3& inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            const std::vector<String> values = String(attribute.as_string()).split(',');
            if (values.size() < 3)
            {
                return inFallback;
            }

            return Vec3(
                std::stof(values.at(0).toStandard()),
                std::stof(values.at(1).toStandard()),
                std::stof(values.at(2).toStandard())
            );
        }

        Vec4 parseColor(const pugi::xml_node& inNode, const char* inName, const Vec4& inFallback)
        {
            const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
            if (attribute.empty())
            {
                return inFallback;
            }

            const String value = String(attribute.as_string()).trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            if (value.startsWith(Color::HEX_KEYWORD) || value.find(Color::RGB_KEYWORD) != String::npos)
            {
                const Color::Rgba rgba = Color::toRgba(value);

                return Vec4(
                    static_cast<float>(rgba.x) / COLOR_CHANNEL_MAX_VALUE,
                    static_cast<float>(rgba.y) / COLOR_CHANNEL_MAX_VALUE,
                    static_cast<float>(rgba.z) / COLOR_CHANNEL_MAX_VALUE,
                    static_cast<float>(rgba.w) / COLOR_CHANNEL_MAX_VALUE
                );
            }

            const std::vector<String> values = value.split(',');
            if (values.size() < 3)
            {
                return inFallback;
            }

            Vec4 result(
                std::stof(values.at(0).toStandard()),
                std::stof(values.at(1).toStandard()),
                std::stof(values.at(2).toStandard()),
                COLOR_ALPHA_DEFAULT_VALUE
            );

            if (values.size() > 3)
            {
                result.w = std::stof(values.at(3).toStandard());
            }

            return result;
        }
    }
}
