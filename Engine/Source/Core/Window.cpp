#include "Window.hpp"

namespace Engine
{
    Window::Window(const WindowCreateInfo& inCreateInfo)
        : m_isFocused(false),
        m_displayResolution({}),
        m_drawResolution({}),
        instance(nullptr)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        m_displayResolution = inCreateInfo.resolution;

        if (m_displayResolution.width < 0 || m_displayResolution.height < 0)
        {
            SDL_DisplayMode display;
            SDL_GetCurrentDisplayMode(0, &display);

            m_displayResolution.width  = display.w;
            m_displayResolution.height = display.h;
        }

        int renderCount = SDL_GetNumRenderDrivers();

        instance = SDL_CreateWindow(
            inCreateInfo.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_displayResolution.width,
            m_displayResolution.height,
            SDL_WINDOW_RESIZABLE |
            SDL_WINDOW_MAXIMIZED |
            SDL_WINDOW_VULKAN
        );

        if (instance == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }

        onResize();
    }

    Window::~Window()
    {
        SDL_DestroyWindow(instance);
        SDL_Quit();
    }

    bool Window::isOutdated()
    {
        int currentWidth  = 0;
        int currentHeight = 0;

        SDL_Vulkan_GetDrawableSize(
            instance,
            &currentWidth,
            &currentHeight
        );

        return currentWidth != m_drawResolution.width && currentHeight != m_drawResolution.height;
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

    Resolution Window::getDisplayResolution()
    {
        return m_displayResolution;
    }

    void Window::setDisplayResolution(const Resolution& inResolution)
    {
        m_displayResolution = inResolution;
    }

    void Window::onResize()
    {
        int currentWidth  = 0;
        int currentHeight = 0;

        SDL_Vulkan_GetDrawableSize(
            instance,
            &currentWidth,
            &currentHeight
        );

        m_drawResolution.width  = currentWidth;
        m_drawResolution.height = currentHeight;
    }

    void Window::setTitle(const std::string& inTitle)
    {
        SDL_SetWindowTitle(
            instance,
            inTitle.c_str()
        );
    }
}