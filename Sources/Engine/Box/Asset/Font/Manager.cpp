#include "Chicane/Box/Asset/Font/Manager.hpp"

#include "Chicane/Box/Asset/Font/TrueType.hpp"

namespace Chicane
{
    namespace Box
    {
        static const FontExtracted EMPTY_DATA     = {};
        static const FontParsed    EMPTY_INSTANCE = {};

        FontManager::FontManager()
            : Super()
        {}

        bool FontManager::isFamilyAllocated(const String& inFamily) const
        {
            return std::find_if(
                       m_datum.begin(),
                       m_datum.end(),
                       [inFamily](const std::pair<String, FontParsed>& pair) {
                           return inFamily.equals(pair.second.name);
                       }
                   ) != m_datum.end();
        }

        void FontManager::load(const String& inId, const Font& inFont)
        {
            if (isLoaded(inId))
            {
                return;
            }

            if (inFont.isEmpty())
            {
                return;
            }

            FontExtracted instance = {};
            instance.vendor        = inFont.getVendor();
            instance.data          = inFont.getData();

            Super::load(inId, instance);

            allocate(inId);
        }

        void FontManager::allocate(const String& inId)
        {
            if (isAllocated(inId))
            {
                return;
            }

            const FontExtracted& instance = getData(inId);

            switch (instance.vendor)
            {
            case FontVendor::TrueType:
                Super::allocate(inId, FontTrueType::parse(instance.data));

                break;

            default:
                throw std::runtime_error(
                    "Failed to import Model due to invalid type"
                );
            }
        }

        const FontExtracted& FontManager::getData(const String& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_DATA;
            }

            return m_instances.at(inId);
        }

        const FontParsed& FontManager::getParsed(const String& inId) const
        {
            if (!isAllocated(inId))
            {
                return EMPTY_INSTANCE;
            }

            return m_datum.at(inId);
        }

        const FontParsed& FontManager::getByFamily(const String& inFamily) const
        {
            if (!isFamilyAllocated(inFamily))
            {
                return EMPTY_INSTANCE;
            }

            auto found = std::find_if(
                m_datum.begin(),
                m_datum.end(),
                [inFamily](const std::pair<String, FontParsed>& pair) {
                    return inFamily.equals(pair.second.name);
                }
            );

            if (found == m_datum.end())
            {
                return EMPTY_INSTANCE;
            }

            return found->second;
        }
    }
}