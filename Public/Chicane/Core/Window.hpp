#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    class Controller;
    class Renderer;
    class Level;
    class Layer;
    class LevelLayer;
    class SkyboxLayer;

    enum class WindowType : uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };

    struct WindowCreateInfo
    {
        std::string title     = "";
        Vec2 resolution       = {};
        int displayIndex      = 0;
        WindowType type       = WindowType::Windowed;
        bool isFocused        = false;
        bool isResizable      = true; // Only takes effect when the type is `WindowType::Windowed`
    };

    class Window
    {
    public:
        Window(const WindowCreateInfo& inCreateInfo);
        ~Window();

    public:
        Renderer* getRenderer();

        Telemetry getTelemetry();

        void addLayer(
            Layer* inLayer,
            LayerPushTecnique inAdditionTecnique = LayerPushTecnique::Back,
            const std::string& inId = ""
        );

        void run();

        void onEvent(const SDL_Event& inEvent);

        bool isFocused();
        void focus();
        void blur();

        void setTitle(const std::string& inTitle);

        Vec2 getResolution();
        void setResolution(const Vec2& inResolution);

        Vec2 getPosition();

        void setDisplay(int inMonitorIndex);

        void setType(WindowType inType);
        WindowType getType();

        bool isResizable();
        void enableResizing();  // Only takes effect when the type is `WindowType::Windowed`
        void disableResizing(); // Only takes effect when the type is `WindowType::Windowed`

        bool isMinimized();

    public:
        SDL_Window* instance;

    private:
        // Events
        void initRenderer();
        void initCoreLayers();

        void updateTelemetry();

        void onWindowEvent(const SDL_WindowEvent& inEvent);
        void onMouseClick();
        void onKeyDown(const SDL_KeyboardEvent& inEvent);

    private:
        Telemetry m_telemetry;

        WindowType m_type;

        bool m_isFocused;
        bool m_isResizable;
        bool m_isMinimized; // Only takes effect when the type is `WindowType::Windowed`

        std::unique_ptr<Renderer> m_renderer;
    };
}