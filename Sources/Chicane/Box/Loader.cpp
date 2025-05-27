#include "Chicane/Box/Loader.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unique_ptr<Sound::Manager>   g_soundManager   = std::make_unique<Sound::Manager>();
        static const std::unique_ptr<Font::Manager>    g_fontManager    = std::make_unique<Font::Manager>();
        static const std::unique_ptr<Model::Manager>   g_modelManager   = std::make_unique<Model::Manager>();
        static const std::unique_ptr<Texture::Manager> g_textureManager = std::make_unique<Texture::Manager>();

        static std::unordered_map<FileSystem::Path, std::unique_ptr<const Asset::Instance>> g_cache = {};

        Sound::Manager* getSoundManager()
        {
            return g_soundManager.get();
        }

        Font::Manager* getFontManager()
        {
            return g_fontManager.get();
        }

        Model::Manager* getModelManager()
        {
            return g_modelManager.get();
        }

        Texture::Manager* getTextureManager()
        {
            return g_textureManager.get();
        }

        bool hasAsset(const FileSystem::Path& inSource)
        {
            return g_cache.find(inSource) != g_cache.end();
        }

        template<class T = Asset::Instance>
        const T* getAsset(const FileSystem::Path& inSource)
        {
            if (!hasAsset(inSource))
            {
                return nullptr;
            }

            return dynamic_cast<const T*>(g_cache.at(inSource).get());
        }

        template<class T = Asset::Instance>
        const T* addAsset(const FileSystem::Path& inSource)
        {
            const std::string identifier = inSource.string();

            if (!hasAsset(inSource))
            {
                g_cache.insert(std::make_pair(identifier, std::make_unique<const T>(inSource)));
            }

            return getAsset<T>(inSource);
        }

        const Sound::Instance* loadAudio(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Sound)
            {
                throw std::runtime_error(inFilePath.string() + " is not a audio");
            }

            if (!hasAsset(inFilePath))
            {
                const Sound::Instance* asset = addAsset<Sound::Instance>(inFilePath);

                g_soundManager->load(*asset);

                return asset;
            }

            return getAsset<Sound::Instance>(inFilePath);
        }

        const Font::Instance* loadFont(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Font)
            {
                throw std::runtime_error(inFilePath.string() + " is not a font");
            }

            if (!hasAsset(inFilePath))
            {
                const Font::Instance* asset = addAsset<Font::Instance>(inFilePath);

                g_fontManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Font::Instance>(inFilePath);
        }

        const Model::Instance* loadModel(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Model)
            {
                throw std::runtime_error(inFilePath.string() + "is not a model");
            }

            if (!hasAsset(inFilePath))
            {
                const Model::Instance* asset = addAsset<Model::Instance>(inFilePath);

                g_modelManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Model::Instance>(inFilePath);
        }

        const Texture::Instance* loadTexture(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Texture)
            {
                throw std::runtime_error(inFilePath.string() + "is not a texture");
            }

            if (!hasAsset(inFilePath))
            {
                const Texture::Instance* asset = addAsset<Texture::Instance>(inFilePath);

                g_textureManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Texture::Instance>(inFilePath);
        }

        const Mesh::Instance* loadMesh(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Mesh)
            {
                throw std::runtime_error(inFilePath.string() + "is not a mesh");
            }

            if (!hasAsset(inFilePath))
            {
                const Mesh::Instance* asset = addAsset<Mesh::Instance>(inFilePath);

                for (const Mesh::Group& group : asset->getGroups())
                {
                    const std::string& model = group.getModel();
                    if (!model.empty() && !hasAsset(model))
                    {
                        loadModel(model);
                    }

                    const std::string& texture = group.getTexture();
                    if (!texture.empty() && !hasAsset(texture))
                    {
                        loadTexture(texture);
                    }
                }

                return asset;
            }

            return getAsset<Mesh::Instance>(inFilePath);
        }

        const Sky::Instance* loadSky(const FileSystem::Path& inFilePath)
        {
            if (Asset::getType(inFilePath) != Asset::Type::Sky)
            {
                throw std::runtime_error(inFilePath.string() + " is not a skybox");
            }

            if (!hasAsset(inFilePath))
            {
                const Sky::Instance* asset = addAsset<Sky::Instance>(inFilePath);

                loadModel(asset->getModel());

                for (const auto& [side, texture] : asset->getSides())
                {
                    loadTexture(texture);
                }

                return asset;
            }

            return getAsset<Sky::Instance>(inFilePath);
        }

        const Asset::Instance* load(const FileSystem::Path& inFilePath)
        {
            if (!Box::Asset::isFileAsset(inFilePath))
            {
                Log::warning("File [%s] is not a valid asset", inFilePath.string().c_str());

                return nullptr;
            }

            const Asset::Header header = Asset::Header(inFilePath);

            switch (header.type)
            {
            case Asset::Type::Font:
                return loadFont(inFilePath);

            case Asset::Type::Mesh:
                return loadMesh(inFilePath);

            case Asset::Type::Sound:
                return loadAudio(inFilePath);

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