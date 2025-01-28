#include "Chicane/Box/Asset/Texture/Manager.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            static const Image::Instance EMPTY_INSTANCE = {};
            static const Image::Data     EMPTY_DATA     = {};

            Manager::Manager()
                : Super()
            {}

            Manager::~Manager()
            {
                for (auto& [id, image] : m_datum)
                {
                    if (image.pixels)
                    {
                        free(image.pixels);
                        image.pixels = nullptr;
                    }
                }
            }

            void Manager::onActivation(const std::string& inId)
            {
                Image::Data data {};
                data.pixels = FileSystem::readImageFromMemory(
                    data.width,
                    data.height,
                    data.channels,
                    getInstance(inId)
                );

                Super::allocate(inId, data);
            }

            void Manager::onDeactivation(const std::string& inId)
            {
                free(m_datum.at(inId).pixels);

                m_datum.at(inId).pixels = nullptr;

                Super::deallocate(inId);
            }

            void Manager::load(const std::string& inId, const Texture::Instance* inAsset)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                Super::load(inId, inAsset->getData());
            }

            const Image::Instance& Manager::getInstance(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_instances.at(inId);
            }

            const Image::Data& Manager::getData(const std::string& inId) const
            {
                if (!isLoaded(inId) || !isAllocated(inId))
                {
                    return EMPTY_DATA;
                }

                return m_datum.at(inId);
            }

            std::uint32_t Manager::getIndex(const std::string& inId) const
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