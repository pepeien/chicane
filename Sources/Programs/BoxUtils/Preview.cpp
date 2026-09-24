#include "Preview.hpp"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Animation.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Material.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Core.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Vec.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Renderer/Feature.hpp>
#include <Chicane/Renderer/Instance.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/View.hpp>

namespace
{
    static constexpr int           PREVIEW_SIZE  = Chicane::Box::AssetPreview::SIZE;
    static constexpr std::uint32_t WARMUP_FRAMES = 16;

    static constexpr const char* SHADER_BALL_MODEL = "Assets/Editor/Models/Preview/ShaderBall.bmdl";

    static bool previewFocus(const std::vector<Chicane::Actor*>& inActors, Chicane::Vec3& outCenter, float& outRadius)
    {
        Chicane::Actor* actor = nullptr;
        for (Chicane::Actor* candidate : inActors)
        {
            if (!candidate || candidate->getBounds().getCorners().empty())
            {
                continue;
            }

            actor = candidate;

            break;
        }

        if (!actor)
        {
            return false;
        }

        outCenter                = actor->getCenter();
        const Chicane::Vec3 size = actor->getBounds().getSize();
        outRadius                = 0.5f * std::sqrt(size.dot(size));

        return true;
    }

    static void previewView(const Chicane::Object* inObject, float& outFieldOfView, float& outAspectRatio)
    {
        outFieldOfView = 45.0f;
        outAspectRatio = 1.0f;
        if (!inObject)
        {
            return;
        }

        for (Chicane::Component* component : inObject->getAttachments())
        {
            Chicane::CView* view = dynamic_cast<Chicane::CView*>(component);
            if (!view)
            {
                continue;
            }

            outFieldOfView = view->getFieldOfView();
            if (view->getAspectRatio() > 0.0f)
            {
                outAspectRatio = view->getAspectRatio();
            }

            return;
        }
    }

    class BakeScene : public Chicane::Scene
    {
    public:
        enum class Phase : std::uint8_t
        {
            Load,
            Warmup,
            Done
        };

    public:
        explicit BakeScene(const std::vector<Chicane::FileSystem::Path>& inAssets)
            : Scene(),
              m_assets(inAssets),
              m_index(0),
              m_frames(0),
              m_phase(Phase::Load),
              m_previewType(Chicane::Box::AssetType::Undefined),
              m_previewActors({}),
              m_tempFiles({})
        {}

        ~BakeScene() override
        {
            destroyPreview();
            removeTempFiles();
        }

        void onTick(float) override
        {
            if (m_phase.load(std::memory_order_acquire) != Phase::Load)
            {
                return;
            }

            if (m_index >= m_assets.size())
            {
                m_phase.store(Phase::Done, std::memory_order_release);

                return;
            }

            try
            {
                loadCurrent();
                m_frames.store(0, std::memory_order_relaxed);
                m_phase.store(Phase::Warmup, std::memory_order_release);
            }
            catch (const std::exception& exception)
            {
                const Chicane::FileSystem::Path& asset = m_assets.at(m_index);
                std::cerr << "Failed to load preview for [" << asset.toString() << "]: " << exception.what()
                          << std::endl;
                if (Chicane::Box::embedPreview(asset))
                {
                    std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
                }
                else
                {
                    std::cerr << "Failed to generated a preview for[" << asset.toString() << "]" << std::endl;
                }

                m_index++;
                m_phase.store(m_index >= m_assets.size() ? Phase::Done : Phase::Load, std::memory_order_release);
            }
        }

        void onFrame()
        {
            if (m_phase.load(std::memory_order_acquire) != Phase::Warmup)
            {
                if (m_phase.load(std::memory_order_acquire) == Phase::Done)
                {
                    if (Chicane::Window* window = Chicane::Window::getCurrent())
                    {
                        window->close();
                    }
                }

                return;
            }

            const std::uint32_t frames = m_frames.fetch_add(1, std::memory_order_relaxed) + 1;
            if (frames < WARMUP_FRAMES)
            {
                return;
            }

            const Chicane::FileSystem::Path asset    = m_assets.at(m_index);
            Chicane::Renderer::Instance*    renderer = Chicane::Application::getInstance().getRenderer();
            std::unique_ptr<Chicane::Image> image    = renderer ? renderer->captureScreen() : nullptr;
            if (image)
            {
                image->flipVertically();
            }
            if (image && Chicane::Box::AssetPreview::bake(asset, Chicane::Box::getTypeFromExtension(asset), *image))
            {
                std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
            }
            else if (Chicane::Box::embedPreview(asset))
            {
                std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
            }
            else
            {
                std::cerr << "Failed to generated a preview for[" << asset.toString() << "]" << std::endl;
            }

            m_index++;
            m_phase.store(m_index >= m_assets.size() ? Phase::Done : Phase::Load, std::memory_order_release);
        }

