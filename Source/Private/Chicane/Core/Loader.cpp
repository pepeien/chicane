#include "Chicane/Core/Loader.hpp"

namespace Chicane
{
    namespace Loader
    {
        std::unique_ptr<Audio::Manager>   m_audioManager   = std::make_unique<Audio::Manager>();
        std::unique_ptr<CubeMap::Manager> m_cubeMapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager>   m_modelManager   = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, const Box::Asset::Instance*> m_cache {};

        Audio::Manager* getAudioManager()
        {
            return m_audioManager.get();
        }

        CubeMap::Manager* getCubeMapManager()
        {
            return m_cubeMapManager.get();
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

        bool isLoaded(const Box::Asset::Instance* inInstance)
        {
            if (!inInstance)
            {
                return false;
            }

            return isLoaded(inInstance->getFilepath().string());
        }

        void cacheAsset(const std::string& inIdentifier, const Box::Asset::Instance* inInstance)
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

        template<class T = Box::Asset::Instance>
        const T* getAsset(const std::string& inIdentifier)
        {
            if (!isLoaded(inIdentifier))
            {
                return nullptr;
            }

            return static_cast<const T*>(m_cache.at(inIdentifier));
        }

        void loadModel(const Box::Asset::Instance* inAsset)
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

        void loadTexture(const Box::Asset::Instance* inAsset)
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

            m_textureManager->load(
                filepath,
                static_cast<const Box::Texture*>(inAsset)
            );
        }

        void loadMesh(const Box::Asset::Instance* inAsset)
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
        }

        void loadAudio(const Box::Asset::Instance* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::Audio))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_audioManager->load(
                filepath,
                static_cast<const Box::Audio*>(inAsset)
            );
        }

        void loadCubeMap(const Box::Asset::Instance* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            if (isLoaded(inAsset))
            {
                return;
            }

            if (!inAsset->isType(Box::Asset::Type::CubeMap))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_cubeMapManager->load(
                filepath,
                static_cast<const Box::CubeMap*>(inAsset)
            );
        }

        const Box::Audio* loadAudio(const std::string& inFilePath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilePath);

            if (header.type != Box::Asset::Type::Audio)
            {
                throw std::runtime_error(inFilePath + " is not a audio");
            }

            if (!isLoaded(inFilePath))
            {
                loadAudio(new Box::Audio(inFilePath));
            }

            return getAsset<Box::Audio>(inFilePath);
        }

        const Box::CubeMap* loadCubeMap(const std::string& inFilePath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilePath);

            if (header.type != Box::Asset::Type::CubeMap)
            {
                throw std::runtime_error(inFilePath + " is not a cube map");
            }

            if (!isLoaded(inFilePath))
            {
                loadCubeMap(new Box::CubeMap(inFilePath));
            }

            return getAsset<Box::CubeMap>(inFilePath);
        }

        const Box::Texture* loadTexture(const std::string& inFilePath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilePath);

            if (header.type != Box::Asset::Type::Texture)
            {
                throw std::runtime_error(inFilePath + "is not a texture");
            }

            if (!isLoaded(inFilePath))
            {
                loadTexture(new Box::Texture(inFilePath));
            }

            return getAsset<Box::Texture>(inFilePath);
        }

        const Box::Mesh* loadMesh(const std::string& inFilePath)
        {
            Box::Asset::Header header = Box::Asset::Header::fromFilepath(inFilePath);

            if (header.type != Box::Asset::Type::Mesh)
            {
                throw std::runtime_error(inFilePath + "is not a mesh");
            }

            if (!isLoaded(inFilePath))
            {
                loadMesh(new Box::Mesh(inFilePath));
            }

            return getAsset<Box::Mesh>(inFilePath);
        }

        void reset()
        {
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}