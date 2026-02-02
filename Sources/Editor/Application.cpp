#include "Application.hpp"

#include <Chicane/Core/List.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

#include "Actor/Character.hpp"
#include "Scene.hpp"
#include "View.hpp"

#include "Layer/Vulkan/Grid.hpp"
#include "Layer/OpenGL/Grid.hpp"

namespace Editor
{
    Application::Application()
        : m_controller(nullptr)
    {
        Chicane::ApplicationCreateInfo createInfo = {};

        // Window
        createInfo.window.title   = "Chicane Editor";
        createInfo.window.icon    = "Contents/Editor/Icon.png";
        createInfo.window.size    = Chicane::Vec<2, int>(1600, 900);
        createInfo.window.display = 0;
        createInfo.window.type    = Chicane::WindowType::Windowed;
        createInfo.window.backend = Chicane::WindowBackend::OpenGL;

        // Setup
        createInfo.onSetup = [&]()
        {
            initScene();
            initCharacter();
            initView();

            Chicane::ListPush<Chicane::Renderer::Layer*> settings;
            settings.strategy  = Chicane::ListPushStrategy::After;
            settings.predicate = [](Chicane::Renderer::Layer* inLayer)
            { return inLayer->getId().equals("Engine_Scene_Mesh"); };

            switch (createInfo.window.backend)
            {
            case Chicane::WindowBackend::OpenGL:
                Chicane::Application::getInstance().getRenderer()->getBackend()->addLayer<OpenGLLGrid>(settings);

                break;

            case Chicane::WindowBackend::Vulkan:
                Chicane::Application::getInstance().getRenderer()->getBackend()->addLayer<VulkanLGrid>(settings);

                break;

            default:
                break;
            }
        };

        Chicane::Application::getInstance().run(createInfo);
    }

    void Application::initScene()
    {
        Chicane::Application::getInstance().setScene<Scene>();
    }

    void Application::initCharacter()
    {
        Chicane::Application& application = Chicane::Application::getInstance();

        m_controller = std::make_unique<Chicane::Controller>();
        application.setController(m_controller.get());

        Character* character = application.getScene<Scene>()->createActor<Character>();
        character->setAbsoluteTranslation(10.0f, -10.0f, 10.0f);

        m_controller->attachTo(character);
    }

    void Application::initView()
    {
        Chicane::Application::getInstance().setView<View>();
    }
}