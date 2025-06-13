#include "Chicane/Box/Asset/Font/Manager.hpp"

#include "Chicane/Box/Asset/Font.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            static const Extracted    EMPTY_DATA     = {};
            static const Parsed EMPTY_INSTANCE = {};

            Manager::Manager()
                : Super()
            {}

            bool Manager::isFamilyAllocated(const String& inFamily) const
            {
                return std::find_if(
                    m_datum.begin(),
                    m_datum.end(),
                    [inFamily](const std::pair<String, Parsed>& pair)
                    {
                        return inFamily.equals(pair.second.name);
                    }
                ) != m_datum.end();
            }

            void Manager::load(const String& inId, const Font::Instance& inFont)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                if (inFont.isEmpty())
                {
                    return;
                }

                Extracted instance = {};
                instance.vendor = inFont.getVendor();
                instance.data   = inFont.getData();

                Super::load(inId, instance);

                allocate(inId);
            }

            void Manager::allocate(const String& inId)
            {
                if (isAllocated(inId))
                {
                    return;
                }

                const Extracted& instance = getData(inId);

                switch (instance.vendor)
                {
                case Vendor::TrueType:
                    Super::allocate(inId, TrueType::parse(instance.data));

                    break;

                default:
                    throw std::runtime_error("Failed to import Model due to invalid type");
                }
            }

            const Extracted& Manager::getData(const String& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_DATA;
                }

                return m_instances.at(inId);
            }

            const Parsed& Manager::getParsed(const String& inId) const
            {
                if (!isAllocated(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_datum.at(inId);
            }

            const Parsed& Manager::getByFamily(const String& inFamily) const
            {
                if (!isFamilyAllocated(inFamily))
                {
                    return EMPTY_INSTANCE;
                }

                auto found = std::find_if(
                    m_datum.begin(),
                    m_datum.end(),
                    [inFamily](const std::pair<String, Parsed>& pair)
                    {
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
}