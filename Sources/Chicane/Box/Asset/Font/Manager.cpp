#include "Chicane/Box/Asset/Font/Manager.hpp"

#include "Chicane/Box/Asset/Font.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            static const RawData    EMPTY_DATA     = {};
            static const ParsedData EMPTY_INSTANCE = {};

            Manager::Manager()
                : Super()
            {}

            bool Manager::isFamilyAllocated(const std::string& inFamily) const
            {
                return std::find_if(
                    m_datum.begin(),
                    m_datum.end(),
                    [inFamily](const std::pair<std::string, ParsedData>& pair)
                    {
                        return String::areEquals(inFamily, pair.second.name);
                    }
                ) != m_datum.end();
            }

            void Manager::load(const std::string& inId, const Font::Instance* inFont)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                if (inFont->isEmpty())
                {
                    return;
                }

                RawData instance = {};
                instance.vendor = inFont->getVendor();
                instance.data   = inFont->getData();

                Super::load(inId, instance);

                allocate(inId);
            }

            void Manager::allocate(const std::string& inId)
            {
                if (isAllocated(inId))
                {
                    return;
                }

                const RawData& instance = getData(inId);

                switch (instance.vendor)
                {
                case Vendor::TrueType:
                    Super::allocate(inId, TrueType::parse(instance.data));

                    break;

                default:
                    throw std::runtime_error("Failed to import Model due to invalid type");
                }
            }

            const RawData& Manager::getData(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_DATA;
                }

                return m_instances.at(inId);
            }

            const ParsedData& Manager::getParsed(const std::string& inId) const
            {
                if (!isAllocated(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_datum.at(inId);
            }

            const ParsedData& Manager::getByFamily(const std::string& inFamily) const
            {
                if (!isFamilyAllocated(inFamily))
                {
                    return EMPTY_INSTANCE;
                }

                auto found = std::find_if(
                    m_datum.begin(),
                    m_datum.end(),
                    [inFamily](const std::pair<std::string, ParsedData>& pair)
                    {
                        return String::areEquals(inFamily, pair.second.name);
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