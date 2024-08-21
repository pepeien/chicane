#include "Chicane/Core/Allocator.hpp"

namespace Chicane
{
    namespace Allocator
    {
        struct CachedEntry
        {
        public:
            std::string identifier = "";
            Box::EntryType type    = Box::EntryType::Undefined;
        };

        struct CachedInstance
        {
        public:
            Box::Type type                   = Box::Type::Undefined;
            std::vector<CachedEntry> entries {};
        };

        std::unique_ptr<CubeMap::Manager> m_cubemapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager> m_modelManager     = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, CachedInstance> m_cache {};

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

        bool isCached(const std::string& inIdentifier)
        {
            return m_cache.find(inIdentifier) != m_cache.end();
        }

        bool isCached(const Box::Instance& inInstance)
        {
            return isCached(inInstance.filepath);
        }

        void cacheEntry(const std::string& inIdentifier, const CachedEntry& inEntry)
        {
            if (!isCached(inIdentifier))
            {
                return;
            }

            m_cache[inIdentifier].entries.push_back(inEntry);
        }

        void cacheEntry(const std::string& inIdentifier, const std::vector<CachedEntry>& inEntries)
        {
            for (const CachedEntry& entry : inEntries)
            {
                cacheEntry(
                    inIdentifier,
                    entry
                );
            }
        }

        void cacheInstance(const std::string& inIdentifier, const CachedInstance& inInstance)
        {
            if (isCached(inIdentifier))
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

        Box::Instance getCached(const std::string& inIdentifier)
        {
            if (!isCached(inIdentifier))
            {
                return {};
            }

            const CachedInstance& data = m_cache.at(inIdentifier);

            Box::Instance result {};
            result.name = inIdentifier;
            result.type = data.type;
 
            for (const CachedEntry& dataEntry : data.entries)
            {
                Box::Entry entry {};
                entry.reference = dataEntry.identifier;
                entry.type      = dataEntry.type;

                result.entries.push_back(entry);
            }

            return result;
        }

        Box::Instance loadMesh(const Box::Instance& inInstance)
        {
            if (!isCached(inInstance))
            {
                Box::Instance instance = Box::read(inInstance.filepath);

                CachedInstance cachedInstance {};
                cachedInstance.type = inInstance.type;

                for (Box::Entry entry : instance.entries)
                {
                    if (entry.reference.empty())
                    {
                        continue;
                    }

                    load(entry.reference);

                    CachedEntry cachedEntry {};
                    cachedEntry.identifier = entry.reference;
                    cachedEntry.type       = entry.type;

                    cachedInstance.entries.push_back(cachedEntry);
                }

                cacheInstance(
                    inInstance.filepath,
                    cachedInstance
                );
            }

            return getCached(inInstance.filepath);
        }

        void loadCubemap(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
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

        void loadModel(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
            {
                return;
            }

            m_modelManager->add(
                inInstance.filepath,
                Box::read(inInstance.filepath).entries[0]
            );

            CachedEntry cachedEntry {};
            cachedEntry.type       = Box::EntryType::Model;
            cachedEntry.identifier = inInstance.filepath;

            CachedInstance cachedInstance {};
            cachedInstance.type = inInstance.type;
            cachedInstance.entries.push_back(cachedEntry);

            cacheInstance(
                inInstance.filepath,
                cachedInstance
            );
        }

        void loadTexture(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
            {
                return;
            }

            m_textureManager->add(
                inInstance.filepath,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        Box::Instance loadFromCache(const std::string& inIdentifier)
        {
            if (!isCached(inIdentifier))
            {
                return {};
            }

            const CachedInstance& instance = m_cache.at(inIdentifier);

            Box::Instance result {};
            result.name = inIdentifier;

            if (instance.type == Box::Type::Mesh)
            {
                for (const CachedEntry& entry : instance.entries)
                {
                    loadFromCache(entry.identifier);

                    Box::Entry data {};
                    data.reference = entry.identifier;
                    data.type      = entry.type;

                    result.entries.push_back(data);

                    return result;
                }
            }

            for (const CachedEntry& entry : instance.entries)
            {
                switch (entry.type)
                {
                case Box::EntryType::Model:
                    m_modelManager->use(entry.identifier);

                    Box::Entry data {};
                    data.reference = entry.identifier;
                    data.type      = entry.type;

                    result.entries.push_back(data);
                }
            }

            return result;
        }

        Box::Instance loadFromDisk(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
            {
                return {};
            }

            switch (inInstance.type)
            {
            case Box::Type::Mesh:
                return loadMesh(inInstance);

            case Box::Type::CubeMap:
                loadCubemap(inInstance);

                break;

            case Box::Type::Texture:
                loadTexture(inInstance);

                break;

            case Box::Type::Model:
                loadModel(inInstance);

                break;

            default:
                return {};
            }

            return inInstance;
        }

        Box::Instance load(const std::string& inFilePath)
        {
            if (isCached(inFilePath))
            {
                return loadFromCache(inFilePath);
            }

            return loadFromDisk(Box::readHeader(inFilePath));
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}