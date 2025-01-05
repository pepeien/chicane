#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    class Window;
    class CameraComponent;

    class Renderer
    {
    public:
        enum class Type : std::uint8_t
        {
            Undefined,
            Vulkan
        };

    public:
        Renderer(Window* inWindow);

    public:
        // Event
        virtual void onEvent(const SDL_Event& inEvent);

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