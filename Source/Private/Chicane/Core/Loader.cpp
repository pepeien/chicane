#include "Chicane/Core/Loader.hpp"

namespace Chicane
{
    namespace Loader
    {
        std::unique_ptr<CubeMap::Manager> m_cubemapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager> m_modelManager     = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, Box::Instance> m_cache {};

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

        bool isLoaded(const Box::Instance& inInstance)
        {
            return isLoaded(inInstance.filepath);
        }

        void cacheEntry(const std::string& inIdentifier, const Box::Entry& inEntry)
        {
            if (!isLoaded(inIdentifier))
            {
                return;
            }

            m_cache[inIdentifier].entries.push_back(inEntry);
        }

        void cacheEntry(const std::string& inIdentifier, const std::vector<Box::Entry>& inEntries)
        {
            for (const Box::Entry& entry : inEntries)
            {
                cacheEntry(
                    inIdentifier,
                    entry
                );
            }
        }

        void cacheAsset(const std::string& inIdentifier, const Box::Instance& inInstance)
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

        Box::Instance getAsset(const std::string& inIdentifier)
        {
            if (!isLoaded(inIdentifier))
            {
                return {};
            }

            return m_cache.at(inIdentifier);
        }

        void loadModel(const Box::Instance& inInstance)
        {
            if (isLoaded(inInstance))
            {
                return;
            }

            Box::Entry cachedEntry {};
            cachedEntry.type      = Box::EntryType::Model;
            cachedEntry.reference = inInstance.filepath;

            Box::Instance cachedInstance {};
            cachedInstance.name     = inInstance.filepath;
            cachedInstance.type     = inInstance.type;
            cachedInstance.filepath = inInstance.filepath;
            cachedInstance.entries.push_back(cachedEntry);

            cacheAsset(
                inInstance.filepath,
                cachedInstance
            );

            m_modelManager->load(
                inInstance.filepath,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        void loadTexture(const Box::Instance& inInstance)
        {
            if (isLoaded(inInstance))
            {
                return;
            }

            Box::Entry cachedEntry {};
            cachedEntry.type      = Box::EntryType::Texture;
            cachedEntry.reference = inInstance.filepath;

            Box::Instance cachedInstance {};
            cachedInstance.name     = inInstance.filepath;
            cachedInstance.type     = inInstance.type;
            cachedInstance.filepath = inInstance.filepath;
            cachedInstance.entries.push_back(cachedEntry);

            cacheAsset(
                inInstance.filepath,
                cachedInstance
            );

            m_textureManager->add(
                inInstance.filepath,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        void loadMesh(const Box::Instance& inInstance)
        {
            if (isLoaded(inInstance))
            {
                return;
            }

            Box::Instance instance = Box::read(inInstance.filepath);

            Box::Instance cachedInstance {};
            cachedInstance.name     = instance.filepath;
            cachedInstance.filepath = inInstance.filepath;
            cachedInstance.type     = inInstance.type;

            for (Box::Entry entry : instance.entries)
            {
                if (entry.reference.empty())
                {
                    continue;
                }

                if (entry.type == Box::EntryType::Model)
                {
                    loadModel(Box::readHeader(entry.reference));
                }

                if (entry.type == Box::EntryType::Texture)
                {
                    loadTexture(Box::readHeader(entry.reference));
                }

                Box::Entry cachedEntry {};
                cachedEntry.reference = entry.reference;
                cachedEntry.type      = entry.type;

                cachedInstance.entries.push_back(cachedEntry);
            }

            cacheAsset(
                inInstance.filepath,
                cachedInstance
            );
        }

        void loadCubemap(const Box::Instance& inInstance)
        {
            if (isLoaded(inInstance))
            {
                return;
            }

            Box::Instance instance = Box::read(inInstance.filepath);

            if (instance.entries.size() < CUBEMAP_IMAGE_COUNT)
            {
                throw std::runtime_error("The [" + instance.filepath + "] cubemap isn't valid");
            }

            std::vector<Box::Entry> textures {};

            for (Box::Entry entry : instance.entries)
            {
                if (entry.reference.empty())
                {
                    continue;
                }

                textures.push_back(Box::read(entry.reference).entries[0]);
            }

            m_cubemapManager->add(
                "Skybox",
                {
                    textures[0].data, // Positive X
                    textures[1].data, // Negative X
                    textures[2].data, // Positive Y
                    textures[3].data, // Negative Y
                    textures[4].data, // Positive Z
                    textures[5].data  // Negative Z
                }
            );
        }

        Box::Instance loadCubemap(const std::string& inFilePath)
        {
            if (Box::readHeader(inFilePath).type != Box::Type::CubeMap)
            {
                throw std::runtime_error(inFilePath + "is not a cubemap");
            }

            if (!isLoaded(inFilePath))
            {
                loadCubemap(Box::readHeader(inFilePath));
            }

            return getAsset(inFilePath);
        }

        Box::Instance loadTexture(const std::string& inFilePath)
        {
            if (Box::readHeader(inFilePath).type != Box::Type::Texture)
            {
                throw std::runtime_error(inFilePath + "is not a texture");
            }

            if (!isLoaded(inFilePath))
            {
                loadTexture(Box::readHeader(inFilePath));
            }

            return getAsset(inFilePath);
        }

        Box::Instance loadMesh(const std::string& inFilePath)
        {
            if (Box::readHeader(inFilePath).type != Box::Type::Mesh)
            {
                throw std::runtime_error(inFilePath + "is not a mesh");
            }

            if (!isLoaded(inFilePath))
            {
                loadMesh(Box::readHeader(inFilePath));
            }

            return getAsset(inFilePath);
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}