#include "Application.hpp"

#include <Chicane/Core/List.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Renderer.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

#include "Scene.hpp"
#include "View.hpp"

#if CHICANE_OPENGL
    #include "Layer/OpenGL/Grid.hpp"
#endif

#if CHICANE_VULKAN
    #include "Layer/Vulkan/Grid.hpp"
#endif

namespace Editor
{
    Application::Application()
        : m_controller(nullptr)
    {
        Chicane::ApplicationCreateInfo createInfo = {};

        // Window
        createInfo.window.title   = "Chicane Editor";
        createInfo.window.icon    = "Contents/Editor/Icon.png";
        createInfo.window.size    = Chicane::Vec<2, std::uint32_t>(1600, 900);
        createInfo.window.display = 0;
        createInfo.window.type    = Chicane::WindowType::Windowed;
        createInfo.window.backend = Chicane::WindowBackend::OpenGL;

        // Setup
        createInfo.onSetup = [&]()
        {
            initController();
            initScene();
            initView();
            initLayers();
        };

        Chicane::Application::getInstance().run(createInfo);
    }

    void Application::initController()
    {
        m_controller = std::make_unique<Chicane::Controller>();

        Chicane::Application::getInstance().setController(m_controller.get());
    }

    void Application::initScene()
    {
        Chicane::Application::getInstance().setScene<Scene>();
    }

    void Application::initView()
    {
        Chicane::Application::getInstance().setView<View>();
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

                switch (inValue)
                {
#if CHICANE_OPENGL
                case Chicane::WindowBackend::OpenGL:

                    Chicane::Application::getInstance().getRenderer()->addBackendLayer<OpenGLLUI>(settings);

                    break;
#endif

#if CHICANE_VULKAN
                case Chicane::WindowBackend::Vulkan:
                    Chicane::Application::getInstance().getRenderer()->addBackendLayer<VulkanLUI>(settings);

                    break;
#endif

                default:
                    break;
                }
            }
        );
    }
}