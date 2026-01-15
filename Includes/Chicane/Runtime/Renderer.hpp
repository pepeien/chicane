#pragma once

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"
#include "Chicane/Runtime/Renderer/Viewport.hpp"

namespace Chicane
{
    class CCamera;
    class CLight;
    class CMesh;

    class CHICANE_RUNTIME Renderer
    {
    public:
        using ViewportObservable   = Observable<RendererViewport>;
        using ViewportSubscription = Subscription<RendererViewport>;

        using SizeObservable       = Observable<const Vec2&>;
        using SizeSubscription     = Subscription<const Vec2&>;

        using PositionObservable   = Observable<const Vec2&>;
        using PositionSubscription = Subscription<const Vec2&>;

    protected:
        using Super = Renderer;

    public:
        Renderer();
        virtual ~Renderer() = default;

    protected:
        virtual void onInit() { return; }
        virtual void onRender() { return; }
        virtual void onEvent(const WindowEvent& inEvent) { return; }
        virtual void onResizing() { return; }
        virtual void onRepositioning() { return; }

    public:
        // Status
        bool canRender() const;

        // Lifecycle
        void init(Window* inWindow);
        void render();

        // Settings
        const Vec2& getSize() const;
        void setSize(const Vec2& inValue);
        void setSize(float inWidth, float inHeight);
        SizeSubscription watchSize(
            SizeObservable::NextCallback     inNext,
            SizeObservable::ErrorCallback    inError    = nullptr,
            SizeObservable::CompleteCallback inComplete = nullptr
        );

        const Vec2& getPosition() const;
        void setPosition(const Vec2& inValue);
        void setPosition(float inX, float inY);
        PositionSubscription watchPosition(
            PositionObservable::NextCallback     inNext,
            PositionObservable::ErrorCallback    inError    = nullptr,
            PositionObservable::CompleteCallback inComplete = nullptr
        );

        // Window
        Window* getWindow() const;
        void setWindow(Window* inWindow);

        // Layers
        template <typename T>
        bool hasLayer() const
        {
            for (const RendererLayer* layer : m_layers)
            {
                if (typeid(T) != typeid(*layer))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        template <typename Target, typename Anchor = RendererLayer, typename... Params>
        void pushLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
        {
            m_layers.add(new Target(inParams...), inStrategy);
        }

        const std::vector<CMesh*>& getMeshes() const;

    protected:
        void setupLayers();
        void cleanupLayers();
        void destroyLayers();
        void rebuildLayers();
        void deleteLayers();

        void setupComponents();

    private:
        // Events
        void loadEvents();

    protected:
        // Settings
        Vec2                 m_size;
        SizeObservable       m_sizeOberservable;

        Vec2                 m_position;
        PositionObservable   m_positionOberservable;

        // Window
        Window*              m_window;

        // Layer
        List<RendererLayer*> m_layers;

        // Scene
        CCamera*             m_camera;
        CLight*              m_light;
        std::vector<CMesh*>  m_meshes;
    };
}