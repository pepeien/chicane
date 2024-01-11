#pragma once

#include "Base.hpp"

namespace Engine
{
    struct Resolution
    {
        int width  = -1;
        int height = -1;
    };

    struct WindowCreateInfo
    {
        Resolution resolution = {};
        std::string title    = "";
    };

    class Window
    {
    public:
        Window(const WindowCreateInfo& inCreateInfo);
        ~Window();

    public:
        // Status
        bool isOutdated();
        bool isFocused();

        // Focus
        void focus();
        void blur();

        // Resolution
        void onResize();
        Resolution getDisplayResolution();
        void setDisplayResolution(const Resolution& inResolution);

        // Properties
        void setTitle(const std::string& inTitle);

    public:
        SDL_Window* instance;

    private:
        // Status
        bool m_isFocused;

        // Resolution
        Resolution m_displayResolution;
        Resolution m_drawResolution;
    };
}