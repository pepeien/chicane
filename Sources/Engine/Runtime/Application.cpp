#include "Chicane/Runtime/Application.hpp"

#include <iostream>
#include <thread>

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Model/Manager.hpp"
#include "Chicane/Box/Texture/Manager.hpp"

#include "Chicane/Core/Log.hpp"

#include "Chicane/Kerb.hpp"

#include "Chicane/Runtime/Scene/Actor/Sky.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    Application::Application()
        : m_telemetry({}),
          m_bIsRunning(false),
          m_controller(nullptr),
          m_controllerObservable({}),
          m_scene(nullptr),
          m_sceneMutex({}),
          m_sceneThread({}),
          m_sceneCommandBuffers({}),
          m_sceneWriteIndex(0),
          m_sceneReadIndex(1),
          m_sceneObservable({}),
          m_view(nullptr),
          m_viewMutex({}),
          m_viewThread({}),
          m_viewCommandBuffers({}),
          m_viewWriteIndex(0),
          m_viewReadIndex(1),
          m_viewObservable({}),
          m_window(nullptr),
          m_renderer(nullptr)
    {
        m_sceneCommandBuffers.resize(2);
        m_viewCommandBuffers.resize(2);
    }

    Application::~Application()
    {}

    void Application::run(const ApplicationCreateInfo& inCreateInfo)
    {
        initRenderer(inCreateInfo.renderer);
        initWindow(inCreateInfo.window);
        initBox();
        initKerb();

        if (inCreateInfo.onSetup)
        {
            inCreateInfo.onSetup();
        }

        m_bIsRunning = true;

        initSceneThread();
        initViewThread();

        while (m_window->run())
        {
            m_telemetry.start();

            render();

            m_telemetry.end();
        }

        m_bIsRunning = false;

        shutdownSceneThread();
        shutdownViewThread();
    }

    void Application::render()
    {
        renderScene();
        renderView();

        m_renderer->render();
    }

    const Telemetry& Application::getTelemetry() const
    {
        return m_telemetry;
    }

    bool Application::hasController()
    {
        return m_controller != nullptr;
    }

    Controller* Application::getController()
    {
        return m_controller;
    }

    void Application::setController(Controller* inController)
    {
        if (inController == m_controller)
        {
            return;
        }

        m_controller = inController;

        m_controllerObservable.next(m_controller);
    }

    Application::ControllerSubscription Application::watchController(
        ControllerSubscription::NextCallback     inNext,
        ControllerSubscription::ErrorCallback    inError,
        ControllerSubscription::CompleteCallback inComplete
    )
    {
        return m_controllerObservable.subscribe(inNext, inError, inComplete).next(m_controller);
    }

    bool Application::hasScene()
    {
        return m_scene && m_scene.get() != nullptr;
    }

    Application::SceneSubscription Application::watchScene(
        SceneSubscription::NextCallback     inNext,
        SceneSubscription::ErrorCallback    inError,
        SceneSubscription::CompleteCallback inComplete
    )
    {
        return m_sceneObservable.subscribe(inNext, inError, inComplete).next(m_scene.get());
    }

    bool Application::hasView()
    {
        return m_view != nullptr;
    }

    Application::ViewSubscription Application::watchView(
        ViewSubscription::NextCallback     inNext,
        ViewSubscription::ErrorCallback    inError,
        ViewSubscription::CompleteCallback inComplete
    )
    {
        return m_viewObservable.subscribe(inNext, inError, inComplete).next(m_view.get());
    }

    bool Application::hasWindow()
    {
        return m_window && m_window.get() != nullptr;
    }

    Window* Application::getWindow()
    {
        if (!hasWindow())
        {
            return nullptr;
        }

        return m_window.get();
    }

    bool Application::hasRenderer()
    {
        return m_renderer && m_renderer.get() != nullptr;
    }

    Renderer::Instance* Application::getRenderer()
    {
        if (!hasRenderer())
        {
            return nullptr;
        }

        return m_renderer.get();
    }

    void Application::setRenderer(WindowBackend inBackend)
    {
        if (!hasWindow())
        {
            return;
        }

        if (hasRenderer())
        {
            m_renderer->shutdownBackend();
        }

        m_window->setBackend(inBackend);
    }

    void Application::initWindow(const WindowSettings& inSettings)
    {
        if (hasWindow())
        {
            return;
        }

        m_window = std::make_unique<Window>();
        m_window->init(inSettings);
        m_window->watchSize(
            [this](Vec<2, std::uint32_t> inSize)
            {
                if (hasRenderer())
                {
                    m_renderer->setResolution(inSize);
                }
            }
        );
        m_window->watchBackend(
            [this](WindowBackend inValue)
            {
                if (hasRenderer())
                {
                    m_renderer->reloadBackend(m_window.get());
                }
            }
        );
    }

    void Application::initRenderer(const Renderer::Settings& inSettings)
    {
        if (hasRenderer())
        {
            return;
        }

        m_renderer = std::make_unique<Renderer::Instance>();
        m_renderer->init(inSettings);
        m_renderer->watchResolution(
            [&](const Vec<2, std::uint32_t>& inResolution)
            {
                if (hasView())
                {
                    m_view->setSize(inResolution);
                }
            }
        );
    }

    void Application::initBox()
    {
        Box::getModelManager()->watch(
            [&](const Box::ModelManager::Instances& inInstances)
            {
                for (const auto& [id, model] : inInstances)
                {
                    Renderer::DrawPolyData data;
                    data.reference = id;
                    data.vertices  = model.vertices;
                    data.indices   = model.indices;

                    m_renderer->loadPoly(Renderer::DrawPolyType::e3D, data);
                }
            }
        );
        Box::getTextureManager()->watch(
            [&](const Box::TextureManager::Instances& inInstances)
            {
                for (const auto& [id, texture] : inInstances)
                {
                    Renderer::DrawTextureData data;
                    data.reference = id;
                    data.image     = texture;

                    m_renderer->loadTexture(data);
                }
            }
        );

        Box::init();
    }

    void Application::initKerb()
    {
        Kerb::init();
    }

    void Application::initSceneThread()
    {
        m_sceneThread = std::thread(&Application::tickScene, this);
    }

    void Application::shutdownSceneThread()
    {
        if (m_sceneThread.joinable())
        {
            m_sceneThread.join();
        }
    }

    void Application::tickScene()
    {
        while (m_bIsRunning)
        {
            if (!hasScene())
            {
                std::this_thread::yield();

                continue;
            }

            {
                // Tick
                m_scene->tick(m_telemetry.delta);

                // Render
                std::uint32_t                      index    = m_sceneWriteIndex.load(std::memory_order_relaxed);
                Renderer::DrawPoly3DCommand::List& commands = m_sceneCommandBuffers[index];
                commands.clear();

                commands.reserve(m_scene->getComponents().size());

                for (CCamera* camera : m_scene->getActiveComponents<CCamera>())
                {
                    camera->onResize(m_renderer->getResolution());

                    Renderer::DrawPoly3DCommand command;
                    command.type   = Renderer::DrawPoly3DCommandType::Camera;
                    command.camera = camera->getData();

                    commands.emplace_back(std::move(command));
                }

                for (CLight* light : m_scene->getActiveComponents<CLight>())
                {
                    light->onResize(m_renderer->getResolution());

                    Renderer::DrawPoly3DCommand command;
                    command.type  = Renderer::DrawPoly3DCommandType::Light;
                    command.light = light->getData();

                    commands.emplace_back(std::move(command));
                }

                for (ASky* sky : m_scene->getActors<ASky>())
                {
                    const Box::Sky* asset = sky->getSky();

                    Renderer::DrawSkyData data;
                    data.reference = asset->getFilepath().string();
                    data.model     = asset->getModel();

                    for (Box::SkySide side : Box::Sky::ORDER)
                    {
                        data.textures.push_back(asset->getSide(side));
                    }

                    Renderer::DrawPoly3DCommand command;
                    command.type = Renderer::DrawPoly3DCommandType::Sky;
                    command.sky  = data;

                    commands.emplace_back(std::move(command));
                }

                for (CMesh* mesh : m_scene->getActiveComponents<CMesh>())
                {
                    if (!mesh->hasMesh())
                    {
                        continue;
                    }

                    Renderer::DrawPoly3DCommand command;
                    command.type           = Renderer::DrawPoly3DCommandType::Mesh;
                    command.instance.model = mesh->getTransform().getMatrix();

                    for (const Box::MeshGroup& group : mesh->getMesh()->getGroups())
                    {
                        command.meshes.push_back({.model = group.getModel(), .texture = group.getTexture()});
                    }

                    commands.emplace_back(std::move(command));
                }

                m_sceneReadIndex.store(index, std::memory_order_release);
                m_sceneWriteIndex.store(1 - index, std::memory_order_relaxed);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Application::renderScene()
    {
        if (!hasScene())
        {
            return;
        }

        std::uint32_t index = m_sceneReadIndex.load(std::memory_order_acquire);

        for (const Renderer::DrawPoly3DCommand& commandBuffer : m_sceneCommandBuffers[index])
        {
            Renderer::DrawPoly3DCommand command = commandBuffer;

            switch (command.type)
            {
            case Renderer::DrawPoly3DCommandType::Camera:
                m_renderer->useCamera(command.camera);

                break;

            case Renderer::DrawPoly3DCommandType::Light:
                m_renderer->addLight(command.light);

                break;

            case Renderer::DrawPoly3DCommandType::Sky:
                m_renderer->loadSky(command.sky);

                break;

            case Renderer::DrawPoly3DCommandType::Mesh:
                for (const Renderer::DrawPoly3DCommandMesh& mesh : command.meshes)
                {
                    command.instance.texture = m_renderer->findTexture(mesh.texture);

                    m_renderer->drawPoly(Renderer::DrawPolyType::e3D, mesh.model, command.instance);
                }

                break;

            default:
                break;
            }
        }
    }

    void Application::initViewThread()
    {
        m_viewThread = std::thread(&Application::tickView, this);
    }

    void Application::shutdownViewThread()
    {
        if (m_viewThread.joinable())
        {
            m_viewThread.join();
        }
    }

    void Application::tickView()
    {
        while (m_bIsRunning)
        {
            if (!hasView())
            {
                std::this_thread::yield();

                continue;
            }

            {
                // Tick
                m_view->tick(m_telemetry.delta);

                // Render
                std::uint32_t                      index    = m_viewWriteIndex.load(std::memory_order_relaxed);
                Renderer::DrawPoly2DCommand::List& commands = m_viewCommandBuffers[index];
                commands.clear();

                const Vec2& viewSize = m_view->getSize();

                for (Grid::Component* component : m_view->getChildrenFlat())
                {
                    if (!component->isDrawable())
                    {
                        continue;
                    }

                    const Grid::Primitive& primitive = component->getPrimitive();
                    const Grid::Style&     style     = component->getStyle();

                    Renderer::DrawPoly2DCommand command;
                    command.polygon.vertices = primitive.vertices;
                    command.polygon.indices  = primitive.indices;
                    command.instance.screen  = viewSize;
                    command.instance.size    = component->getSize();
                    command.instance
                        .position = {component->getPosition().x, component->getPosition().y, style.zIndex.get()};
                    command.textureReference = style.background.image.get();
                    command.opacity          = style.opacity.get();
                    command.instance.color   = style.background.color.get();

                    commands.emplace_back(std::move(command));
                }

                m_viewReadIndex.store(index, std::memory_order_release);
                m_viewWriteIndex.store(1 - index, std::memory_order_relaxed);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Application::renderView()
    {
        if (!hasRenderer())
        {
            return;
        }

        std::uint32_t index = m_viewReadIndex.load(std::memory_order_acquire);

        for (const Renderer::DrawPoly2DCommand& command : m_viewCommandBuffers[index])
        {
            Renderer::DrawPoly2DInstance draw = command.instance;
            draw.texture                      = m_renderer->findTexture(command.textureReference);
            draw.color.a = (draw.texture > Renderer::Draw::UnknownId ? 255.0f : draw.color.a) * command.opacity;

            m_renderer->drawPoly(m_renderer->loadPoly(Renderer::DrawPolyType::e2D, command.polygon), draw);
        }
    }
}