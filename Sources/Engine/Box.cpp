#include "Chicane/Box.hpp"

#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Font/Manager.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Model/Manager.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Sound/Manager.hpp"
#include "Chicane/Box/Texture.hpp"
#include "Chicane/Box/Texture/Manager.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unique_ptr<SoundManager>   g_soundManager   = std::make_unique<SoundManager>();
        static const std::unique_ptr<FontManager>    g_fontManager    = std::make_unique<FontManager>();
        static const std::unique_ptr<ModelManager>   g_modelManager   = std::make_unique<ModelManager>();
        static const std::unique_ptr<TextureManager> g_textureManager = std::make_unique<TextureManager>();

        static std::unordered_map<FileSystem::Path, std::unique_ptr<const Asset>> g_cache = {};

        SoundManager* getSoundManager()
        {
            return g_soundManager.get();
        }

        FontManager* getFontManager()
        {
            return g_fontManager.get();
        }

        ModelManager* getModelManager()
        {
            return g_modelManager.get();
        }

        TextureManager* getTextureManager()
        {
            return g_textureManager.get();
        }

        bool hasAsset(const FileSystem::Path& inSource)
        {
            return g_cache.find(inSource) != g_cache.end();
        }

        template <class T = Asset>
        const T* getAsset(const FileSystem::Path& inSource)
        {
            if (!hasAsset(inSource))
            {
                return nullptr;
            }

            return dynamic_cast<const T*>(g_cache.at(inSource).get());
        }

        template <class T = Asset>
        const T* addAsset(const FileSystem::Path& inSource)
        {
            if (!hasAsset(inSource))
            {
                g_cache.insert(std::make_pair(inSource, std::make_unique<const T>(inSource)));
            }

            return getAsset<T>(inSource);
        }

        const Sound* loadSound(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Sound)
            {
                throw std::runtime_error(inFilePath.string() + " is not a audio");
            }

            if (!hasAsset(inFilePath))
            {
                const Sound* asset = addAsset<Sound>(inFilePath);

                g_soundManager->load(*asset);

                return asset;
            }

            return getAsset<Sound>(inFilePath);
        }

        const Font* loadFont(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Font)
            {
                throw std::runtime_error(inFilePath.string() + " is not a font");
            }

            if (!hasAsset(inFilePath))
            {
                const Font* asset = addAsset<Font>(inFilePath);

                g_fontManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Font>(inFilePath);
        }

        const Model* loadModel(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Model)
            {
                throw std::runtime_error(inFilePath.string() + "is not a model");
            }

            if (!hasAsset(inFilePath))
            {
                const Model* asset = addAsset<Model>(inFilePath);

                g_modelManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Model>(inFilePath);
        }

        const Texture* loadTexture(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Texture)
            {
                throw std::runtime_error(inFilePath.string() + "is not a texture");
            }

            if (!hasAsset(inFilePath))
            {
                const Texture* asset = addAsset<Texture>(inFilePath);

                g_textureManager->load(inFilePath.string(), *asset);

                return asset;
            }

            return getAsset<Texture>(inFilePath);
        }

        const Mesh* loadMesh(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Mesh)
            {
                throw std::runtime_error(inFilePath.string() + "is not a mesh");
            }

            if (!hasAsset(inFilePath))
            {
                const Mesh* asset = addAsset<Mesh>(inFilePath);

                for (const MeshGroup& group : asset->getGroups())
                {
                    const String& model = group.getModel();
                    if (!model.isEmpty() && !hasAsset(model.toStandard()))
                    {
                        loadModel(model.toStandard());
                    }

                    const String& texture = group.getTexture();
                    if (!texture.isEmpty() && !hasAsset(texture.toStandard()))
                    {
                        loadTexture(texture.toStandard());
                    }
                }

                return asset;
            }

            return getAsset<Mesh>(inFilePath);
        }

        const Sky* loadSky(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getType(inFilePath) != AssetType::Sky)
            {
                throw std::runtime_error(inFilePath.string() + " is not a skybox");
            }

            if (!hasAsset(inFilePath))
            {
                const Sky* asset = addAsset<Sky>(inFilePath);

                loadModel(asset->getModel().toStandard());

                for (const auto& [side, texture] : asset->getSides())
                {
                    loadTexture(texture.toStandard());
                }

                return asset;
            }

            return getAsset<Sky>(inFilePath);
        }

        const Asset* load(const FileSystem::Path& inFilePath)
        {
            if (!AssetHeader::isFileAsset(inFilePath))
            {
                Log::warning("File [%s] is not a valid asset", inFilePath.string().c_str());

                return nullptr;
            }

            const AssetHeader header = AssetHeader(inFilePath);

            switch (header.type)
            {
            case AssetType::Font:
                return loadFont(inFilePath);

            case AssetType::Mesh:
                return loadMesh(inFilePath);

            case AssetType::Sound:
                return loadSound(inFilePath);

            case AssetType::Sky:
                return loadSky(inFilePath);

            case AssetType::Texture:
                return loadTexture(inFilePath);

            default:
                return nullptr;
            }
        }

        void init(const String& inPath)
        {
            for (const FileSystem::Item item : FileSystem::ls(inPath.toStandard()))
            {
                if (item.type != FileSystem::ItemType::File)
                {
                    init(item.path);

                    continue;
                }

                if (!AssetHeader::isFileAsset(item.path.toStandard()))
                {
                    continue;
                }

                load(item.path.toStandard());
            }
        }
    }
}