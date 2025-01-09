#pragma once

#include "Chicane/Core/Window.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Instance
        {
        public:
            Instance(Window::Instance* inWindow);
            virtual ~Instance() = default;

        public:
            // Setup
            virtual void initLayers() { return; };

            // Event
            virtual void onEvent(const SDL_Event& inEvent) { emmitEventToLayers(inEvent); };

            // Render
            virtual void render() { return; };

        public:
            // Layer
            bool hasLayer(Layer::Instance* inLayer);
            bool hasLayer(const std::string& inId);

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

        protected:
            // Window
            Window::Instance*   m_window;

            // Layer
            std::vector<Layer::Instance*> m_layers;
        };
    }
}