    private:
        void loadCurrent()
        {
            removeTempFiles();
            destroyPreview();

            const Chicane::FileSystem::Path& asset = m_assets.at(m_index);
            const Chicane::Box::AssetType    type  = Chicane::Box::getTypeFromExtension(asset);
            const Chicane::FileSystem::Path  track = Chicane::Box::AssetPreview::trackPath(type);
            if (type != m_previewType)
            {
                if (Chicane::FileSystem::exists(track))
                {
                    open(track);
                }

                m_previewType = type;
                activateCamera();
            }

            if (type == Chicane::Box::AssetType::Model)
            {
                spawnModel(asset);
            }
            else if (type == Chicane::Box::AssetType::Material)
            {
                spawnPreview(asset, type);
            }
            else if (type == Chicane::Box::AssetType::Mesh)
            {
                spawnMesh(asset);
            }

            frameCamera();
        }

        void activateCamera()
        {
            for (Chicane::ACamera* camera : getActors<Chicane::ACamera>())
            {
                if (!camera)
                {
                    continue;
                }

                camera->activate();

                break;
            }
        }

        void frameCamera()
        {
            Chicane::ACamera* camera = nullptr;
            for (Chicane::ACamera* candidate : getActors<Chicane::ACamera>())
            {
                if (!candidate)
                {
                    continue;
                }

                camera = candidate;

                break;
            }

            if (!camera)
            {
                return;
            }

            Chicane::Vec3 pivot  = Chicane::Xml::parseVec3(camera->lookTo, Chicane::Vec3::Zero());
            float         radius = 0.0f;
            Chicane::Vec3 center;
            if (previewFocus(m_previewActors, center, radius))
            {
                pivot = center;
            }

            float fieldOfView = 45.0f;
            float aspectRatio = 1.0f;
            previewView(camera, fieldOfView, aspectRatio);

            const Chicane::Vec3 start = Chicane::Box::AssetPreview::cameraStart(
                pivot,
                camera->getAbsoluteTranslation(),
                Chicane::Box::AssetPreview::cameraDistance(radius, fieldOfView, aspectRatio)
            );
            camera->setAbsoluteTranslation(start);
            camera->lookAt(pivot);
            camera->activate();
        }

        void destroyPreview()
        {
            for (Chicane::Actor* actor : m_previewActors)
            {
                destroyActor(actor);
            }

            m_previewActors.clear();
        }

        void destroyActor(Chicane::Actor* inActor)
        {
            if (!inActor)
            {
                return;
            }

            const std::vector<Chicane::Component*> attachments = inActor->getAttachments();
            for (Chicane::Component* component : attachments)
            {
                if (!component)
                {
                    continue;
                }

                component->detach();
                removeComponent(component);
                delete component;
            }

            removeActor(inActor);
            delete inActor;
        }

        void spawnMesh(const Chicane::FileSystem::Path& inMesh)
        {
            Chicane::Actor* actor = createActor<Chicane::Actor>();
            m_previewActors.push_back(actor);
            actor->setId("Preview");

            Chicane::CMesh* mesh = createComponent<Chicane::CMesh>();
            mesh->setId("PreviewMesh");
            mesh->setMesh(inMesh);
            mesh->attachTo(actor);
            mesh->activate();

            const std::vector<const Chicane::Box::Animation*>& animations = mesh->getAnimations();
            if (!animations.empty() && animations.front())
            {
                mesh->playAnimation(animations.front()->getId());
            }
        }

