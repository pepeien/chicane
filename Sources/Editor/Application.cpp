#include "Editor/Application.hpp"

#include <Chicane/Core/List/Push.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>

#include <Chicane/Renderer.hpp>

#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Instance/CreateInfo.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Layer/Grid.hpp"
#include "Editor/Layer/Icon.hpp"
#include "Editor/Layer/Line.hpp"
#include "Editor/UI/View/Home.hpp"

namespace Editor
{
    Application* Application::s_instance = nullptr;

    Application& Application::sInstance()
    {
        return *s_instance;
    }

    Application::Application(int inArgCount, char* inArgValues[])
        : Chicane::Application(inArgCount, inArgValues),
          m_controller(nullptr),
          m_homeScene(nullptr),
          m_viewerScene(nullptr)
    {
        s_instance = this;

        std::vector<Chicane::FileSystem::Path> modules;
        for (const Chicane::String& path : getArgValues("module"))
        {
            if (path.isEmpty())
            {
                continue;
            }

            modules.emplace_back(path);
        }

        Chicane::InstanceCreateInfo createInfo;

        // Window
        createInfo.window.title   = "Chicane Editor";
        createInfo.window.icon    = "Assets/Editor/Icon.png";
        createInfo.window.display = 0;
        createInfo.window.type    = Chicane::WindowType::WindowedBorderless;
        createInfo.window.backend = Chicane::WindowBackend::Vulkan;

        createInfo.modules = modules;

        // Setup
        createInfo.onSetup = [this]()
        {
            initController();
            initScene();
            initView();
            initLayers();
        };

        Chicane::Instance::sInstance().run(createInfo);

        s_instance = nullptr;
    }

    std::shared_ptr<Scene> Application::getHomeScene() const
    {
        return m_homeScene;
    }

    std::shared_ptr<ViewerScene> Application::getViewerScene() const
    {
        return m_viewerScene;
    }

    void Application::activateHomeScene()
    {
        Chicane::Instance::sInstance().setScene(m_homeScene);
        possess(m_homeScene);
    }

    void Application::activateViewerScene()
    {
        Chicane::Instance::sInstance().setScene(m_viewerScene);
        possess(m_viewerScene);
    }

    void Application::initController()
    {
        m_controller = std::make_unique<Chicane::Controller>();

        Chicane::Instance::sInstance().setController(m_controller.get());
    }

    void Application::initScene()
    {
        m_homeScene = std::make_shared<Scene>();
        m_homeScene->load();

        m_viewerScene = std::make_shared<ViewerScene>();
        m_viewerScene->load();

        activateHomeScene();
    }

    void Application::initView()
    {
        Chicane::Instance::sInstance().setView<HomeView>();
    }

    void Application::initLayers()
    {
        Chicane::Instance::sInstance().getWindow()->watchBackend(
            [](Chicane::WindowBackend inValue)
            {
                Chicane::Renderer::Instance* renderer = Chicane::Instance::sInstance().getRenderer();

                Chicane::ListPush<Chicane::Renderer::Layer*> grid;
                grid.strategy  = Chicane::ListPushStrategy::After;
                grid.predicate = [](Chicane::Renderer::Layer* inLayer)
                { return inLayer->getId().equals(Chicane::Renderer::SCENE_MESH_LAYER_ID); };
                renderer->addBackendLayer<LGrid>(grid);

                Chicane::ListPush<Chicane::Renderer::Layer*> line;
                line.strategy  = Chicane::ListPushStrategy::Replace;
                line.predicate = [](Chicane::Renderer::Layer* inLayer)
                { return inLayer->getId().equals(Chicane::Renderer::SCENE_LINE_LAYER_ID); };
                renderer->addBackendLayer<LLine>(line);

                Chicane::ListPush<Chicane::Renderer::Layer*> icon;
                icon.strategy  = Chicane::ListPushStrategy::After;
                icon.predicate = [](Chicane::Renderer::Layer* inLayer)
                { return inLayer->getId().equals(Chicane::Renderer::SCENE_LINE_LAYER_ID); };
                renderer->addBackendLayer<LIcon>(icon);
            }
        );
    }

    void Application::possess(const std::shared_ptr<Chicane::Scene>& inScene)
    {
        Chicane::Controller* controller = Chicane::Instance::sInstance().getController();
        if (!controller)
        {
            return;
        }

        if (controller->isAttached())
        {
            controller->deattach();
        }

        if (!inScene)
        {
            return;
        }

        for (Character* character : inScene->getActors<Character>())
        {
            if (!character)
            {
                continue;
            }

            controller->attachTo(character);

            for (Chicane::CCamera* camera : inScene->getComponents<Chicane::CCamera>())
            {
                if (!camera)
                {
                    continue;
                }

                camera->deactivate();
            }

            for (Chicane::Component* attachment : character->getAttachments())
            {
                Chicane::CCamera* camera = dynamic_cast<Chicane::CCamera*>(attachment);
                if (!camera)
                {
                    continue;
                }

                camera->activate();
            }

            return;
        }
    }
}
