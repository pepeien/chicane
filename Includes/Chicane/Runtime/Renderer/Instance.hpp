#pragma once

#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Runtime/Renderer/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/Viewport.hpp"

namespace Chicane
{
    class CCamera;
    class CLight;
    class CMesh;

    namespace Renderer
    {
        class CHICANE Instance
        {
        public:
            typedef Instance Super;

        public:
            Instance(const CreateInfo& inCreateInfo, Window::Instance* inWindow);
            virtual ~Instance() = default;

        public:
            // Setup
            virtual void initLayers() { return; };

            // Event
            virtual void onEvent(const SDL_Event& inEvent) { emmitEventToLayers(inEvent); };

            // Render
            virtual void render() { return; };

        public:
            // Settings
            const Viewport& getViewport() const;
            void setViewportSize(std::uint32_t inWidth, std::uint32_t inHeight);
            void setViewportSize(const Vec<2, std::uint32_t>& inSize);
            void setViewportPosition(float inX, float inY);
            void setViewportPosition(const Vec<2, float>& inPosition);
            void setViewport(const Viewport& inViewport);
            Subscription<const Viewport&>* watchViewport(
                std::function<void (const Viewport&)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

            // Layer
            bool hasLayer(Layer::Instance* inLayer) const;
            bool hasLayer(const std::string& inId) const;

            void pushLayer(
                Layer::Instance* inLayer,
                Layer::PushStrategy inPushStrategy = Layer::PushStrategy::Back,
                const std::string& inId = ""
            );

        protected:
            void pushLayerStart(Layer::Instance* inLayer);
            void pushLayerBack(Layer::Instance* inLayer);
            void pushLayerBefore(const std::string& inId, Layer::Instance* inLayer);
            void pushLayerAfter(const std::string& inId, Layer::Instance* inLayer);

            void emmitEventToLayers(const SDL_Event& inEvent);
            void destroyLayers();
            void rebuildLayers();
            void deleteLayers();

        private:
            // Events
            void loadEvents();
            void updateViewComponents();

        protected:
            // Window
            Window::Instance*                            m_window;

            // Settings
            Viewport                                     m_viewport;  
            std::unique_ptr<Observable<const Viewport&>> m_viewportObservable;      

            // Layer
            std::vector<Layer::Instance*>                m_layers;

            // Game
            std::vector<CCamera*>                        m_cameras;
            std::vector<CLight*>                         m_lights;
            std::vector<CMesh*>                          m_meshes;
        };
    }
}