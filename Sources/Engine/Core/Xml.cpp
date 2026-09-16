#include "Chicane/Core/Xml.hpp"

#include "Chicane/Core/Color.hpp"

namespace Chicane
{
    namespace Xml
    {
        static constexpr inline const char* BOOLEAN_TRUE_VALUE        = "true";
        static constexpr inline const char* BOOLEAN_TRUE_ONE_VALUE    = "1";
        static constexpr inline const char* BOOLEAN_TRUE_YES_VALUE    = "yes";
        static constexpr inline const float COLOR_CHANNEL_MAX_VALUE   = 255.0f;
        static constexpr inline const float COLOR_ALPHA_DEFAULT_VALUE = 1.0f;

        bool isEmpty(const XmlDocument& inDocument)
        {
            return inDocument.isEmpty();
        }

        bool isEmpty(const XmlNode& inNode)
        {
            return inNode.isEmpty();
        }

        void save(const XmlDocument& inDocument, const FileSystem::Path& inFilepath)
        {
            inDocument.save(inFilepath);
        }

        XmlDocument load(const FileSystem::Path& inFilepath)
        {
            return XmlDocument::load(inFilepath);
        }

        Attributes getAttributes(const XmlNode& inNode)
        {
            return inNode.getAttributes();
        }

        String getAttribute(const String& inName, const XmlNode& inNode)
        {
            return inNode.getAttribute(inName);
        }

        void addAttribute(XmlNode& outNode, const String& inName, const String& inValue)
        {
            outNode.setAttribute(inName, inValue);
        }

        void addText(XmlNode& outNode, const String& inText)
        {
            outNode.setText(inText);
        }

        float parseFloat(const String& inValue, float inFallback)
        {
            const String value = inValue.trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            try
            {
                return std::stof(value.toStandard());
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        bool parseBool(const String& inValue, bool inFallback)
        {
            const String value = inValue.trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            return value.equals(BOOLEAN_TRUE_VALUE, BOOLEAN_TRUE_ONE_VALUE, BOOLEAN_TRUE_YES_VALUE);
        }

        std::uint32_t parseUint(const String& inValue, std::uint32_t inFallback)
        {
            const String value = inValue.trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            try
            {
                return static_cast<std::uint32_t>(std::stoul(value.toStandard()));
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        int parseInt(const String& inValue, int inFallback)
        {
            const String value = inValue.trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            try
            {
                return std::stoi(value.toStandard());
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        Vec3 parseVec3(const String& inValue, const Vec3& inFallback)
        {
            const std::vector<String> values = inValue.split(',');
            if (values.size() < 3)
            {
                return inFallback;
            }

            try
            {
                return Vec3(
                    std::stof(values.at(0).toStandard()),
                    std::stof(values.at(1).toStandard()),
                    std::stof(values.at(2).toStandard())
                );
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        Vec4 parseColor(const String& inValue, const Vec4& inFallback)
        {
            const String value = inValue.trim();
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

            try
            {
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
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        float parseFloat(const XmlNode& inNode, const String& inName, float inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseFloat(inNode.getAttribute(inName), inFallback);
        }

        bool parseBool(const XmlNode& inNode, const String& inName, bool inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseBool(inNode.getAttribute(inName), inFallback);
        }

        std::uint32_t parseUint(const XmlNode& inNode, const String& inName, std::uint32_t inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseUint(inNode.getAttribute(inName), inFallback);
        }

        String parseString(const XmlNode& inNode, const String& inName, const String& inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return inNode.getAttribute(inName);
        }

        Vec3 parseVec3(const XmlNode& inNode, const String& inName, const Vec3& inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseVec3(inNode.getAttribute(inName), inFallback);
        }

        Vec4 parseColor(const XmlNode& inNode, const String& inName, const Vec4& inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseColor(inNode.getAttribute(inName), inFallback);
        }
    }
}
