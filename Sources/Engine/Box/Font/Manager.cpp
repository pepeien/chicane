#include "Chicane/Box/Font/Manager.hpp"

#include "Chicane/Box/Font/TrueType.hpp"

namespace Chicane
{
    namespace Box
    {
        FontManager::FontManager()
            : Manager()
        {}

        void FontManager::onLoad(const String& inId, const Font& inData)
        {
            switch (inData.getVendor())
            {
            case FontVendor::OpenType:
            case FontVendor::TrueType:
                add(inId, FontTrueType::parse(inData));

                break;

            default:
                throw std::runtime_error("Failed to import Font due to invalid type");
            }
        }

        bool FontManager::containsFamily(const String& inName) const
        {
            return std::find_if(
                       m_instances.begin(),
                       m_instances.end(),
                       [inName](const std::pair<String, FontParsed>& pair) { return inName.equals(pair.second.name); }
                   ) != m_instances.end();
        }

        const FontParsed& FontManager::getFamily(const String& inName) const
        {
            if (!containsFamily(inName))
            {
                return FontParsed::empty();
            }

            auto found = std::find_if(
                m_instances.begin(),
                m_instances.end(),
                [inName](const std::pair<String, FontParsed>& pair) { return inName.equals(pair.second.name); }
            );

            return found->second;
        }
    }
}