#include "Chicane/Box/Asset/Font/Manager.hpp"

#include "Chicane/Box/Asset/Font/Vendor/TrueType.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            static const RawData      EMPTY_INSTANCE = {};
            static const CompiledData EMPTY_DATA     = {};

            Manager::Manager()
                : Super()
            {}

            bool Manager::isFamilyLoaded(const std::string& inFamily) const
            {
                return std::find_if(
                    m_instances.begin(),
                    m_instances.end(),
                    [inFamily](const std::pair<std::string, RawData>& pair)
                    {
                        return String::areEquals(inFamily, pair.second.name);
                    }
                ) != m_instances.end();
            }

            void Manager::load(const std::string& inId, const Font::Instance* inFont)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                switch (inFont->getVendor())
                {
                case Vendor::Type::TrueType:
                    Super::load(inId, Vendor::TrueType::parse(inFont->getData()));

                    break;

                default:
                    throw std::runtime_error("Failed to import Model due to invalid type");
                }

                Super::allocate(inId, {});
            }

            const RawData& Manager::getInstance(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_instances.at(inId);
            }

            const CompiledData& Manager::getData(const std::string& inId) const
            {
                if (!isAllocated(inId))
                {
                    return EMPTY_DATA;
                }

                return m_datum.at(inId);
            }

            const RawData& Manager::getByFamily(const std::string& inFamily) const
            {
                if (!isFamilyLoaded(inFamily))
                {
                    return EMPTY_INSTANCE;
                }

                auto found = std::find_if(
                    m_instances.begin(),
                    m_instances.end(),
                    [inFamily](const std::pair<std::string, RawData>& pair)
                    {
                        return String::areEquals(inFamily, pair.second.name);
                    }
                );

                if (found == m_instances.end())
                {
                    return EMPTY_INSTANCE;
                }

                return found->second;
            }
        }
    }
}