        void spawnModel(const Chicane::FileSystem::Path& inModel)
        {
            const Chicane::Box::Model model(inModel);
            if (model.getData().empty())
            {
                return;
            }

            std::error_code             error;
            const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-boxutils";
            std::filesystem::create_directories(directory, error);
            if (error)
            {
                return;
            }

            const Chicane::FileSystem::Path path =
                Chicane::FileSystem::Path(directory) /
                (inModel.stem().toString() + "_model" + Chicane::Box::Mesh::EXTENSION);

            std::vector<Chicane::Box::MeshGroup> groups;
            for (const auto& [name, parsed] : model.getData())
            {
                if (parsed.vertices.empty() || name.isEmpty())
                {
                    continue;
                }

                Chicane::Box::MeshGroup group;
                group.setId(name);
                group.setModel(inModel.toString(), name);
                group.setMaterial(Chicane::Box::Material::DEFAULT_SOURCE, Chicane::Box::Material::DEFAULT_REFERENCE);
                if (!group.isValid())
                {
                    continue;
                }

                groups.push_back(group);
            }

            if (groups.empty())
            {
                return;
            }

            Chicane::Box::Mesh mesh(path);
            mesh.setId(model.getId().isEmpty() ? inModel.stem().toString() : model.getId());
            mesh.setGroups(groups);
            mesh.saveXML();
            m_tempFiles.push_back(path);

            spawnMesh(path);
        }

        void spawnPreview(const Chicane::FileSystem::Path& inAsset, Chicane::Box::AssetType inType)
        {
            Chicane::String materialSource;
            Chicane::String materialReference;
            Chicane::String previewId = inAsset.stem().toString();

            if (inType == Chicane::Box::AssetType::Material)
            {
                const Chicane::Box::Material material(inAsset);
                materialSource    = inAsset.toString();
                materialReference = material.getId().isEmpty() ? previewId : material.getId();
                previewId         = materialReference;
            }
            else
            {
                return;
            }

            Chicane::Box::MeshGroup group;
            group.setId("Body");
            group.setModel(SHADER_BALL_MODEL, Chicane::Box::Model::DEFAULT_REFERENCE);
            group.setMaterial(materialSource, materialReference);
            if (!group.isValid())
            {
                return;
            }

            std::error_code             error;
            const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-boxutils";
            std::filesystem::create_directories(directory, error);
            if (error)
            {
                return;
            }

            const Chicane::FileSystem::Path path =
                Chicane::FileSystem::Path(directory) / (previewId + Chicane::Box::Mesh::EXTENSION);

            Chicane::Box::Mesh mesh(path);
            mesh.setId(previewId);
            mesh.setGroups({group});
            mesh.saveXML();
            m_tempFiles.push_back(path);

            spawnMesh(path);
        }

        void removeTempFiles()
        {
            for (const Chicane::FileSystem::Path& path : m_tempFiles)
            {
                std::error_code error;
                std::filesystem::remove(path.toStandard(), error);
            }

            m_tempFiles.clear();
        }

    private:
        std::vector<Chicane::FileSystem::Path> m_assets;
        std::size_t                            m_index;
        std::atomic<std::uint32_t>             m_frames;
        std::atomic<Phase>                     m_phase;
        Chicane::Box::AssetType                m_previewType;
        std::vector<Chicane::Actor*>           m_previewActors;
        std::vector<Chicane::FileSystem::Path> m_tempFiles;
    };
}

bool bakePreviewsWithRuntime(const std::vector<Chicane::FileSystem::Path>& inAssets)
{
    if (inAssets.empty())
    {
        return true;
    }

    std::shared_ptr<BakeScene> scene = std::make_shared<BakeScene>(inAssets);

    Chicane::ApplicationCreateInfo createInfo;
    createInfo.window.title     = "Chicane BoxUtils";
    createInfo.window.size      = Chicane::Vec<2, std::uint32_t>(PREVIEW_SIZE, PREVIEW_SIZE);
    createInfo.window.display   = 0;
    createInfo.window.type      = Chicane::WindowType::Windowed;
    createInfo.window.bIsHidden = true;
#if CHICANE_VULKAN
    createInfo.window.backend = Chicane::WindowBackend::Vulkan;
#elif CHICANE_OPENGL
    createInfo.window.backend = Chicane::WindowBackend::OpenGL;
#endif
    createInfo.renderer.resolution = createInfo.window.size;

    createInfo.onSetup = [scene]()
    {
        scene->load();
        Chicane::Application::getInstance().setScene(scene);
        if (Chicane::Renderer::Instance* renderer = Chicane::Application::getInstance().getRenderer())
        {
            renderer->disableFeature(Chicane::Renderer::RendererFeature::Bloom);
            renderer->disableFeature(Chicane::Renderer::RendererFeature::HDR);
            renderer->disableFeature(Chicane::Renderer::RendererFeature::Outline);
        }
        if (Chicane::Window* window = Chicane::Window::getCurrent())
        {
            window->disableResizing();
            window->hide();
        }
    };
    createInfo.onFrame = [scene]() { scene->onFrame(); };

    Chicane::Application::getInstance().run(createInfo);

    return true;
}
