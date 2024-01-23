#include "Window.hpp"

namespace Chicane
{
    Window::Window(
        const WindowCreateInfo& inCreateInfo,
        Level* inLevel,
        Controller* inController
    )
        : instance(nullptr),
        m_isFocused(inCreateInfo.isFocused),
        m_isResizable(inCreateInfo.isResizable),
        m_isMinimized(false),
        m_renderer(nullptr),
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

        setType(inCreateInfo.type);

        m_renderer = std::make_unique<Renderer>(
            this,
            inLevel
        );
    }

    Window::~Window()
    {
        SDL_DestroyWindow(instance);
        SDL_Quit();
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

        case SDL_MOUSEBUTTONDOWN:
            onMouseClick();

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