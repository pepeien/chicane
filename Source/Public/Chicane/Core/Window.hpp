#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Layer.hpp"

namespace Chicane
{
    class Controller;
    class Renderer;
    class Level;
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
        std::string title        = "";
        std::string icon         = ""; // Optional [file path]
        Vec<int>::Two resolution = Vec<int>::Two(0);
        int displayIndex         = 0;
        WindowType type          = WindowType::Windowed;
        bool isFocused           = false;
        bool isResizable         = true; // Only takes effect when the type is `WindowType::Windowed`
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
            Layer::Push inPushTechnique = Layer::Push::Back,
            const std::string& inId = ""
        );

        void run();

        void onEvent(const SDL_Event& inEvent);

        bool isFocused();
        void focus();
        void blur();

        void setTitle(const std::string& inTitle);
        void setIcon(const std::string& inIconPath);

        Vec<int>::Two getResolution();
        void setResolution(const Vec<int>::Two& inResolution);

        Vec<int>::Two getPosition();

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
        // Telemetry
        std::clock_t m_beginFrame;
        std::clock_t m_endFrame;

        Telemetry m_telemetry;

        // Settings
        WindowType m_type;

        bool m_isFocused;
        bool m_isResizable;
        bool m_isMinimized; // Only takes effect when the type is `WindowType::Windowed`

        // Render
        std::unique_ptr<Renderer> m_renderer;
    };
}