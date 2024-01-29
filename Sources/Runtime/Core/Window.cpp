#include "Window.hpp"

#include "Layer.hpp"
#include "Layers/Level.hpp"
#include "Layers/Skybox.hpp"

#include "Game.hpp"
#include "Renderer.hpp"

namespace Chicane
{
    Window::Window(
        const WindowCreateInfo& inCreateInfo,
        Controller* inController,
        Level* inLevel
    )
        : instance(nullptr),
        m_isFocused(inCreateInfo.isFocused),
        m_isResizable(inCreateInfo.isResizable),
        m_isMinimized(false),
        m_renderer(nullptr),
        m_level(inLevel),
        m_controller(inController)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        if (inCreateInfo.resolution.width < 0 || inCreateInfo.resolution.height < 0)
        {
            throw std::runtime_error("Invalid resolution");
        }

        instance = SDL_CreateWindow(
            inCreateInfo.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            inCreateInfo.resolution.width,
            inCreateInfo.resolution.height,
            SDL_WINDOW_VULKAN
        );

        if (instance == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }

        setDisplay(inCreateInfo.displayIndex);
        setType(inCreateInfo.type);

        m_renderer = std::make_unique<Renderer>(this);

        initCoreLayers();
    }

    Window::~Window()
    {
        SDL_DestroyWindow(instance);
        SDL_Quit();
    }

    Renderer* Window::getRenderer()
    {
        return m_renderer.get();
    }

    Level* Window::getLevel()
    {
        return m_level;
    }

    void Window::addLayer(Layer* inLayer)
    {
        m_renderer->pushLayer(inLayer);
    }

    void Window::run()
    {
        SDL_Event event;

        bool shouldClose = false;
        while (shouldClose == false)
        {
            while(SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    shouldClose = true;

                    break;
                }

                onEvent(event);

                m_renderer->onEvent(event);
            }

            m_renderer->render();
        }
    }

    void Window::onEvent(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        case SDL_WINDOWEVENT:
            onWindowEvent(inEvent.window);

            break;

        case SDL_KEYDOWN:
            onKeyDown(inEvent.key);

            break;
        }
    }

    bool Window::isFocused()
    {
        return m_isFocused;
    }

    void Window::focus()
    {
        m_isFocused = true;

        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    void Window::blur()
    {
        m_isFocused = false;

        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    Resolution Window::getResolution()
    {
        int currentWidth  = 0;
        int currentHeight = 0;

        SDL_Vulkan_GetDrawableSize(
            instance,
            &currentWidth,
            &currentHeight
        );

        return { currentWidth, currentHeight };
    }

    void Window::setTitle(const std::string& inTitle)
    {
        SDL_SetWindowTitle(
            instance,
            inTitle.c_str()
        );
    }

    void Window::setResolution(const Resolution& inResolution)
    {
        if (m_isResizable)
        {
            return;
        }

        SDL_SetWindowSize(
            instance,
            inResolution.width,
            inResolution.height
        );
    }

    void Window::setDisplay(int inMonitorIndex)
    {
        SDL_SetWindowPosition(
            instance,
            SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(inMonitorIndex)
        );
    }

    void Window::setType(WindowType inType)
    {
        m_type = inType;

        switch (inType)
        {
        case WindowType::Windowed:
            SDL_SetWindowBordered(instance, SDL_TRUE);

            if (m_isResizable)
            {
                enableResizing();

                break;
            }
            
            disableResizing();

            break;
        
        case WindowType::WindowedBorderless:
            disableResizing();

            SDL_SetWindowBordered(instance, SDL_FALSE);

            break;
        
        case WindowType::Fullscreen:
            disableResizing();

            SDL_SetWindowFullscreen(instance, SDL_WINDOW_FULLSCREEN);

            break;
        }
    }

    WindowType Window::getType()
    {
        return m_type;
    }

    bool Window::isResizable()
    {
        return m_isResizable;
    }

    void Window::enableResizing()
    {
        if (m_type != WindowType::Windowed)
        {
            return;
        }

        SDL_SetWindowResizable(
            instance,
            SDL_TRUE
        );

        m_isResizable = true;
    }

    void Window::disableResizing()
    {
        SDL_SetWindowResizable(
            instance,
            SDL_FALSE
        );

        m_isResizable = false;
    }

    bool Window::isMinimized()
    {
        return m_isMinimized;
    }

    void Window::initCoreLayers()
    {
        m_skyboxLayer = std::make_unique<SkyboxLayer>(this);
        addLayer(m_skyboxLayer.get());

        m_levelLayer = std::make_unique<LevelLayer>(this);
        addLayer(m_levelLayer.get());
    }

    void Window::onWindowEvent(const SDL_WindowEvent& inEvent)
    {
        switch (inEvent.event)
        {
        case SDL_WINDOWEVENT_MINIMIZED:
            m_isMinimized = true;

            break;

        case SDL_WINDOWEVENT_EXPOSED:
            m_isMinimized = false;

            break;
        }
    }

    void Window::onMouseClick()
    {
        if (!isFocused())
        {
            focus();

            return;
        }

        blur();
    }

    void Window::onKeyDown(const SDL_KeyboardEvent& inEvent)
    {
        if (m_controller == nullptr)
        {
            return;
        }

        m_controller->onEvent(inEvent.keysym.scancode);
    }
}