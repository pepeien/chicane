#pragma once

#include "Runtime/Runtime.hpp"

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
        Vec2 resolution = {};
        int displayIndex      = 0;
        WindowType type       = WindowType::Windowed;
        bool isFocused        = false;
        bool isResizable      = true; // Only takes effect when the type is `WindowType::Windowed`
    };

    class Window
    {
    public:
        Window(
            const WindowCreateInfo& inCreateInfo,
            Controller* inController = nullptr,
            Level* inLevel = nullptr
        );
        ~Window();

    public:
        Renderer* getRenderer();

        Level* getLevel();
        void setLevel(Level* inLevel);

        Controller* getController();
        void setController(Controller* inController);

        Telemetry getTelemetry();

        void addLayer(Layer* inLayer);

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
        void nextEvent(SDL_Event inEvent);
        void errorEvent(std::string inMessage);
        void completeEvent();

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
        Level* m_level;
        Controller* m_controller;
    };
}