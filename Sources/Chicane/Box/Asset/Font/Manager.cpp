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

            void Manager::load(const std::string& inId, const Font::Instance* inModel)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                switch (inModel->getVendor())
                {
                case Vendor::Type::TrueType :
                    Super::load(inId, Vendor::TrueType::parse(inModel->getData()));

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
        }
    }
}