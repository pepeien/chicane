#pragma once

#include <memory>
#include <unordered_map>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Backend.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Type.hpp"
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

            // Frame
            const Frame& getCurrentFrame() const;
            Frame& getCurrentFrame();

            // View
            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);

            // Render
            const Draw::List& getDraws(DrawType inType) const;
            const Vertex::List& getVertices(DrawType inType) const;
            const Vertex::Indices& getIndices(DrawType inType) const;

            Draw::Id load(DrawType inType, const DrawData& inData);

            template <typename T>
            inline void draw(DrawType inType, const String& inReference, const T& inInstance)
            {
                Draw::Id id = getResource(inType).find(inReference);

                if (id <= Draw::UnknownId)
                {
                    return;
                }

                draw(id, inInstance);
            }

            template <typename T>
            inline void draw(Draw::Id inId, const T& inInstance)
            {
                getCurrentFrame().use(inId, inInstance);
            }

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
            DrawResource& getResource(DrawType inType);
            const DrawResource& getResource(DrawType inType) const;

        private:
            // Window
            Window*                        m_window;

            // Frame
            std::array<Frame, FRAME_COUNT> m_frames;
            std::uint32_t                  m_currentFrame;

            // Draw
            DrawResource::Map              m_drawResources;

            // Settings
            Viewport                       m_viewport;

            // Backend
            std::unique_ptr<Backend>       m_backend;
        };
    }
}