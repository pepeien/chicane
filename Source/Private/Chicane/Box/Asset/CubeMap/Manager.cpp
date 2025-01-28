#include "Chicane/Box/Asset/CubeMap/Manager.hpp"

#include "Chicane/Box/Loader.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace CubeMap
        {
            static const RawData      EMPTY_INSTANCE = {};
            static const CompiledData EMPTY_DATA     = {};

            Manager::Manager()
                : Super()
            {}

            void Manager::load(const std::string& inId, const CubeMap::Instance* inAsset)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                RawData instance {};

                for (const auto& [side, texture] : inAsset->getTextures())
                {
                    instance.insert(
                        std::make_pair(
                            side,
                            loadTexture(texture)->getData()
                        )
                    );
                }

                Super::load(inId, instance);

        	    Texture::Manager* manager = getTextureManager();

                CompiledData data {};

                for (const auto& [side, texture] : inAsset->getTextures())
                {
                    if (!manager->isAllocated(texture))
                    {
                        Image::Data data {};
                        data.pixels = FileSystem::readImageFromMemory(
                            data.width,
                            data.height,
                            data.channels,
                            instance.at(side)
                        );

                        manager->allocate(texture, data);
                    }

                    data.insert(
                        std::make_pair(
                            side,
                            manager->getData(texture)
                        )
                    );
                }

                Super::allocate(inId, data);
                Super::activate(inId);
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
                if (!isLoaded(inId) || !isAllocated(inId))
                {
                    return EMPTY_DATA;
                }

                return m_datum.at(inId);
            }
        }
    }
}