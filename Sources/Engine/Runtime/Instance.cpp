#include "Chicane/Runtime/Instance.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <typeinfo>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Asset/Preview/Upload.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Material.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Module.hpp"
#include "Chicane/Core/Texture/Material.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Engine.hpp"

#include "Chicane/Drift.hpp"

#include "Chicane/Screech.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Engine.hpp"
#include "Chicane/Smoke/Particle.hpp"

#include "Chicane/Grid/Component/Viewport.hpp"

#include "Chicane/Core/Script/Channel.hpp"

#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Draw/Glyph/Data.hpp"
#include "Chicane/Renderer/Draw/Particle.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Poly/Mode.hpp"
#include "Chicane/Renderer/Draw/Poly/Topology.hpp"
#include "Chicane/Renderer/Draw/Sky/Kind.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"

#include "Chicane/Runtime/Scene/Actor/Sky.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"
#include "Chicane/Runtime/Scene/Component/Physics.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Cone.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Cylinder.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Rectangle.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

namespace Chicane
{
    std::shared_ptr<std::vector<Renderer::DrawPolyData>> g_pendingPolys =
        std::make_shared<std::vector<Renderer::DrawPolyData>>();
    std::shared_ptr<std::vector<Renderer::DrawTextureData>> g_pendingTextures =
        std::make_shared<std::vector<Renderer::DrawTextureData>>();
    std::shared_ptr<std::vector<Renderer::DrawGlyphData>> g_pendingGlyphs =
        std::make_shared<std::vector<Renderer::DrawGlyphData>>();

    static String resolveModelDrawId(const Box::AssetReference& inReference)
    {
        const Box::Model* model = Box::load<Box::Model>(inReference.getSource());
        if (!model)
        {
            model = Box::Model::sGetDefault();
        }

        if (!model)
        {
            return inReference.getReference();
        }

        return model->getUniqueId(inReference.getReference());
    }

    static String resolveDefaultModelDrawId()
    {
        const Box::Model* model = Box::Model::sGetDefault();
        if (!model)
        {
            return Box::Model::DEFAULT_REFERENCE;
        }

        return model->getUniqueId(Box::Model::DEFAULT_REFERENCE);
    }

    template <typename T>
    static void enqueuePending(std::shared_ptr<std::vector<T>>& ioQueue, const T& inValue)
    {
        std::shared_ptr<std::vector<T>> current = std::atomic_load_explicit(&ioQueue, std::memory_order_acquire);

        while (true)
        {
            std::shared_ptr<std::vector<T>> next = std::make_shared<std::vector<T>>(*current);
            next->push_back(inValue);

            if (std::atomic_compare_exchange_weak_explicit(
                    &ioQueue,
                    &current,
                    next,
                    std::memory_order_release,
                    std::memory_order_acquire
                ))
            {
                return;
            }
        }
    }

    template <typename T>
    static std::shared_ptr<std::vector<T>> drainPending(std::shared_ptr<std::vector<T>>& ioQueue)
    {
        return std::atomic_exchange_explicit(&ioQueue, std::make_shared<std::vector<T>>(), std::memory_order_acq_rel);
    }

    static std::size_t nextSceneWriteIndex(std::size_t inCurrent, std::size_t inBusy, std::size_t inCount)
    {
        if (inCount == 0)
        {
            return 0;
        }

        std::size_t next = (inCurrent + 1) % inCount;
        if (next == inBusy)
        {
            next = (next + 1) % inCount;
        }

        return next;
    }

    static Renderer::Draw::Id resolvePolyId(
        Renderer::Instance* inRenderer, const Renderer::Draw::Reference& inReference
    )
    {
        if (!inRenderer)
        {
            return Renderer::Draw::InvalidId;
        }

        Renderer::Draw::Id id = inRenderer->findPoly(Renderer::DrawPolyType::e3D, inReference);
        if (id > Renderer::Draw::InvalidId)
        {
            return id;
        }

        return inRenderer->findPoly(Renderer::DrawPolyType::e3D, resolveDefaultModelDrawId());
    }

    static Renderer::Draw::Id resolveTextureId(
        Renderer::Instance* inRenderer, const Renderer::Draw::Reference& inReference, bool inUseDefault
    )
    {
        if (!inRenderer)
        {
            return Renderer::Draw::InvalidId;
        }

        if (!inReference.isEmpty())
        {
            const Renderer::Draw::Id id = inRenderer->findTexture(inReference);
            if (id > Renderer::Draw::InvalidId)
            {
                return id;
            }
        }

        if (!inUseDefault)
        {
            return Renderer::Draw::InvalidId;
        }

        return inRenderer->findTexture(Box::Texture::DEFAULT_REFERENCE);
    }

    static Renderer::DrawPoly3DCommandPoly makeLinePoly(
        Vertex::List inVertices, Renderer::DrawPoly3DFlag inFlags = Renderer::DrawPoly3DFlag::None
    )
    {
        Renderer::DrawPoly3DCommandPoly poly;
        poly.data.mode      = Renderer::DrawPolyMode::Line;
        poly.data.topology  = Renderer::DrawPolyTopology::LineList;
        poly.data.vertices  = std::move(inVertices);
        poly.instance.flags = inFlags;

        return poly;
    }

