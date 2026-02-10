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
        using ResolutionObservable   = EventObservable<Vec<2, std::uint32_t>>;
        using ResolutionSubscription = EventSubscription<Vec<2, std::uint32_t>>;

        class CHICANE_RENDERER Instance
        {
            friend Application;

        public:
            Instance();

        public:
            // Lifecycle
            void init(const Settings& inSettings);
            void render();

            // View
            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);

            // Frame
            void setFrameCount(std::uint32_t inValue);
            Frame& getCurrentFrame();

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
            const Vec<2, std::uint32_t>& getResolution() const;
            void setResolution(const Vec<2, std::uint32_t>& inValue);
            ResolutionSubscription watchResolution(
                ResolutionSubscription::NextCallback     inNext,
                ResolutionSubscription::ErrorCallback    inError    = nullptr,
                ResolutionSubscription::CompleteCallback inComplete = nullptr
            );

            // Backend
            bool hasBackend() const;

            template <typename Target = Layer<>, typename... Params>
            inline void addBackendLayer(const ListPush<Layer<>*>& inSettings, Params... inParams)
            {
                m_backend->addLayer<Target>(inSettings, inParams...);
            }

        protected:
            // Window
            void handle(const WindowEvent& inEvent);

            // Backend
            void shutdownBackend();
            void reloadBackend(const Window* inWindow);

            // Draw
            DrawPolyResource& getPolyResource(DrawPolyType inType);

        private:
            // Settings
            Vec<2, std::uint32_t>      m_resolution;
            ResolutionObservable       m_resolutionObservable;

            // Frame
            std::vector<Frame>         m_frames;
            std::uint32_t              m_currentFrame;

            // Draw
            DrawPolyResource::Map      m_polyResources;
            DrawTexture::List          m_textureResources;
            DrawSky                    m_skyResource;

            // Backend
            std::unique_ptr<Backend<>> m_backend;
        };
    }
}