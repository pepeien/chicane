#include "Chicane/Box/Loader.hpp"

namespace Chicane
{
    namespace Box
    {
        std::unique_ptr<Audio::Manager>   m_audioManager   = std::make_unique<Audio::Manager>();
        std::unique_ptr<Font::Manager>    m_fontManager    = std::make_unique<Font::Manager>();
        std::unique_ptr<Model::Manager>   m_modelManager   = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        std::unordered_map<std::string, const Asset::Instance*> m_cache = {};

        Audio::Manager* getAudioManager()
        {
            return m_audioManager.get();
        }

        Font::Manager* getFontManager()
        {
            return m_fontManager.get();
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

        bool isLoaded(const Asset::Instance* inInstance)
        {
            if (!inInstance)
            {
                return false;
            }

            return isLoaded(inInstance->getFilepath().string());
        }

        void cacheAsset(const std::string& inIdentifier, const Asset::Instance* inInstance)
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

        template<class T = Asset::Instance>
        const T* getAsset(const std::string& inIdentifier)
        {
            if (!isLoaded(inIdentifier))
            {
                return nullptr;
            }

            return static_cast<const T*>(m_cache.at(inIdentifier));
        }

        void loadAudio(const Asset::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Audio))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_audioManager->load(
                filepath,
                static_cast<const Audio::Instance*>(inAsset)
            );
        }

        const Audio::Instance* loadAudio(const std::string& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Audio)
            {
                throw std::runtime_error(inFilePath + " is not a audio");
            }

            if (!isLoaded(inFilePath))
            {
                loadAudio(new Audio::Instance(inFilePath));
            }

            return getAsset<Audio::Instance>(inFilePath);
        }

        void loadFont(const Font::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Font))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_fontManager->load(
                filepath,
                static_cast<const Font::Instance*>(inAsset)
            );
        }

        const Font::Instance* loadFont(const std::string& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Font)
            {
                throw std::runtime_error(inFilePath + " is not a font");
            }

            if (!isLoaded(inFilePath))
            {
                loadFont(new Font::Instance(inFilePath));
            }

            return getAsset<Font::Instance>(inFilePath);
        }

        void loadModel(const Asset::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Model))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_modelManager->load(
                filepath,
                static_cast<const Model::Instance*>(inAsset)
            );
        }

        void loadTexture(const Asset::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Texture))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            m_textureManager->load(
                filepath,
                static_cast<const Texture::Instance*>(inAsset)
            );
        }

        const Texture::Instance* loadTexture(const std::string& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Texture)
            {
                throw std::runtime_error(inFilePath + "is not a texture");
            }

            if (!isLoaded(inFilePath))
            {
                loadTexture(new Texture::Instance(inFilePath));
            }

            return getAsset<Texture::Instance>(inFilePath);
        }

        void loadMesh(const Asset::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Mesh))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            const Mesh::Instance* mesh = static_cast<const Mesh::Instance*>(inAsset);

            for (const Mesh::Group& group : mesh->getGroups())
            {
                const std::string& model = group.getModel();

                if (!model.empty() && !isLoaded(model))
                {
                    loadModel(new Model::Instance(model));
                }

                const std::string& texture = group.getTexture();

                if (!texture.empty() && !isLoaded(texture))
                {
                    loadTexture(new Texture::Instance(texture));
                }
            }
        }

        const Mesh::Instance* loadMesh(const std::string& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Mesh)
            {
                throw std::runtime_error(inFilePath + "is not a mesh");
            }

            if (!isLoaded(inFilePath))
            {
                loadMesh(new Mesh::Instance(inFilePath));
            }

            return getAsset<Mesh::Instance>(inFilePath);
        }

        void loadSky(const Asset::Instance* inAsset)
        {
            if (!inAsset || isLoaded(inAsset) || !inAsset->isType(Asset::Type::Sky))
            {
                return;
            }

            const std::string filepath = inAsset->getFilepath().string();

            cacheAsset(filepath, inAsset);

            const Sky::Instance* cubeMap = static_cast<const Sky::Instance*>(inAsset);

            for (const auto& [side, texture] : cubeMap->getSides())
            {
                loadTexture(texture);
            }

            loadModel(new Model::Instance(cubeMap->getModel()));
        }

        const Sky::Instance* loadSky(const std::string& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Sky)
            {
                throw std::runtime_error(inFilePath + " is not a skybox");
            }

            if (!isLoaded(inFilePath))
            {
                loadSky(new Sky::Instance(inFilePath));
            }

            return getAsset<Sky::Instance>(inFilePath);
        }

        const Asset::Instance* load(const std::string& inFilePath)
        {
            if (!Box::Asset::isFileAsset(inFilePath))
            {
                Log::warning("File [%s] is not a valid asset", inFilePath.c_str());

                return nullptr;
            }

            const Asset::Header header = Asset::Header::fromFilepath(inFilePath);

            switch (header.type)
            {
            case Asset::Type::Audio:
                return loadAudio(inFilePath);

            case Asset::Type::Font:
                return loadFont(inFilePath);

            case Asset::Type::Mesh:
                return loadMesh(inFilePath);

            case Asset::Type::Sky:
                return loadSky(inFilePath);

            case Asset::Type::Texture:
                return loadTexture(inFilePath);
            
            default:
                return nullptr;
            }
        }
    }
}