    static void appendTrace(Vertex::List& outVertices, const SceneTraceRequest& inRequest, const Vec4& inColor)
    {
        if (!inRequest.isValid() || !inRequest.shape)
        {
            return;
        }

        if (dynamic_cast<const SceneTraceShapeLine*>(inRequest.shape.get()))
        {
            Renderer::Debug::appendSegment(outVertices, inRequest.origin, inRequest.destination, inColor);

            return;
        }

        if (const SceneTraceShapeRectangle* rectangle =
                dynamic_cast<const SceneTraceShapeRectangle*>(inRequest.shape.get()))
        {
            Renderer::Debug::appendRectangle(
                outVertices,
                inRequest.origin,
                inRequest.destination,
                rectangle->halfExtents,
                inColor
            );

            return;
        }

        if (const SceneTraceShapeCylinder* cylinder =
                dynamic_cast<const SceneTraceShapeCylinder*>(inRequest.shape.get()))
        {
            Renderer::Debug::appendRadial(
                outVertices,
                inRequest.origin,
                inRequest.destination,
                cylinder->getRadiusAt(0.0f),
                cylinder->getRadiusAt(1.0f),
                inColor,
                cylinder->segmentCount
            );

            return;
        }

        if (const SceneTraceShapeCone* cone = dynamic_cast<const SceneTraceShapeCone*>(inRequest.shape.get()))
        {
            const float length = SceneTraceShapeUtility::axisLength(inRequest.origin, inRequest.destination);

            Renderer::Debug::appendRadial(
                outVertices,
                inRequest.origin,
                inRequest.destination,
                cone->getRadiusAt(0.0f, length),
                cone->getRadiusAt(1.0f, length),
                inColor,
                cone->segmentCount
            );
        }
    }

    Instance& Instance::sInstance()
    {
        static Instance instance;

        return instance;
    }

    Instance::Instance()
        : m_telemetry({}),
          m_bIsRunning(false),
          m_controller(nullptr),
          m_controllerObservable({}),
          m_scene(nullptr),
          m_sceneThread({}),
          m_sceneCommandBuffers({}),
          m_sceneWriteIndex(0),
          m_sceneReadIndex(1),
          m_sceneBusyIndex(3),
          m_sceneObservable({}),
          m_view(nullptr),
          m_viewThread({}),
          m_viewCommandBuffers({}),
          m_viewDrawables({}),
          m_viewWriteIndex(0),
          m_viewReadIndex(1),
          m_screenViewportX(0),
          m_screenViewportY(0),
          m_screenViewportWidth(0),
          m_screenViewportHeight(0),
          m_viewObservable({}),
          m_window(nullptr),
          m_renderer(nullptr),
          m_featureFlags(0),
          m_rendererWidth(0),
          m_rendererHeight(0)
    {
        m_sceneCommandBuffers.resize(3);
        m_sceneBusyIndex.store(m_sceneCommandBuffers.size(), std::memory_order_relaxed);
        m_viewCommandBuffers.resize(2);
    }

    void Instance::run(const InstanceCreateInfo& inCreateInfo)
    {
        initWindow(inCreateInfo.window);
        initRenderer(inCreateInfo.renderer);
        initBox();
        initKerb();
        initDrift();
        initScreech();
        initSmoke();
        initModules(inCreateInfo.modules);

        const std::function<void()> onFrame = inCreateInfo.onFrame;

        if (inCreateInfo.onSetup)
        {
            inCreateInfo.onSetup();
        }

        m_bIsRunning.store(true, std::memory_order_seq_cst);
        initScene();
        initUI();

        while (m_window->run())
        {
            if (hasController())
            {
                m_controller->repeat();
            }

            render();

            if (onFrame)
            {
                onFrame();
            }

            m_telemetry.renderer.frame.set(m_renderer ? m_renderer->getGpuDelta() : 0.0f);
        }

        m_bIsRunning.store(false, std::memory_order_seq_cst);

        shutdownScene();
        shutdownUI();
        shutdownDrift();
        shutdownSmoke();
        shutdownModules();
        shutdownRenderer();

        m_renderer.reset();
        m_window.reset();
    }

    void Instance::render()
    {
        snapshotRendererState();
        uploadPreviewTextures();
        renderScene();
        renderUI();

        m_renderer->render();
    }

    void Instance::uploadPreviewTextures()
    {
        if (!m_renderer)
        {
            return;
        }

        if (std::shared_ptr<std::vector<Renderer::DrawPolyData>> polys = drainPending(g_pendingPolys))
        {
            for (const Renderer::DrawPolyData& data : *polys)
            {
                m_renderer->loadPoly(Renderer::DrawPolyType::e3D, data);
            }
        }

        if (std::shared_ptr<std::vector<Renderer::DrawTextureData>> textures = drainPending(g_pendingTextures))
        {
            for (const Renderer::DrawTextureData& data : *textures)
            {
                m_renderer->loadTexture(data);
            }
        }

        if (std::shared_ptr<std::vector<Renderer::DrawGlyphData>> glyphs = drainPending(g_pendingGlyphs))
        {
            for (const Renderer::DrawGlyphData& data : *glyphs)
            {
                m_renderer->loadGlyph(data);
            }
        }

        std::vector<Box::PreviewUpload> pending;
        Box::PreviewUpload::sDrain(pending);

        for (const Box::PreviewUpload& upload : pending)
        {
            if (!upload.image)
            {
                continue;
            }

            Renderer::DrawTextureData data;
            data.reference   = upload.reference;
            data.image       = upload.image;
            data.bStreamable = false;

            m_renderer->loadTexture(data);
        }
    }

