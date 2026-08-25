#include "Chicane/Box.hpp"

#include <cmath>

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Box
    {
        static AssetObservable                                                    g_assetObservable = {};
        static std::unordered_map<FileSystem::Path, std::unique_ptr<const Asset>> g_cache           = {};

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

                g_assetObservable.next(g_cache.at(inSource).get());
            }

            return getAsset<T>(inSource);
        }

        const Sound* loadSound(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Sound)
            {
                throw std::runtime_error(inFilePath.toString() + " is not a audio");
            }

            if (!hasAsset(inFilePath))
            {
                return addAsset<Sound>(inFilePath);
            }

            return getAsset<Sound>(inFilePath);
        }

        const Font* loadFont(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Font)
            {
                throw std::runtime_error(inFilePath.toString() + " is not a font");
            }

            if (!hasAsset(inFilePath))
            {
                return addAsset<Font>(inFilePath);
            }

            return getAsset<Font>(inFilePath);
        }

        const Model* loadModel(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Model)
            {
                throw std::runtime_error(inFilePath.toString() + " is not a model");
            }

            const bool bIsDefault = inFilePath == Model::DEFAULT_SOURCE;

            if (!bIsDefault && !FileSystem::exists(inFilePath))
            {
                return loadModel(Model::DEFAULT_SOURCE);
            }

            const Model* asset = hasAsset(inFilePath) ? getAsset<Model>(inFilePath) : addAsset<Model>(inFilePath);

            if (asset && !asset->getData().empty())
            {
                return asset;
            }

            if (!bIsDefault)
            {
                return loadModel(Model::DEFAULT_SOURCE);
            }

            return asset;
        }

        const Texture* loadTexture(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Texture)
            {
                throw std::runtime_error(inFilePath.toString() + "is not a texture");
            }

            const bool bIsDefault = inFilePath == Texture::DEFAULT_SOURCE;

            if (!bIsDefault && !FileSystem::exists(inFilePath))
            {
                return loadTexture(Texture::DEFAULT_SOURCE);
            }

            const Texture* asset = hasAsset(inFilePath) ? getAsset<Texture>(inFilePath) : addAsset<Texture>(inFilePath);

            if (asset && !asset->isEmpty())
            {
                return asset;
            }

            if (!bIsDefault)
            {
                return loadTexture(Texture::DEFAULT_SOURCE);
            }

            return asset;
        }

        const Mesh* loadMesh(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Mesh)
            {
                throw std::runtime_error(inFilePath.toString() + "is not a mesh");
            }

            if (!hasAsset(inFilePath))
            {
                const Mesh* asset = addAsset<Mesh>(inFilePath);

                for (const MeshGroup& group : asset->getGroups())
                {
                    loadTexture(group.getTexture().getSource());
                    loadModel(group.getModel().getSource());
                }

                return asset;
            }

            return getAsset<Mesh>(inFilePath);
        }

        const Sky* loadSky(const FileSystem::Path& inFilePath)
        {
            if (AssetHeader::getTypeFromExtension(inFilePath) != AssetType::Sky)
            {
                throw std::runtime_error(inFilePath.toString() + " is not a skybox");
            }

            if (!hasAsset(inFilePath))
            {
                const Sky* asset = addAsset<Sky>(inFilePath);

                for (const AssetReference& texture : asset->getTextures())
                {
                    loadTexture(texture.getSource());
                }

                loadModel(asset->getModel().getSource());

                return asset;
            }

            return getAsset<Sky>(inFilePath);
        }

        std::vector<const Asset*> getById(const String& inId)
        {
            std::vector<const Asset*> result;

            for (const auto& [path, asset] : g_cache)
            {
                if (!asset->getId().equals(inId))
                {
                    continue;
                }

                result.push_back(asset.get());
            }

            return result;
        }

        const Font* findFont(const String& inFamily, float inWeight)
        {
            const Font* result    = nullptr;
            float       bestDelta = 0.0f;

            for (const auto& [path, asset] : g_cache)
            {
                const Font* font = dynamic_cast<const Font*>(asset.get());

                if (!font)
                {
                    continue;
                }

                const FontFamily& data = font->getData();

                if (!font->getId().equals(inFamily) && !data.getName().equals(inFamily) &&
                    !data.getFamily().equals(inFamily))
                {
                    continue;
                }

                const bool  bCovers = data.isVariable() && inWeight >= data.getWeightMin() &&
                                     inWeight <= data.getWeightMax();
                const float delta   = bCovers ? 0.0f : std::fabs(data.getWeight() - inWeight);

                if (!result || delta < bestDelta ||
                    (delta == bestDelta && result->getData().isVariable() && !data.isVariable()))
                {
                    result    = font;
                    bestDelta = delta;
                }
            }

            if (result)
            {
                return result;
            }

            return getById<Font>(inFamily);
        }

        void notify(const Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            g_assetObservable.next(inAsset);
        }

        const Asset* load(const FileSystem::Path& inFilePath)
        {
            if (!AssetHeader::isFileAsset(inFilePath))
            {
                Log::warning("File [%s] is not a valid asset", inFilePath.toChar());

                return nullptr;
            }

            const AssetHeader header = AssetHeader(inFilePath);

            switch (header.type)
            {
            case AssetType::Font:
                return loadFont(inFilePath);

            case AssetType::Mesh:
                return loadMesh(inFilePath);

            case AssetType::Model:
                return loadModel(inFilePath);

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

        AssetSubscription watch(
            AssetSubscription::NextCallback     inNext,
            AssetSubscription::ErrorCallback    inError,
            AssetSubscription::CompleteCallback inComplete
        )
        {
            return g_assetObservable.subscribe(inNext, inError, inComplete);
        }
    }
}