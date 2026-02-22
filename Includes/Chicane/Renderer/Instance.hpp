#pragma once

#include <memory>
#include <unordered_map>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Backend.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Sky/Data.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Settings.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    class Application;

    namespace Renderer
    {
        class CHICANE_RENDERER Instance
        {
            friend Application;

        public:
            Instance();

        public:
            // Lifecycle
            void init(const Settings& inSettings);
            void shutdown();

            // Render
            void render();

            // View
            void useCamera(const View& inData);
            void addLight(const View& inData);
            void addLight(const View::List& inData);

            // Frame
            Frame& getCurrentFrame();

            // Render
            Draw::Id findPoly(DrawPolyType inType, const Draw::Reference& inReference);
            Draw::Id loadPoly(DrawPolyType inType, const DrawPolyData& inData);

            template <typename T>
            inline void drawPoly(DrawPolyType inType, const Draw::Reference& inReference, const T& inInstance)
            {
                Draw::Id id = findPoly(inType, inReference);

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

            // Window
            bool hasWindow() const;
            const Window* getWindow() const;
            void setWindow(const Window* inWindow);

            // Settings
            Vec<2, std::uint32_t> getResolution() const;
            void setResolution(const Vec<2, std::uint32_t>& inValue);

            std::uint32_t getFrameInFlighCount() const;
            void setFramesInFlight(std::uint32_t inValue);

            const ResourceBudget& getResourceBudget() const;
            void setResourceBudget(const ResourceBudget& inValue);

            // Backend
            bool hasBackend() const;
            template <typename Target = Layer, typename... Params>
            inline void addBackendLayer(const ListPush<Layer*>& inSettings, Params... inParams)
            {
                m_backend->addLayer<Target>(inSettings, inParams...);
            }

        private:
            // Backend
            void reloadBackend();

            // Draw
            DrawPolyResource& getPolyResource(DrawPolyType inType);

        private:
            // Window
            const Window*            m_window;

            // Settings
            Settings                 m_settings;

            // Frame
            std::vector<Frame>       m_frames;
            std::uint32_t            m_currentFrame;

            // Draw
            DrawPolyResource::Map    m_polyResources;
            DrawTextureResource      m_textureResources;
            DrawSky                  m_skyResource;

            // Backend
            std::unique_ptr<Backend> m_backend;
        };
    }
}