    void Instance::snapshotRendererState()
    {
        if (!m_renderer)
        {
            m_featureFlags.store(0, std::memory_order_relaxed);
            m_rendererWidth.store(0, std::memory_order_relaxed);
            m_rendererHeight.store(0, std::memory_order_relaxed);

            return;
        }

        const Vec<2, std::uint32_t> resolution = m_renderer->getResolution();

        m_featureFlags.store(static_cast<std::uint16_t>(m_renderer->getFeature()), std::memory_order_relaxed);
        m_rendererWidth.store(resolution.x, std::memory_order_relaxed);
        m_rendererHeight.store(resolution.y, std::memory_order_relaxed);
    }

    bool Instance::hasSceneFeature(Renderer::RendererFeature inFeature) const
    {
        return (static_cast<Renderer::RendererFeature>(m_featureFlags.load(std::memory_order_relaxed)) & inFeature) ==
               inFeature;
    }

    Vec<2, std::uint32_t> Instance::getRendererResolution() const
    {
        return {m_rendererWidth.load(std::memory_order_relaxed), m_rendererHeight.load(std::memory_order_relaxed)};
    }

    const InstanceTelemetry& Instance::getTelemetry() const
    {
        return m_telemetry;
    }

    bool Instance::hasController()
    {
        return m_controller != nullptr;
    }

    Controller* Instance::getController()
    {
        return m_controller;
    }

    void Instance::setController(Controller* inController)
    {
        if (inController == m_controller)
        {
            return;
        }

        m_controller = inController;

        m_controllerObservable.next(m_controller);
    }

    Instance::ControllerSubscription Instance::watchController(
        ControllerSubscription::NextCallback     inNext,
        ControllerSubscription::ErrorCallback    inError,
        ControllerSubscription::CompleteCallback inComplete
    )
    {
        return m_controllerObservable.subscribe(inNext, inError, inComplete).next(m_controller);
    }

    Instance::SceneSubscription Instance::watchScene(
        SceneSubscription::NextCallback     inNext,
        SceneSubscription::ErrorCallback    inError,
        SceneSubscription::CompleteCallback inComplete
    )
    {
        return m_sceneObservable.subscribe(inNext, inError, inComplete).next(getScene());
    }

    Instance::ViewSubscription Instance::watchView(
        ViewSubscription::NextCallback     inNext,
        ViewSubscription::ErrorCallback    inError,
        ViewSubscription::CompleteCallback inComplete
    )
    {
        return m_viewObservable.subscribe(inNext, inError, inComplete).next(getView());
    }

    bool Instance::hasWindow() const
    {
        return m_window && m_window.get() != nullptr;
    }

    Window* Instance::getWindow() const
    {
        if (!hasWindow())
        {
            return nullptr;
        }

        return m_window.get();
    }

    Vec<2, std::uint32_t> Instance::getScreenViewport() const
    {
        const std::uint32_t width  = m_screenViewportWidth.load(std::memory_order_relaxed);
        const std::uint32_t height = m_screenViewportHeight.load(std::memory_order_relaxed);

        if (width > 0 && height > 0)
        {
            return {width, height};
        }

        return getRendererResolution();
    }

    Bounds2D Instance::getScreenViewportRect() const
    {
        const std::uint32_t width  = m_screenViewportWidth.load(std::memory_order_relaxed);
        const std::uint32_t height = m_screenViewportHeight.load(std::memory_order_relaxed);

        if (width == 0 || height == 0)
        {
            return {};
        }

        const std::uint32_t x = m_screenViewportX.load(std::memory_order_relaxed);
        const std::uint32_t y = m_screenViewportY.load(std::memory_order_relaxed);

        Bounds2D result = {};
        result.set(
            static_cast<float>(y),
            static_cast<float>(x),
            static_cast<float>(y + height),
            static_cast<float>(x + width)
        );

        return result;
    }

    bool Instance::hasRenderer() const
    {
        return m_renderer && m_renderer.get() != nullptr;
    }

    Renderer::Instance* Instance::getRenderer() const
    {
        if (!hasRenderer())
        {
            return nullptr;
        }

        return m_renderer.get();
    }

    void Instance::setRenderer(WindowBackend inBackend)
    {
        if (!hasWindow())
        {
            return;
        }

        shutdownRenderer();

        m_window->setBackend(inBackend);
    }

    void Instance::initWindow(const WindowSettings& inSettings)
    {
        if (hasWindow())
        {
            return;
        }

        m_window = std::make_unique<Window>();
        m_window->init(inSettings);
        m_window->watchEvent(
            [this](const WindowEvent& inEvent)
            {
                if (std::shared_ptr<Grid::View> view = getView())
                {
                    view->post(inEvent);

                    if (m_window && !m_window->isFocused())
                    {
                        m_window->setCursor(view->getPointer());
                    }
                }
            }
        );
        m_window->watchBackend(
            [this](WindowBackend inValue)
            {
                if (hasRenderer())
                {
                    m_renderer->reloadBackend();
                }
            }
        );
    }

    void Instance::initRenderer(const Renderer::Settings& inSettings)
    {
        if (hasRenderer() || !hasWindow())
        {
            return;
        }

        m_renderer = std::make_unique<Renderer::Instance>();
        m_renderer->init(inSettings);
        m_renderer->setWindow(m_window.get());
        snapshotRendererState();
    }

