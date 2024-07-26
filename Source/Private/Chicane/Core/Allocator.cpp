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
            std::vector<CachedEntry> entries = {};
        };

        std::unique_ptr<CubeMap::Manager> m_cubemapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager> m_modelManager     = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, CachedInstance> m_cache = {};

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

        void loadMesh(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
            {
                return;
            }

            Box::Instance instance = Box::read(inInstance.filepath);

            CachedInstance cachedInstance = {};
            cachedInstance.type = inInstance.type;

            for (Box::Entry entry : instance.entries)
            {
                if (entry.reference.empty())
                {
                    continue;
                }

                load(entry.reference);

                CachedEntry cachedEntry = {};
                cachedEntry.identifier = entry.reference;
                cachedEntry.type       = entry.type;

                cachedInstance.entries.push_back(cachedEntry);
            }

            cacheInstance(
                inInstance.filepath,
                cachedInstance
            );
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
                throw std::runtime_error("The [" + instance.name + "] cubemap isn't valid");
            }

            std::vector<Box::Entry> textures = {};

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
                inInstance.name,
                Box::read(inInstance.filepath).entries[0]
            );

            CachedEntry cachedEntry = {};
            cachedEntry.type       = Box::EntryType::Model;
            cachedEntry.identifier = inInstance.name;

            CachedInstance cachedInstance = {};
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
                inInstance.name,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        void loadCached(const std::string& inIdentifier)
        {
            if (!isCached(inIdentifier))
            {
                return;
            }

            const CachedInstance& instance = m_cache.at(inIdentifier);

            if (instance.type == Box::Type::Mesh)
            {
                for (const CachedEntry& entry : instance.entries)
                {
                    loadCached(entry.identifier);
                }

                return;
            }

            for (const CachedEntry& entry : instance.entries)
            {
                switch (entry.type)
                {
                case Box::EntryType::Model:
                    m_modelManager->use(entry.identifier);

                    return;
                }
            }
        }

        void load(const Box::Instance& inInstance)
        {
            if (isCached(inInstance))
            {
                return;
            }

            switch (inInstance.type)
            {
            case Box::Type::Mesh:
                loadMesh(inInstance);

                return;

            case Box::Type::CubeMap:
                loadCubemap(inInstance);

                return;

            case Box::Type::Texture:
                loadTexture(inInstance);

                return;

            case Box::Type::Model:
                loadModel(inInstance);

                return;

            default:
                return;
            }
        }

        void load(const std::string& inFilePath)
        {
            if (isCached(inFilePath))
            {
                loadCached(inFilePath);

                return;
            }

            Box::Instance headerOnlyInstance = Box::readHeader(inFilePath);

            load(headerOnlyInstance);
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}