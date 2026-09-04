#include "Chicane/Box.hpp"

#include <cmath>
#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        static AssetObservable                                                    g_assetObservable   = {};
        static PreviewObservable                                                  g_previewObservable = {};
        static std::unordered_map<FileSystem::Path, std::unique_ptr<const Asset>> g_cache             = {};

        static std::unordered_map<FileSystem::Path, std::unique_ptr<AssetPreview>> g_previewCache   = {};
        static std::list<FileSystem::Path>                                         g_previewOrder   = {};
        static std::unordered_map<FileSystem::Path, std::list<FileSystem::Path>::iterator>
            g_previewOrderIt = {};

        FileSystem::Path normalizePreviewPath(const FileSystem::Path& inFilePath)
        {
            return inFilePath.lexicallyNormal();
        }

        void appendGeometry(
            const ModelParsed::Map& inModels,
            Vertex::List&           outVertices,
            Vertex::Indices&        outIndices
        )
        {
            for (const auto& entry : inModels)
            {
                const ModelParsed& data = entry.second;
                if (data.vertices.empty())
                {
                    continue;
                }

                const Vertex::Index base = static_cast<Vertex::Index>(outVertices.size());
                outVertices.insert(outVertices.end(), data.vertices.begin(), data.vertices.end());

                if (data.indices.empty())
                {
                    for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(data.vertices.size()); i++)
                    {
                        outIndices.push_back(base + i);
                    }

                    continue;
                }

                for (const Vertex::Index index : data.indices)
                {
                    outIndices.push_back(base + index);
                }
            }
        }

        Image::Instance loadTextureImage(const FileSystem::Path& inFilePath)
        {
            if (!FileSystem::exists(inFilePath))
            {
                return nullptr;
            }

            try
            {
                const Texture texture(inFilePath);

                return texture.getData().lock();
            }
            catch (...)
            {
                return nullptr;
            }
        }

        bool loadCubemapFaces(const FileSystem::Path& inTexture, std::vector<Image::Instance>& outFaces)
        {
            static const char* faces[] = {"Right", "Left", "Front", "Back", "Up", "Down"};

            const String stem = inTexture.stem().toString().toLower();
            bool         isFace = false;
            for (const char* face : faces)
            {
                if (stem.equals(String(face).toLower()))
                {
                    isFace = true;

                    break;
                }
            }

            if (!isFace)
            {
                return false;
            }

            const FileSystem::Path directory = inTexture.parent();
            const String           extension = inTexture.extension().toString();
            outFaces.assign(6, nullptr);

            int found = 0;
            for (int i = 0; i < 6; i++)
            {
                FileSystem::Path sibling = directory / (String(faces[i]) + extension);
                outFaces[static_cast<std::size_t>(i)] = loadTextureImage(sibling);
                if (outFaces[static_cast<std::size_t>(i)])
                {
                    found++;
                }
            }

            if (found < 6)
            {
                outFaces.clear();

                return false;
            }

            return true;
        }

        std::unique_ptr<AssetPreview> decodeTexturePreview(const FileSystem::Path& inFilePath, bool inUseStored)
        {
            if (inUseStored)
            {
                if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                {
                    return preview;
                }
            }

            if (!FileSystem::exists(inFilePath))
            {
                return nullptr;
            }

            std::vector<Image::Instance> faces = {};
            if (loadCubemapFaces(inFilePath, faces))
            {
                const Texture texture(inFilePath);
                std::unique_ptr<AssetPreview> preview =
                    AssetPreview::createFromSky(inFilePath, texture.getId(), {}, {}, faces);
                if (preview)
                {
                    preview->type = AssetType::Texture;

                    return preview;
                }
            }

            const Texture      texture(inFilePath);
            Image::Reference data = texture.getData();
            if (data.expired())
            {
                return nullptr;
            }

            const Image::Instance image = data.lock();
            if (!image)
            {
                return nullptr;
            }

            return AssetPreview::create(inFilePath, texture.getId(), AssetType::Texture, *image);
        }

        std::unique_ptr<AssetPreview> decodePreview(const FileSystem::Path& inFilePath, bool inUseStored = true)
        {
            switch (AssetHeader::getTypeFromExtension(inFilePath))
            {
            case AssetType::Texture:
                return decodeTexturePreview(inFilePath, inUseStored);

            case AssetType::Mesh:
            {
                if (!FileSystem::exists(inFilePath))
                {
                    return nullptr;
                }

                if (inUseStored)
                {
                    if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                    {
                        preview->path = inFilePath;
                        preview->type = AssetType::Mesh;

                        return preview;
                    }
                }

                const Mesh mesh(inFilePath);
                if (mesh.getGroups().empty())
                {
                    return nullptr;
                }

                Vertex::List    vertices = {};
                Vertex::Indices indices  = {};

                for (const MeshGroup& group : mesh.getGroups())
                {
                    const FileSystem::Path modelPath = group.getModel().getSource();
                    if (!FileSystem::exists(modelPath))
                    {
                        continue;
                    }

                    const Model             model(modelPath);
                    const ModelParsed::Map& models = model.getData();
                    if (models.empty())
                    {
                        continue;
                    }

                    auto found = models.find(group.getModel().getReference());
                    if (found == models.end())
                    {
                        found = models.begin();
                    }

                    ModelParsed::Map selected = {};
                    selected[found->first]    = found->second;
                    appendGeometry(selected, vertices, indices);
                }

                return AssetPreview::createFromGeometry(inFilePath, mesh.getId(), vertices, indices);
            }

            case AssetType::Model:
            {
                if (!FileSystem::exists(inFilePath))
                {
                    return nullptr;
                }

                if (inUseStored)
                {
                    if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                    {
                        preview->path = inFilePath;
                        preview->type = AssetType::Model;

                        return preview;
                    }
                }

                const Model model(inFilePath);
                Vertex::List    vertices = {};
                Vertex::Indices indices  = {};
                appendGeometry(model.getData(), vertices, indices);

                std::unique_ptr<AssetPreview> preview =
                    AssetPreview::createFromGeometry(inFilePath, model.getId(), vertices, indices);
                if (preview)
                {
                    preview->type = AssetType::Model;
                }

                return preview;
            }

            case AssetType::Sound:
            {
                if (!FileSystem::exists(inFilePath))
                {
                    return nullptr;
                }

                if (inUseStored)
                {
                    if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                    {
                        preview->path = inFilePath;
                        preview->type = AssetType::Sound;

                        return preview;
                    }
                }

                const Sound sound(inFilePath);

                return AssetPreview::createFromSound(inFilePath, sound.getId(), sound.getData());
            }

            case AssetType::Font:
            {
                if (!FileSystem::exists(inFilePath))
                {
                    return nullptr;
                }

                if (inUseStored)
                {
                    if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                    {
                        preview->path = inFilePath;
                        preview->type = AssetType::Font;

                        return preview;
                    }
                }

                const Font        font(inFilePath);
                const FontFamily& family = font.getData();
                String            label  = family.getFamily();
                if (label.isEmpty())
                {
                    label = family.getName();
                }
                if (label.isEmpty())
                {
                    label = font.getId();
                }

                return AssetPreview::createFromFont(inFilePath, font.getId(), family, label);
            }

            case AssetType::Sky:
            {
                if (!FileSystem::exists(inFilePath))
                {
                    return nullptr;
                }

                if (inUseStored)
                {
                    if (std::unique_ptr<AssetPreview> preview = AssetPreview::read(inFilePath))
                    {
                        preview->path = inFilePath;
                        preview->type = AssetType::Sky;

                        return preview;
                    }
                }

                const Sky sky(inFilePath);

                Vertex::List    vertices = {};
                Vertex::Indices indices  = {};
                const FileSystem::Path modelPath = sky.getModel().getSource();
                if (FileSystem::exists(modelPath))
                {
                    const Model             model(modelPath);
                    const ModelParsed::Map& models = model.getData();
                    if (!models.empty())
                    {
                        auto found = models.find(sky.getModel().getReference());
                        if (found == models.end())
                        {
                            found = models.begin();
                        }

                        ModelParsed::Map selected = {};
                        selected[found->first]    = found->second;
                        appendGeometry(selected, vertices, indices);
                    }
                }

                std::vector<Image::Instance> faces = {};
                for (const AssetReference& texture : sky.getTextures())
                {
                    if (Image::Instance image = loadTextureImage(texture.getSource()))
                    {
                        faces.push_back(image);
                    }
                }

                std::unique_ptr<AssetPreview> preview =
                    AssetPreview::createFromSky(inFilePath, sky.getId(), vertices, indices, faces);
                if (preview)
                {
                    preview->type = AssetType::Sky;
                }

                return preview;
            }

            default:
                return nullptr;
            }
        }

        const AssetPreview* touchPreview(const FileSystem::Path& inFilePath)
        {
            const auto found = g_previewCache.find(inFilePath);
            if (found == g_previewCache.end())
            {
                return nullptr;
            }

            const auto order = g_previewOrderIt.find(inFilePath);
            if (order != g_previewOrderIt.end())
            {
                g_previewOrder.splice(g_previewOrder.end(), g_previewOrder, order->second);
                order->second = std::prev(g_previewOrder.end());
            }

            return found->second.get();
        }

        const AssetPreview* insertPreview(std::unique_ptr<AssetPreview> inPreview)
        {
            if (!inPreview)
            {
                return nullptr;
            }

            const FileSystem::Path key = normalizePreviewPath(inPreview->path);
            inPreview->path            = key;

            if (g_previewCache.find(key) == g_previewCache.end())
            {
                while (g_previewCache.size() >= AssetPreview::CACHE_LIMIT && !g_previewOrder.empty())
                {
                    const FileSystem::Path oldest = g_previewOrder.front();
                    g_previewOrder.pop_front();
                    g_previewOrderIt.erase(oldest);
                    g_previewCache.erase(oldest);
                }
            }

            if (const auto order = g_previewOrderIt.find(key); order != g_previewOrderIt.end())
            {
                g_previewOrder.erase(order->second);
                g_previewOrderIt.erase(order);
            }

            g_previewCache[key] = std::move(inPreview);
            g_previewOrder.push_back(key);
            g_previewOrderIt[key] = std::prev(g_previewOrder.end());

            const AssetPreview* preview = g_previewCache.at(key).get();
            g_previewObservable.next(preview);

            return preview;
        }

        struct PreviewService
        {
            PreviewService() = default;

            ~PreviewService()
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    bRunning = false;
                }

                readySignal.notify_all();

                if (worker.joinable())
                {
                    worker.join();
                }
            }

            void start()
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (bRunning)
                {
                    return;
                }

                bRunning = true;
                worker   = std::thread(&PreviewService::loop, this);
            }

            void enqueue(const FileSystem::Path& inFilePath)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (inFlight.find(inFilePath) != inFlight.end())
                    {
                        return;
                    }

                    inFlight.insert(inFilePath);
                    pending.push(inFilePath);
                }

                start();
                readySignal.notify_one();
            }

            void drain(std::vector<std::unique_ptr<AssetPreview>>& outReady)
            {
                std::lock_guard<std::mutex> lock(mutex);
                outReady.swap(ready);
            }

            std::mutex                                 mutex;
            std::condition_variable                    readySignal;
            std::queue<FileSystem::Path>               pending;
            std::unordered_set<FileSystem::Path>       inFlight;
            std::vector<std::unique_ptr<AssetPreview>> ready;
            std::thread                                worker;
            bool                                       bRunning = false;

            void loop()
            {
                while (true)
                {
                    FileSystem::Path path;

                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        readySignal.wait(
                            lock,
                            [this]()
                            {
                                return !bRunning || !pending.empty();
                            }
                        );

                        if (!bRunning && pending.empty())
                        {
                            return;
                        }

                        path = pending.front();
                        pending.pop();
                    }

                    std::unique_ptr<AssetPreview> preview;
                    try
                    {
                        preview = decodePreview(path);
                    }
                    catch (...)
                    {
                        preview.reset();
                    }

                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        inFlight.erase(path);

                        if (preview)
                        {
                            ready.push_back(std::move(preview));
                        }
                    }
                }
            }
        };

        static PreviewService g_previewService = {};

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

                const bool bCovers =
                    data.isVariable() && inWeight >= data.getWeightMin() && inWeight <= data.getWeightMax();
                const float delta = bCovers ? 0.0f : std::fabs(data.getWeight() - inWeight);

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
            if (const Asset* cached = getAsset(inFilePath))
            {
                return cached;
            }

            if (!AssetHeader::isFileAsset(inFilePath))
            {
                Log::warning("File [%s] is not a valid asset", inFilePath.toChar());

                return nullptr;
            }

            switch (AssetHeader::getTypeFromExtension(inFilePath))
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

        const AssetPreview* findPreview(const FileSystem::Path& inFilePath)
        {
            if (inFilePath.isEmpty())
            {
                return nullptr;
            }

            return touchPreview(normalizePreviewPath(inFilePath));
        }

        bool embedPreview(const FileSystem::Path& inFilePath)
        {
            if (inFilePath.isEmpty() || !FileSystem::exists(inFilePath) || !AssetHeader::isFileAsset(inFilePath))
            {
                return false;
            }

            try
            {
                std::unique_ptr<AssetPreview> preview = decodePreview(inFilePath, false);
                if (!preview || !preview->image)
                {
                    return false;
                }

                return AssetPreview::bake(inFilePath, preview->id, preview->type, *preview->image);
            }
            catch (...)
            {
                return false;
            }
        }

        const AssetPreview* loadPreview(const FileSystem::Path& inFilePath)
        {
            if (inFilePath.isEmpty())
            {
                return nullptr;
            }

            const FileSystem::Path path = normalizePreviewPath(inFilePath);
            if (const AssetPreview* cached = touchPreview(path))
            {
                return cached;
            }

            std::unique_ptr<AssetPreview> preview;
            try
            {
                preview = decodePreview(path);
            }
            catch (...)
            {
                return nullptr;
            }

            return insertPreview(std::move(preview));
        }

        void requestPreview(const FileSystem::Path& inFilePath)
        {
            if (inFilePath.isEmpty())
            {
                return;
            }

            const FileSystem::Path path = normalizePreviewPath(inFilePath);
            if (touchPreview(path))
            {
                return;
            }

            g_previewService.enqueue(path);
        }

        void pumpPreview()
        {
            std::vector<std::unique_ptr<AssetPreview>> ready;
            g_previewService.drain(ready);

            for (std::unique_ptr<AssetPreview>& preview : ready)
            {
                insertPreview(std::move(preview));
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

        PreviewSubscription watchPreview(
            PreviewSubscription::NextCallback     inNext,
            PreviewSubscription::ErrorCallback    inError,
            PreviewSubscription::CompleteCallback inComplete
        )
        {
            return g_previewObservable.subscribe(inNext, inError, inComplete);
        }
    }
}