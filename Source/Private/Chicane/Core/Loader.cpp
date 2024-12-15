#include "Chicane/Core/Loader.hpp"

namespace Chicane
{
    namespace Loader
    {
        std::unique_ptr<CubeMap::Manager> m_cubemapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager> m_modelManager     = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, const Box::Asset*> m_cache {};

        CubeMap::Manager* getCubemapManager()
        {
            return m_cubemapManager.get();
        }

        Model::Manager* getModelManager()
        {
            return m_modelManager.get();
        }

        Texture::Manager* getTextureManager()
        {
            return m_textureManager.get();
        }

        bool isLoaded(const std::string& inIdentifier)
        {
            return m_cache.find(inIdentifier) != m_cache.end();
        }

        bool isLoaded(const Box::Asset* inInstance)
        {
            return isLoaded(inInstance->getHeader().filepath.string());
        }

        void cacheEntry(const std::string& inIdentifier, const Box::Asset* inEntry)
        {
            if (!isLoaded(inIdentifier))
            {
                return;
            }

            
        }

        void cacheEntry(const std::string& inIdentifier, const std::vector<Box::Asset*>& inEntries)
        {
            for (const Box::Asset* entry : inEntries)
            {
                cacheEntry(
                    inIdentifier,
                    entry
                );
            }
        }

        void cacheAsset(const std::string& inIdentifier, const Box::Asset* inInstance)
        {
            if (isLoaded(inIdentifier))
            {
                return;
            }

            m_cache.insert(
                std::make_pair(
                    inIdentifier,
                    inInstance
                )
            );
        }

        const Box::Asset* getAsset(const std::string& inIdentifier)
        {
            if (!isLoaded(inIdentifier))
            {
                return nullptr;
            }

            return m_cache.at(inIdentifier);
        }

        void loadModel(const Box::Asset* inAsset)
        {
            if (isLoaded(inAsset))
            {
                return;
            }

        }

        void loadTexture(const Box::Asset* inAsset)
        {
            if (isLoaded(inAsset))
            {
                return;
            }

        }

        void loadMesh(const Box::Asset* inAsset)
        {
            if (isLoaded(inAsset))
            {
                return;
            }

        }

        void loadCubemap(const Box::Asset* inAsset)
        {
            if (isLoaded(inAsset))
            {
                return;
            }

        }

        const Box::Cubemap* loadCubemap(const std::string& inFilePath)
        {
            return nullptr;
        }

        const Box::Texture* loadTexture(const std::string& inFilePath)
        {
            return nullptr;
        }

        const Box::Mesh* loadMesh(const std::string& inFilePath)
        {
            return nullptr;
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}