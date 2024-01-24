#pragma once

#include "Base.hpp"

namespace Chicane
{
    class Controller;
    class Renderer;
    class Level;

    enum class WindowType : uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };

    struct WindowCreateInfo
    {
        std::string title     = "";
        Resolution resolution = {};
        WindowType type       = WindowType::Windowed;
        bool isFocused        = false;
        bool isResizable      = true; // Only takes effect when the type is `WindowType::Windowed`
    };

    class Window
    {
    public:
        Window(
            const WindowCreateInfo& inCreateInfo,
            Level* inLevel,
            Controller* inController
        );
        ~Window();

    public:
        void run();

        void onEvent(const SDL_Event& inEvent);

        bool isFocused();
        void focus();
        void blur();

        void setTitle(const std::string& inTitle);

        Resolution getResolution();
        void setResolution(const Resolution& inResolution);

        void setType(WindowType inType);
        WindowType getType();

        bool isResizable();
        void enableResizing();  // Only takes effect when the type is `WindowType::Windowed`
        void disableResizing(); // Only takes effect when the type is `WindowType::Windowed`

        bool isMinimized();

    public:
        SDL_Window* instance;

    private:
        void onWindowEvent(const SDL_WindowEvent& inEvent);
        void onMouseClick();
        void onKeyDown(const SDL_KeyboardEvent& inEvent);

    private:
        WindowType m_type;

        bool m_isFocused;
        bool m_isResizable;
        bool m_isMinimized; // Only takes effect when the type is `WindowType::Windowed`

        std::unique_ptr<Renderer> m_renderer;

        Controller* m_controller;
    };
}