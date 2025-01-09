#include "Chicane/Renderer/CubeMap/Manager.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Loader.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        static const CubeMap::Instance EMPTY_INSTANCE = {};
        static const CubeMap::Data     EMPTY_DATA     = {};

        Manager::Manager()
            : Super()
        {}

        void Manager::load(const std::string& inId, const Box::CubeMap* inAsset)
        {
            if (isLoaded(inId))
            {
                return;
            }

            CubeMap::Instance instance {};

            for (const auto& [side, texture] : inAsset->getTextures())
            {
                instance.insert(
                    std::make_pair(
                        side,
                        Loader::loadTexture(texture)->getData()
                    )
                );
            }

            Super::load(inId, instance);

    	    Texture::Manager* manager = Loader::getTextureManager();

            CubeMap::Data data {};

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

        const CubeMap::Instance& Manager::getInstance(const std::string& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_INSTANCE;
            }

            return m_instances.at(inId);
        }

        const CubeMap::Data& Manager::getData(const std::string& inId) const
        {
            if (!isLoaded(inId) || !isAllocated(inId))
            {
                return EMPTY_DATA;
            }

            return m_datum.at(inId);
        }
    }
}