#pragma once

#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"

namespace Chicane
{
    class CCamera;
    class CLight;
    class CMesh;

    class CHICANE_RUNTIME Renderer
    {
    public:
        struct CreateInfo
        {
        public:
            Vec2 resolution = Vec2::Zero;
            Vec2 position   = Vec2::Zero;
        };

        struct Viewport
        {
        public:
            Vec2 size     = Vec2::Zero;
            Vec2 position = Vec2::Zero;
        };

    public:
        using ViewportObservable   = Observable<Viewport>;
        using ViewportSubscription = Subscription<Viewport>;

    protected:
        using Super = Renderer;

    public:
        Renderer();
        virtual ~Renderer() = default;

    protected:
        virtual void onInit() { return; }
        virtual void onRender() { return; }
        virtual void onEvent(const Window::Event& inEvent) { return; }
        virtual void onResizing() { return; }
        virtual void onRepositioning() { return; }

    public:
        // Status
        bool canRender() const;

        // Lifecycle
        void init(const CreateInfo& inCreateInfo);
        void render();

        // Settings
        const Vec2& getSize() const;
        void setSize(const Vec2& inValue);
        void setSize(float inWidth, float inHeight);

        const Vec2& getPosition() const;
        void setPosition(const Vec2& inValue);
        void setPosition(float inX, float inY);

        // Layers
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

        void setupLayers();
        void destroyLayers();
        void rebuildLayers();
        void deleteLayers();

    private:
        // Events
        void loadEvents();
        void updateViewComponents();

    protected:
        // Settings
        Vec2                          m_size;
        Vec2                          m_position;

        // Layer
        std::vector<Layer::Instance*> m_layers;

        // Game
        std::vector<CCamera*>         m_cameras;
        std::vector<CLight*>          m_lights;
        std::vector<CMesh*>           m_meshes;
    };
}