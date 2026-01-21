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
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
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

            // View
            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);

            // Render
            Draw::Id loadPoly(DrawPolyType inType, const DrawPolyData& inData);

            template <typename T>
            inline void drawPoly(DrawPolyType inType, const String& inReference, const T& inInstance)
            {
                Draw::Id id = getPolyResource(inType).find(inReference);

                if (id <= Draw::UnknownId)
                {
                    return;
                }

                drawPoly(id, inInstance);
            }

            template <typename T>
            inline void drawPoly(Draw::Id inId, const T& inInstance)
            {
                getCurrentFrame().use(inId, inInstance);
            }

            Draw::Id findTexture(const String& inReference);
            Draw::Id loadTexture(const DrawTextureData& inData);

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
            // Frame
            Frame& getCurrentFrame();

            // Draw
            DrawPolyResource& getPolyResource(DrawPolyType inType);

        private:
            // Window
            Window*                        m_window;

            // Frame
            std::array<Frame, FRAME_COUNT> m_frames;
            std::uint32_t                  m_currentFrame;

            // Draw
            DrawPolyResource::Map          m_polyResources;
            DrawTexture::List              m_textureResources;

            // Settings
            Viewport                       m_viewport;

            // Backend
            std::unique_ptr<Backend>       m_backend;
        };
    }
}