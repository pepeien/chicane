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
            if (!inInstance)
            {
                return false;
            }

            return isLoaded(inInstance->getFilepath().string());
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

        template<class T = Box::Asset>
        const T* getAsset(const std::string& inIdentifier)
        {
            if (!isLoaded(inIdentifier))
            {
                return nullptr;
            }

            return static_cast<const T*>(m_cache.at(inIdentifier));
        }

        void loadModel(const Box::Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::Model))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_modelManager->load(
                filepath,
                static_cast<const Box::Model*>(inAsset)
            );
        }

        void loadTexture(const Box::Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::Texture))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_textureManager->add(
                filepath,
                static_cast<const Box::Texture*>(inAsset)
            );
        }

        void loadMesh(const Box::Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::Mesh))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            const Box::Mesh* mesh = static_cast<const Box::Mesh*>(inAsset);

            for (const Box::Mesh::Group& group : mesh->getGroups())
            {
                const std::string& model = group.getModel();

                if (!model.empty() && !isLoaded(model))
                {
                    loadModel(new Box::Model(model));
                }

                const std::string& texture = group.getTexture();

                if (!texture.empty() && !isLoaded(texture))
                {
                    loadTexture(new Box::Texture(texture));
                }
            }

            cacheAsset(
                filepath,
                inAsset
            );
        }

        void loadCubemap(const Box::Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::Cubemap))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            const Box::Cubemap* cubemap = static_cast<const Box::Cubemap*>(inAsset);

            m_cubemapManager->add(
                "Skybox",
                {
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Left))->getData(),  // Positive X
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Right))->getData(), // Negative X
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Front))->getData(), // Positive Y
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Back))->getData(),  // Negative Y
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Up))->getData(),    // Positive Z
                    loadTexture(cubemap->getTexture(Box::Cubemap::Side::Down))->getData()   // Negative Z
                }
            );
        }

        const Box::Cubemap* loadCubemap(const std::string& inFilepath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilepath);

            if (header.type != Box::Asset::Type::Cubemap)
            {
                throw std::runtime_error(inFilepath + " is not a cubemap");
            }

            if (!isLoaded(inFilepath))
            {
                loadCubemap(new Box::Cubemap(inFilepath));
            }

            return getAsset<Box::Cubemap>(inFilepath);
        }

        const Box::Texture* loadTexture(const std::string& inFilepath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilepath);

            if (header.type != Box::Asset::Type::Texture)
            {
                throw std::runtime_error(inFilepath + "is not a texture");
            }

            if (!isLoaded(inFilepath))
            {
                loadTexture(new Box::Texture(inFilepath));
            }

            return getAsset<Box::Texture>(inFilepath);
        }

        const Box::Mesh* loadMesh(const std::string& inFilepath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilepath);

            if (header.type != Box::Asset::Type::Mesh)
            {
                throw std::runtime_error(inFilepath + "is not a mesh");
            }

            if (!isLoaded(inFilepath))
            {
                loadMesh(new Box::Mesh(inFilepath));
            }

            return getAsset<Box::Mesh>(inFilepath);
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}