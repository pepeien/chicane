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
#include "Chicane/Renderer/Draw/Sky/Data.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Settings.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    class Application;

    namespace Renderer
    {
        using ResolutionObservable   = EventObservable<Vec<2, int>>;
        using ResolutionSubscription = EventSubscription<Vec<2, int>>;

        using ViewportObservable   = EventObservable<Viewport>;
        using ViewportSubscription = EventSubscription<Viewport>;

        class CHICANE_RENDERER Instance
        {
            friend Application;

        public:
            static inline constexpr const std::uint32_t FRAME_COUNT = 2;

        public:
            Instance();
            ~Instance();

        public:
            // Lifecycle
            void init(Window* inWindow, WindowBackend inBackend, const Settings& inSettings);
            void render();
            void destroy();

            // View
            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);

            // Render
            Draw::Id loadPoly(DrawPolyType inType, const DrawPolyData& inData);

            template <typename T>
            inline void drawPoly(DrawPolyType inType, const Draw::Reference& inReference, const T& inInstance)
            {
                Draw::Id id = getPolyResource(inType).findId(inReference);

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

            Draw::Id findTexture(const Draw::Reference& inReference);
            Draw::Id loadTexture(const DrawTextureData& inData);

            Draw::Id findSky(const Draw::Reference& inReference);
            Draw::Id loadSky(const DrawSkyData& inData);

            // Settings
            const Vec<2, int>& getResolution() const;
            void setResolution(const Vec<2, int>& inValue);
            ResolutionSubscription watchResolution(
                ResolutionSubscription::NextCallback     inNext,
                ResolutionSubscription::ErrorCallback    inError    = nullptr,
                ResolutionSubscription::CompleteCallback inComplete = nullptr
            );

            const Viewport& getViewport() const;
            void setViewport(const Viewport& inValue);
            void setViewport(const Vec2& inPosition, const Vec2& inSize);
            void setViewportPosition(const Vec2& inPosition);
            void setViewportPosition(float inX, float inY);
            void setViewportSize(const Vec2& inSize);
            void setViewportSize(float inWidth, float inHeight);
            ViewportSubscription watchViewport(
                ViewportSubscription::NextCallback     inNext,
                ViewportSubscription::ErrorCallback    inError    = nullptr,
                ViewportSubscription::CompleteCallback inComplete = nullptr
            );

            // Window
            Window* getWindow() const;
            void setWindow(Window* inWindow);
            void handle(const WindowEvent& inEvent);

            // Backend
            bool hasBackend() const;
            Backend* getBackend();

        protected:
            void setBackend(WindowBackend inType);

        private:
            // Frame
            Frame& getCurrentFrame();

            // Draw
            DrawPolyResource& getPolyResource(DrawPolyType inType);
            void reloadResources();

            // Settings
            void refreshViewport();
            void propagateResize();

        private:
            // Window
            Window*                        m_window;

            // Frame
            std::array<Frame, FRAME_COUNT> m_frames;
            std::uint32_t                  m_currentFrame;

            // Draw
            DrawPolyResource::Map          m_polyResources;
            DrawTexture::List              m_textureResources;
            DrawSky                        m_skyResource;

            // Settings
            Vec<2, int>                    m_resolution;
            ResolutionObservable           m_resolutionObservable;

            Viewport                       m_viewport;
            ViewportObservable             m_viewportObservable;

            // Backend
            std::unique_ptr<Backend>       m_backend;
        };
    }
}