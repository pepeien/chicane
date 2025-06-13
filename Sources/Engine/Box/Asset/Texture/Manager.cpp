#include "Chicane/Box/Asset/Texture/Manager.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            static const Image::Raw      EMPTY_RAW_DATA      = {};
            static const Image::Instance EMPTY_COMPILED_DATA = {};

            Manager::Manager()
                : Super()
            {}

            void Manager::onActivation(const String& inId)
            {
                Super::allocate(inId, Image::Instance(getInstance(inId)));
            }

            void Manager::onDeactivation(const String& inId)
            {
                Super::deallocate(inId);
            }

            void Manager::load(const String& inId, const Texture::Instance& inAsset)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                Super::load(inId, inAsset.getData());
            }

            const Image::Raw& Manager::getInstance(const String& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_RAW_DATA;
                }

                return m_instances.at(inId);
            }

            const Image::Instance& Manager::getData(const String& inId) const
            {
                if (!isLoaded(inId) || !isAllocated(inId))
                {
                    return EMPTY_COMPILED_DATA;
                }

                return m_datum.at(inId);
            }

            std::uint32_t Manager::getIndex(const String& inId) const
            {
                auto interator = std::find(
                    m_activeIds.begin(),
                    m_activeIds.end(),
                    inId
                );

                if (interator == m_activeIds.end())
                {
                    return 0;
                }

                return static_cast<std::uint32_t>(interator - m_activeIds.begin());
            }
        }
    }
}