    void Instance::shutdownRenderer()
    {
        if (!hasRenderer())
        {
            return;
        }

        m_renderer->shutdown();
    }

    void Instance::initModules(const std::vector<FileSystem::Path>& inModules)
    {
        for (const FileSystem::Path& path : inModules)
        {
            if (path.isEmpty())
            {
                continue;
            }

            Module::load(path);
        }
    }

    void Instance::shutdownModules()
    {
        Module::unloadAll();
    }

    void Instance::initBox()
    {
        Box::watch(
            [&](const Box::Asset* inAsset)
            {
                if (typeid(*inAsset) == typeid(Box::Model))
                {
                    const Box::Model* model = static_cast<const Box::Model*>(inAsset);

                    for (const auto& [id, polygon] : model->getData())
                    {
                        Renderer::DrawPolyData data;
                        data.reference = model->getUniqueId(id);
                        data.mode      = Renderer::DrawPolyMode::Fill;
                        data.vertices  = polygon.vertices;
                        data.indices   = polygon.indices;

                        enqueuePending(g_pendingPolys, data);
                    }

                    return;
                }

                if (typeid(*inAsset) == typeid(Box::Texture))
                {
                    const Box::Texture* texture = static_cast<const Box::Texture*>(inAsset);
                    const std::size_t   count   = std::max<std::size_t>(1, texture->getFrameCount());

                    for (std::size_t i = 0; i < count; i++)
                    {
                        Renderer::DrawTextureData data;
                        data.reference   = texture->getFrameId(i);
                        data.image       = texture->getFrame(i).lock();
                        data.mips        = texture->getMipChain(i);
                        data.bStreamable = true;

                        enqueuePending(g_pendingTextures, data);
                    }

                    return;
                }

                if (typeid(*inAsset) == typeid(Box::Font))
                {
                    const Box::Font* font = static_cast<const Box::Font*>(inAsset);

                    for (const Box::FontFamily* instance : font->getInstances())
                    {
                        if (!instance)
                        {
                            continue;
                        }

                        for (const auto& [code, glyph] : instance->getGlyphs())
                        {
                            if (glyph.curves.empty())
                            {
                                continue;
                            }

                            Renderer::DrawGlyphData data;
                            data.reference = glyph.name;
                            data.boundsMin = glyph.boundsMin;
                            data.boundsMax = glyph.boundsMax;

                            data.points.reserve(glyph.curves.size() * 3);
                            for (const Box::FontGlyphCurve& curve : glyph.curves)
                            {
                                data.points.push_back(curve.start);
                                data.points.push_back(curve.control);
                                data.points.push_back(curve.end);
                            }

                            enqueuePending(g_pendingGlyphs, data);
                        }
                    }

                    return;
                }
            }
        );

        Box::watchPreview(
            [](const Box::AssetPreview* inPreview)
            {
                if (!inPreview || !inPreview->image)
                {
                    return;
                }

                Box::PreviewUpload::sEnqueue(inPreview->textureId(), inPreview->image);
            }
        );

        Box::load(Box::Font::DEFAULT_SOURCE);
        Box::load(Box::Model::DEFAULT_SOURCE);
        Box::load(Box::Mesh::SPHERE_SOURCE);
        Box::load(Box::Texture::DEFAULT_SOURCE);

        uploadPreviewTextures();
    }

    void Instance::initKerb()
    {
        Kerb::init();
    }

    void Instance::initDrift()
    {
        Drift::init();
    }

    void Instance::shutdownDrift()
    {
        Drift::shutdown();
    }

    void Instance::initScreech()
    {
        Screech::init();
    }

    void Instance::initSmoke()
    {
        Smoke::init();
    }

    void Instance::shutdownSmoke()
    {
        Smoke::shutdown();
    }

    void Instance::initScene()
    {
        m_sceneThread = std::thread(&Instance::tickScene, this);
    }

    void Instance::shutdownScene()
    {
        if (m_sceneThread.joinable())
        {
            m_sceneThread.join();
        }
    }

