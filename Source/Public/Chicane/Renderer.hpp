#pragma once

#include "Chicane/Renderer/Camera.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Manager.hpp"
#include "Chicane/Renderer/Mesh.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Texture.hpp"
#include "Chicane/Renderer/Type.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    class Window;

    class Renderer
    {
    public:
        Renderer(Window* inWindow);
        virtual ~Renderer() = default;

    public:
        // Setup
        virtual void initLayers() { return; };

        // Event
        virtual void onEvent(const SDL_Event& inEvent) { emmitEventToLayers(inEvent); };

        // Render
        virtual void render() { return; };

    public:
        // Layer
        bool hasLayer(Layer* inLayer);
        bool hasLayer(const std::string& inId);

        void pushLayer(
            Layer* inLayer,
            Layer::PushStrategy inPushStrategy = Layer::PushStrategy::Back,
            const std::string& inId = ""
        );

    protected:
        void pushLayerStart(Layer* inLayer);
        void pushLayerBack(Layer* inLayer);
        void pushLayerBefore(const std::string& inId, Layer* inLayer);
        void pushLayerAfter(const std::string& inId, Layer* inLayer);

        void emmitEventToLayers(const SDL_Event& inEvent);
        void destroyLayers();
        void rebuildLayers();
        void deleteLayers();

    protected:
        // Window
        Window*             m_window;

        // Layer
        std::vector<Layer*> m_layers;
    };
}