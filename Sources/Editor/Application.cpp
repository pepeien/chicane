#include "Editor/Application.hpp"

#include <Chicane/Core/List/Push.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>

#include <Chicane/Renderer.hpp>

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Layer/Grid.hpp"
#include "Editor/UI/View/Home.hpp"

namespace Editor
{
    Application* Application::s_instance = nullptr;

    Application& Application::getInstance()
    {
        return *s_instance;
    }

    Application::Application(const std::vector<Chicane::FileSystem::Path>& inModules)
        : m_controller(nullptr),
          m_homeScene(nullptr),
          m_viewerScene(nullptr)
    {
        s_instance = this;

        Chicane::ApplicationCreateInfo createInfo;

        // Window
        createInfo.window.title   = "Chicane Editor";
        createInfo.window.icon    = "Assets/Editor/Icon.png";
        createInfo.window.display = 0;
        createInfo.window.type    = Chicane::WindowType::WindowedBorderless;
        createInfo.window.backend = Chicane::WindowBackend::Vulkan;

        createInfo.modules = inModules;

        // Setup
        createInfo.onSetup = [this]()
        {
            initController();
            initScene();
            initView();
            initLayers();
        };

        Chicane::Application::getInstance().run(createInfo);

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
        Chicane::Application::getInstance().setScene(m_homeScene);
        possess(m_homeScene);
    }

    void Application::activateViewerScene()
    {
        Chicane::Application::getInstance().setScene(m_viewerScene);
        possess(m_viewerScene);
    }

    void Application::initController()
    {
        m_controller = std::make_unique<Chicane::Controller>();

        Chicane::Application::getInstance().setController(m_controller.get());
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
        Chicane::Application::getInstance().setView<HomeView>();
    }

    void Application::initLayers()
    {
        Chicane::Application::getInstance().getWindow()->watchBackend(
            [](Chicane::WindowBackend inValue)
            {
                Chicane::ListPush<Chicane::Renderer::Layer*> settings;
                settings.strategy  = Chicane::ListPushStrategy::After;
                settings.predicate = [](Chicane::Renderer::Layer* inLayer)
                { return inLayer->getId().equals(Chicane::Renderer::SCENE_MESH_LAYER_ID); };

                Chicane::Application::getInstance().getRenderer()->addBackendLayer<LGrid>(settings);
            }
        );
    }

    void Application::possess(const std::shared_ptr<Chicane::Scene>& inScene)
    {
        Chicane::Controller* controller = Chicane::Application::getInstance().getController();
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
