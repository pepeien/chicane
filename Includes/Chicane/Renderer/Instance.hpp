#pragma once

#include <memory>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Backend.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/2D/Data.hpp"
#include "Chicane/Renderer/Draw/3D/Data.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER Instance
        {
        public:
            using ViewportObservable   = EventObservable<Viewport>;
            using ViewportSubscription = EventSubscription<Viewport>;

        public:
            static inline constexpr const std::uint32_t FRAME_COUNT = 2;

        public:
            Instance();
            ~Instance();

        public:
            // Lifecycle
            void init(Window* inWindow, WindowBackend inBackend);
            void render();
            void destroy();

            // Render
            const Frame& getCurrentFrame() const;
            Frame& getCurrentFrame();

            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);
            void draw(const DrawData2D& inData);
            void draw(const DrawData3D& inData);

            // Settings
            const Viewport& getViewport() const;
            void setViewport(const Viewport& inValue);
            void setViewport(const Vec2& inPosition, const Vec2& inSize);
            void setViewportPosition(const Vec2& inPosition);
            void setViewportPosition(float inX, float inY);
            void setViewportSize(const Vec2& inSize);
            void setViewportSize(float inWidth, float inHeight);

            // Window
            Window* getWindow() const;
            void setWindow(Window* inWindow);
            void handle(const WindowEvent& inEvent);

            // Backend
            bool hasBackend() const;
            void setBackend(WindowBackend inType);

        private:
            // Window
            Window*                        m_window;

            // Draws
            std::array<Frame, FRAME_COUNT> m_frames;
            std::uint32_t                  m_currentFrame;

            // Settings
            Viewport                       m_viewport;

            // Backend
            std::unique_ptr<Backend>       m_backend;
        };
    }
}