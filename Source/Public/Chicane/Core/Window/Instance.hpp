#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Core/Window/CreateInfo.hpp"
#include "Chicane/Core/Window/Type.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Controller;
    class Level;
    class LevelLayer;
    class SkyboxLayer;

    namespace Window
    {
        class Instance
        {
        public:
            Instance(const CreateInfo& inCreateInfo);
            ~Instance();

        public:
            const Renderer::Internals& getRendererInternals();

            const Telemetry& getTelemetry() const;

            void addLayer(
                Layer* inLayer,
                Layer::PushStrategy inPushTechnique = Layer::PushStrategy::Back,
                const std::string& inId = ""
            );

            void tick();
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
                const Vec<2, float>& inPosition = Vec2Zero
            );

            void setDisplay(int inMonitorIndex);

            void setType(Type inType);
            Type getType() const;

            bool isResizable();
            void enableResizing();  // Only takes effect when the type is `Type::Windowed`
            void disableResizing(); // Only takes effect when the type is `Type::Windowed`

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

        private:
            // Telemetry
            std::clock_t m_beginFrame;
            std::clock_t m_endFrame;

            Telemetry m_telemetry;

            // Settings
            Type        m_type;
            Vec<2, int> m_size;
            Vec<2, int> m_drawableSize;
            Vec<2, int> m_position;

            bool m_bIsFocused;
            bool m_bIsResizable;
            bool m_bIsMinimized; // Only takes effect when the type is `Window::Type::Windowed`

            // Render
            std::unique_ptr<Renderer> m_renderer;
            Renderer::Internals m_rendererInternals;
        };
    }
}