    void Instance::tickScene()
    {
        Telemetry telemetry = {};

        while (m_bIsRunning)
        {
            telemetry.start();
            {
                std::shared_ptr<Scene> scene = getScene();

                if (!scene)
                {
                    std::this_thread::yield();

                    continue;
                }

                scene->pumpEvents();

                m_telemetry.physics.start();
                {
                    Kerb::Engine::sInstance().tick(telemetry.frame.delta * 0.001f);
                }
                m_telemetry.physics.end();

                m_telemetry.animation.start();
                {
                    Drift::tick(telemetry.frame.delta);
                }
                m_telemetry.animation.end();

                m_telemetry.scene.start();
                {
                    scene->tick(telemetry.frame.delta);

                    Smoke::tick(telemetry.frame.delta * 0.001f);

                    buildSceneCommands(scene);
                }
                m_telemetry.scene.end();

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            telemetry.end();
        }
    }

    void Instance::buildSceneCommands(std::shared_ptr<Scene> inScene)
    {
        if (!inScene)
        {
            return;
        }

        const std::size_t index = m_sceneWriteIndex.load(std::memory_order_relaxed);

        Renderer::DrawPoly3DCommand& command = m_sceneCommandBuffers.at(index);
        command.clear();

        CCamera*                    activeCamera       = nullptr;
        const Vec<2, std::uint32_t> screenViewport     = getScreenViewport();
        const Vec<2, std::uint32_t> rendererResolution = getRendererResolution();
        for (CCamera* camera : inScene->getActiveComponents<CCamera>())
        {
            camera->onResize(screenViewport);

            command.camera = camera->getData();

            activeCamera = camera;
        }

        for (CLight* light : inScene->getActiveComponents<CLight>())
        {
            light->onResize(rendererResolution);

            command.lights.push_back(light->getLight());
        }

        auto submitMesh = [&](CMesh* mesh)
        {
            if (!mesh || !mesh->isActive() || !mesh->hasMesh())
            {
                return;
            }

            if (activeCamera != nullptr && !activeCamera->canSee(mesh))
            {
                return;
            }

            const Mat4& matrix = mesh->getMatrix();

            for (const Box::MeshGroup& group : mesh->getMesh()->getGroups())
            {
                Renderer::DrawPoly3DCommandMesh subcommand;
                subcommand.model                     = resolveModelDrawId(group.getModel());
                subcommand.instance.model            = matrix * mesh->getGroupMatrix(group);
                subcommand.instance.flags            = mesh->getFlags();
                subcommand.instance.emissiveStrength = group.getEmissiveStrength();
                subcommand.instance.tileSize         = group.getTileSize();

                const Box::Material* material = nullptr;
                if (group.hasMaterial())
                {
                    material = Box::load<Box::Material>(group.getMaterial().getSource());
                }

                for (std::uint8_t slot = 0; slot < TEXTURE_MATERIAL_COUNT; slot++)
                {
                    const TextureMaterial type = static_cast<TextureMaterial>(slot);
                    if (material && material->hasTexture(type))
                    {
                        subcommand.textures[slot] = material->getTexture(type).getReference();
                    }
                    else if (type == TextureMaterial::Albedo)
                    {
                        subcommand.textures[slot] = Box::Texture::DEFAULT_REFERENCE;
                    }
                }

                command.meshes.emplace_back(std::move(subcommand));
            }
        };

        if (activeCamera != nullptr)
        {
            inScene->forEachInFrustum(
                activeCamera->getFrustum(),
                [&submitMesh](Object* object)
                {
                    if (typeid(*object) != typeid(CMesh))
                    {
                        return;
                    }

                    submitMesh(static_cast<CMesh*>(object));
                }
            );
        }
        else
        {
            for (CMesh* mesh : inScene->getActiveComponents<CMesh>())
            {
                submitMesh(mesh);
            }
        }

        for (ASky* sky : inScene->getActors<ASky>())
        {
            const Box::Sky* asset = sky->getSky();
            if (!asset)
            {
                continue;
            }

            Renderer::DrawSkyData data;
            data.reference = asset->getFilepath();
            data.model     = resolveModelDrawId(asset->getModel());
            data.kind      = asset->getKind() == Box::SkyKind::Panorama ? Renderer::DrawSkyKind::Panorama
                                                                        : Renderer::DrawSkyKind::Cube;
            data.exposure  = sky->getExposure();
            data.bVisible  = sky->isVisible();

            for (const Box::AssetReference& texture : asset->getTextures())
            {
                data.textures.push_back(texture.getReference());
            }

            command.sky = data;
        }

        for (const Smoke::Particle& particle : Smoke::Engine::sInstance().getParticles())
        {
            Renderer::DrawParticle draw;
            draw.positionRotation = Vec4(particle.position, particle.rotation);
            draw.sizeAge          = Vec4(particle.size.x, particle.size.y, particle.age, particle.additive);
            draw.color            = particle.color;
            draw.axis             = Vec4(particle.axis, 0.0f);

            command.particles.push_back(draw);
        }

        Vertex::List debugLines;
        if (hasSceneFeature(Renderer::RendererFeature::Bounds))
        {
            for (Actor* actor : inScene->getActors())
            {
                if (!actor)
                {
                    continue;
                }

                Renderer::Debug::appendBounds(debugLines, actor->getBounds(), Renderer::Debug::BOUNDS_COLOR);
            }
        }

        if (hasSceneFeature(Renderer::RendererFeature::Colliders))
        {
            for (CPhysics* physics : inScene->getComponents<CPhysics>())
            {
                if (!physics)
                {
                    continue;
                }

                physics->appendDebugWireframe(debugLines, Renderer::Debug::COLLIDER_COLOR);
            }
        }

        Vertex::List skeletonLines;
        if (hasSceneFeature(Renderer::RendererFeature::Skeletons))
        {
            for (CMesh* mesh : inScene->getComponents<CMesh>())
            {
                if (!mesh)
                {
                    continue;
                }

                mesh->appendDebugWireframe(
                    skeletonLines,
                    command.meshes,
                    Box::Model::SPHERE_REFERENCE,
                    Renderer::Debug::SKELETON_COLOR
                );
            }
        }

        if (!debugLines.empty())
        {
            command.polys.push_back(makeLinePoly(std::move(debugLines)));
        }

        if (!skeletonLines.empty())
        {
            command.polys.push_back(makeLinePoly(std::move(skeletonLines), Renderer::DrawPoly3DFlag::Foreground));
        }

        m_sceneReadIndex.store(index, std::memory_order_release);
        m_sceneWriteIndex.store(
            nextSceneWriteIndex(index, m_sceneBusyIndex.load(std::memory_order_acquire), m_sceneCommandBuffers.size()),
            std::memory_order_relaxed
        );
    }

    void Instance::renderScene()
    {
        const std::size_t index = m_sceneReadIndex.load(std::memory_order_acquire);
        m_sceneBusyIndex.store(index, std::memory_order_release);

        const Renderer::DrawPoly3DCommand command = m_sceneCommandBuffers.at(index);

        m_sceneBusyIndex.store(m_sceneCommandBuffers.size(), std::memory_order_release);

        m_renderer->useCamera(command.camera);
        m_renderer->addLight(command.lights);
        m_renderer->loadSky(command.sky);

        for (const Renderer::DrawParticle& particle : command.particles)
        {
            m_renderer->drawParticle(particle);
        }

        for (const Renderer::DrawPoly3DCommandMesh& mesh : command.meshes)
        {
            Renderer::DrawPoly3DInstance instance = mesh.instance;
            if (!m_renderer->hasFeature(Renderer::RendererFeature::Light))
            {
                instance.flags &= ~Renderer::DrawPoly3DFlag::Lit;
            }
            for (std::uint8_t slot = 0; slot < TEXTURE_MATERIAL_COUNT; slot++)
            {
                instance.textures[slot] = resolveTextureId(
                    m_renderer.get(),
                    mesh.textures[slot],
                    slot == static_cast<std::uint8_t>(TextureMaterial::Albedo)
                );
            }

            m_renderer->drawPoly(resolvePolyId(m_renderer.get(), mesh.model), instance);
        }

        for (const Renderer::DrawPoly3DCommandPoly& poly : command.polys)
        {
            m_renderer->drawPoly(m_renderer->loadPoly(Renderer::DrawPolyType::e3D, poly.data), poly.instance);
        }

        if (!m_renderer->hasFeature(Renderer::RendererFeature::Traces))
        {
            return;
        }

        Vertex::List traces = Renderer::Debug::getVertices();
        if (traces.empty())
        {
            return;
        }

        const Renderer::DrawPoly3DCommandPoly poly = makeLinePoly(std::move(traces));
        m_renderer->drawPoly(m_renderer->loadPoly(Renderer::DrawPolyType::e3D, poly.data), poly.instance);
    }

    void Instance::pushTrace(const SceneTraceRequest& inRequest)
    {
        pushTrace(inRequest, Renderer::Debug::TRACE_COLOR);
    }

    void Instance::pushTrace(const SceneTraceRequest& inRequest, const Vec4& inColor)
    {
        if (!inRequest.isValid() || !hasRenderer())
        {
            return;
        }

        Vertex::List vertices;
        appendTrace(vertices, inRequest, inColor);

        Renderer::Debug::push(vertices, inRequest.duration);
    }

    void Instance::initUI()
    {
        watchView(
            [&](std::shared_ptr<Grid::View> inView)
            {
                if (!inView)
                {
                    return;
                }

                for (const Grid::StyleImport& import : inView->getStyleFile().getImports())
                {
                    if (import.location.type == Grid::StyleLocationType::URL)
                    {
                        continue;
                    }

                    switch (import.type)
                    {
                    case Grid::StyleImportType::Style:
                        inView->importStyleFile(FileSystem::Path(import.location.value));

                        break;

                    case Grid::StyleImportType::Font:
                    case Grid::StyleImportType::Texture:
                        Box::load(FileSystem::Path(import.location.value));

                        break;

                    default:
                        break;
                    };
                }
            }
        );

        m_viewThread = std::thread(&Instance::tickUI, this);
    }

    void Instance::shutdownUI()
    {
        if (m_viewThread.joinable())
        {
            m_viewThread.join();
        }
    }

    void Instance::tickUI()
    {
        Time::Point lastTick = Time::Clock::now();

        while (m_bIsRunning)
        {
            Box::pumpPreview();

            std::shared_ptr<Grid::View> view = getView();

            if (!view)
            {
                lastTick = Time::Clock::now();
                std::this_thread::yield();

                continue;
            }

            const Time::Point now   = Time::Clock::now();
            const float       delta = std::min(Time::sMiliseconds(now - lastTick), 100.0f);
            lastTick                = now;

            {
                m_telemetry.ui.start();

                view->pumpEvents();

                view->setSize(getRendererResolution());

                view->tick(delta);

                snapshotScreenViewport(view);

                buildUICommands(view);

                m_telemetry.ui.end();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Instance::snapshotScreenViewport(const std::shared_ptr<Grid::View>& inView)
    {
        Vec<2, std::uint32_t> size     = {0, 0};
        Vec<2, std::uint32_t> position = {0, 0};

        Grid::Component* viewport = nullptr;
        if (inView)
        {
            if (auto* viewViewport = dynamic_cast<Grid::Viewport*>(inView.get()))
            {
                if (viewViewport->isDisplayable())
                {
                    viewport = viewViewport;
                }
            }

            if (!viewport)
            {
                for (Grid::Component* child : inView->getChildrenFlat())
                {
                    auto* candidate = dynamic_cast<Grid::Viewport*>(child);
                    if (!candidate || !candidate->isDisplayable())
                    {
                        continue;
                    }

                    viewport = candidate;

                    break;
                }
            }
        }

        if (viewport)
        {
            const Bounds2D bounds = viewport->getDrawBounds();

            size = {
                static_cast<std::uint32_t>(std::max(0.0f, std::round(bounds.right - bounds.left))),
                static_cast<std::uint32_t>(std::max(0.0f, std::round(bounds.bottom - bounds.top)))
            };
            position = {
                static_cast<std::uint32_t>(std::max(0.0f, std::round(bounds.left))),
                static_cast<std::uint32_t>(std::max(0.0f, std::round(bounds.top)))
            };
        }

        m_screenViewportX.store(position.x, std::memory_order_relaxed);
        m_screenViewportY.store(position.y, std::memory_order_relaxed);
        m_screenViewportWidth.store(size.x, std::memory_order_relaxed);
        m_screenViewportHeight.store(size.y, std::memory_order_relaxed);
    }

    void Instance::buildUICommands(std::shared_ptr<Grid::View> inView)
    {
        if (!inView)
        {
            return;
        }

        std::size_t index = m_viewWriteIndex.load(std::memory_order_relaxed);

        Renderer::DrawPoly2DCommand& command = m_viewCommandBuffers.at(index);
        command.clear();

        const Vec2& viewSize = inView->getSize();

        inView->collectDrawables(m_viewDrawables);

        for (Grid::Component* component : m_viewDrawables)
        {
            const Bounds2D& clip = component->getOverflowClip();
            Bounds2D        draw = component->getDrawBounds();
            const float     blur = component->getFilterBlur();

            if (blur > 0.0f)
            {
                const float halo = blur * 3.0f;

                draw.left -= halo;
                draw.top -= halo;
                draw.right += halo;
                draw.bottom += halo;
            }

            if (clip.isEmpty() || !draw.overlaps(clip))
            {
                continue;
            }

            const Grid::Primitive& primitive    = component->getPrimitive();
            const Grid::Style&     style        = component->getStyle();
            const Vec2             size         = component->getSize();
            const Mat3             paint        = component->getPaintMatrix();
            const Vec2             visualCenter = component->getVisualCenter();
            const Vec3             mapped       = paint * Vec3(visualCenter.x, visualCenter.y, 1.0f);

            Renderer::DrawPoly2DCommandFill subcommand;
            subcommand.polygon         = primitive.reference;
            subcommand.vertices        = primitive.getSharedVertices();
            subcommand.indices         = primitive.getSharedIndices();
            subcommand.instance.view   = viewSize;
            subcommand.instance.scale  = component->getScale();
            subcommand.instance.size   = size;
            subcommand.instance.offset = Vec2::sZero();
            subcommand.instance
                .position = {mapped.x - (size.x * 0.5f), mapped.y - (size.y * 0.5f), component->getDepth()};
            subcommand.instance.transformX = {paint[0][0], paint[0][1]};
            subcommand.instance.transformY = {paint[1][0], paint[1][1]};
            subcommand.instance.clip       = {clip.left, clip.top, clip.right, clip.bottom};
            component->getPaintRadius(subcommand.instance.radiusX, subcommand.instance.radiusY);
            component->getOverflowRoundClips(
                subcommand.instance.innerClip,
                subcommand.instance.innerClipRadiusX,
                subcommand.instance.innerClipRadiusY,
                subcommand.instance.outerClip,
                subcommand.instance.outerClipRadiusX,
                subcommand.instance.outerClipRadiusY
            );
            const String backgroundImage = style.background.image.getRaw();
            const bool   bImageGradient  = Grid::StyleGradient::sIsDeclaration(backgroundImage);
            const bool   bColorGradient  = Grid::StyleGradient::sIsDeclaration(style.background.color.getRaw());

            subcommand.bHasTexture = !backgroundImage.isEmpty() && !bImageGradient;
            subcommand.texture     = subcommand.bHasTexture ? backgroundImage : Renderer::Draw::InvalidReference;

            subcommand.glyph        = primitive.glyph;
            subcommand.glyphOutline = primitive.getSharedOutline();
            subcommand.glyphMin     = primitive.outlineMin;
            subcommand.glyphMax     = primitive.outlineMax;

            subcommand.instance.dilation     = primitive.dilation;
            subcommand.instance.filterBlur   = blur;
            subcommand.instance.backdropBlur = style.backdrop.blur.get();
            subcommand.instance.color        = style.background.color.get();

            const float opacity = component->getOpacity();

            auto applyGradient = [](Renderer::DrawPoly2DCommandFill& outFill, const Grid::StyleGradient& inGradient)
            {
                if (!inGradient.isActive())
                {
                    return;
                }

                const std::uint32_t count = static_cast<std::uint32_t>(
                    std::min(inGradient.stops.size(), static_cast<std::size_t>(Grid::StyleGradient::MAX_STOPS))
                );

                outFill.instance.gradientType      = static_cast<std::int32_t>(inGradient.type);
                outFill.instance.gradientStopCount = static_cast<std::int32_t>(count);
                outFill.instance.gradientAxis      = inGradient.axis;

                float offsets[Grid::StyleGradient::MAX_STOPS] = {};
                for (std::uint32_t i = 0; i < count; i++)
                {
                    outFill.instance.gradientStops[i] = inGradient.stops.at(i).color;
                    offsets[i]                        = inGradient.stops.at(i).offset;
                }

                outFill.instance.gradientOffsets0 = Vec4(offsets[0], offsets[1], offsets[2], offsets[3]);
                outFill.instance.gradientOffsets1 = Vec4(offsets[4], offsets[5], offsets[6], offsets[7]);

                if (count > 0)
                {
                    outFill.instance.color = outFill.instance.gradientStops[0];
                }
            };

            auto applyOpacity = [opacity](Renderer::DrawPoly2DCommandFill& outFill)
            {
                outFill.instance.color.a = (outFill.bHasTexture ? 255.0f : outFill.instance.color.a) * opacity;

                if (outFill.instance.gradientStopCount > 0)
                {
                    for (std::int32_t i = 0; i < outFill.instance.gradientStopCount; i++)
                    {
                        outFill.instance.gradientStops[i].a *= opacity;
                    }
                }
            };

            auto stripOverlay = [](Renderer::DrawPoly2DCommandFill& outFill)
            {
                outFill.bHasTexture                = false;
                outFill.texture                    = Renderer::Draw::InvalidReference;
                outFill.instance.backdropBlur      = 0.0f;
                outFill.instance.borderWidth       = Vec4::sZero();
                outFill.instance.borderColorTop    = Vec4::sZero();
                outFill.instance.borderColorRight  = Vec4::sZero();
                outFill.instance.borderColorBottom = Vec4::sZero();
                outFill.instance.borderColorLeft   = Vec4::sZero();
            };

            subcommand.instance.borderWidth = style.border.paintedWidths();

            auto borderColor = [&](const auto& inProperty) -> Vec4
            {
                Vec4 result = inProperty.getRaw().isEmpty() ? style.foregroundColor.get() : inProperty.get();
                result.a *= component->getOpacity();

                return result;
            };

            subcommand.instance.borderColorTop    = borderColor(style.border.colorTop);
            subcommand.instance.borderColorRight  = borderColor(style.border.colorRight);
            subcommand.instance.borderColorBottom = borderColor(style.border.colorBottom);
            subcommand.instance.borderColorLeft   = borderColor(style.border.colorLeft);

            const Grid::StyleGradient::List&        gradients = style.background.gradients;
            std::vector<const Grid::StyleGradient*> layers;
            layers.reserve(gradients.size());
            for (const Grid::StyleGradient& layer : gradients)
            {
                if (layer.isActive())
                {
                    layers.push_back(&layer);
                }
            }

            const bool bPaintSolid = !bColorGradient && style.background.color.get().a > 0.0f;

            if (layers.empty())
            {
                applyOpacity(subcommand);
                command.fills.emplace_back(std::move(subcommand));
            }
            else
            {
                if (bPaintSolid)
                {
                    Renderer::DrawPoly2DCommandFill fill = subcommand;
                    fill.bHasTexture                     = false;
                    fill.texture                         = Renderer::Draw::InvalidReference;
                    fill.instance.gradientType           = 0;
                    fill.instance.gradientStopCount      = 0;
                    stripOverlay(fill);
                    applyOpacity(fill);
                    command.fills.emplace_back(std::move(fill));
                }

                for (int i = static_cast<int>(layers.size()) - 1; i >= 0; i--)
                {
                    Renderer::DrawPoly2DCommandFill fill = subcommand;
                    fill.bHasTexture                     = false;
                    fill.texture                         = Renderer::Draw::InvalidReference;
                    applyGradient(fill, *layers.at(static_cast<std::size_t>(i)));

                    if (i > 0)
                    {
                        stripOverlay(fill);
                    }

                    applyOpacity(fill);
                    command.fills.emplace_back(std::move(fill));
                }
            }
        }

        // Sort draw order by z-index
        std::stable_sort(
            command.fills.begin(),
            command.fills.end(),
            [](const Renderer::DrawPoly2DCommandFill& inLeft, const Renderer::DrawPoly2DCommandFill& inRight)
            { return inLeft.instance.position.z < inRight.instance.position.z; }
        );

        m_viewReadIndex.store(index, std::memory_order_release);
        m_viewWriteIndex.store(1 - index, std::memory_order_relaxed);
    }

    void Instance::renderUI()
    {
        if (!hasRenderer())
        {
            return;
        }

        const std::size_t index = m_viewReadIndex.load(std::memory_order_acquire);

        const Renderer::DrawPoly2DCommand& command = m_viewCommandBuffers.at(index);

        for (const Renderer::DrawPoly2DCommandFill& fill : command.fills)
        {
            Renderer::DrawPoly2DInstance instance = fill.instance;

            if (fill.bHasTexture)
            {
                instance.texture = m_renderer->findTexture(fill.texture);

                if (instance.texture <= Renderer::Draw::InvalidId)
                {
                    instance.texture = m_renderer->findTexture(Box::Texture::DEFAULT_REFERENCE);
                }
            }

            instance.glyph = m_renderer->findGlyph(fill.glyph);

            if (instance.glyph <= Renderer::Draw::InvalidId && fill.glyphOutline && !fill.glyphOutline->empty())
            {
                Renderer::DrawGlyphData data;
                data.reference = fill.glyph;
                data.boundsMin = fill.glyphMin;
                data.boundsMax = fill.glyphMax;
                data.points    = *fill.glyphOutline;

                instance.glyph = m_renderer->loadGlyph(data);
            }

            Renderer::Draw::Id polygon = m_renderer->findPoly(Renderer::DrawPolyType::e2D, fill.polygon);

            if (polygon <= Renderer::Draw::InvalidId)
            {
                Renderer::DrawPolyData data;
                data.reference = fill.polygon;

                if (fill.vertices)
                {
                    data.vertices = *fill.vertices;
                }

                if (fill.indices)
                {
                    data.indices = *fill.indices;
                }

                polygon = m_renderer->loadPoly(Renderer::DrawPolyType::e2D, data);
            }

            m_renderer->drawPoly(polygon, instance);
        }
    }
}
