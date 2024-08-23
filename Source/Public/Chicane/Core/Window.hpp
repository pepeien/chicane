#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Controller;
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
        Vec<2, int> resolution = Vec<2, int>(0);
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
        const Renderer::Internals& getRendererInternals();

        const Telemetry& getTelemetry() const;

        void addLayer(
            Layer* inLayer,
            Layer::Push inPushTechnique = Layer::Push::Back,
            const std::string& inId = ""
        );

        void run();

        void onEvent(const SDL_Event& inEvent);

        bool isFocused() const;
        void focus();
        void blur();

        void setTitle(const std::string& inTitle);
        void setIcon(const std::string& inIconPath);

        const Vec<2, int>& getSize() const;
        void setSize(const Vec<2, int>& inSize);
        void setSize(int inWidth, int inHeight);

        const Vec<2, int>& getDrawableSize() const;
        void setDrawableSize(const Vec<2, int>& inSize);
        void setDrawableSize(int inWidth, int inHeight);

        const Vec<2, int>& getPosition() const;
        void setPosition(const Vec<2, int>& inPosition);
        void setPosition(int inX, int inY);

        void setViewport(
            const Vec<2, std::uint32_t>& inSize,
            const Vec<2, float>& inPosition = Vec<2, float>(0.0f)
        );

        void setDisplay(int inMonitorIndex);

        void setType(WindowType inType);
        WindowType getType() const;

        bool isResizable();
        void enableResizing();  // Only takes effect when the type is `WindowType::Windowed`
        void disableResizing(); // Only takes effect when the type is `WindowType::Windowed`

        bool isMinimized();

    public:
        SDL_Window* instance;

    private:
        // Fetchers
        void refreshSize();
        void refreshDrawableSize();
        void refreshPosition();

        // Events
        void initRenderer();
        void initCoreLayers();

        void updateTelemetry();

        void onWindowEvent(const SDL_WindowEvent& inEvent);

    private:
        // Telemetry
        std::clock_t m_beginFrame;
        std::clock_t m_endFrame;

        Telemetry m_telemetry;

        // Settings
        WindowType m_type;
        Vec<2, int> m_size;
        Vec<2, int> m_drawableSize;
        Vec<2, int> m_position;

        bool m_isFocused;
        bool m_isResizable;
        bool m_isMinimized; // Only takes effect when the type is `WindowType::Windowed`

        // Render
        std::unique_ptr<Renderer> m_renderer;
        Renderer::Internals m_rendererInternals;
    };
}