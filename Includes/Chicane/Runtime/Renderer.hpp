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

        const Vec2& getPosition() const;
        void setPosition(const Vec2& inValue);
        void setPosition(float inX, float inY);

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

        template <
            typename Target,
            typename Anchor = RendererLayer,
            typename... Params
        >
        void pushLayer(
            ListPushStrategy inStrategy = ListPushStrategy::Back,
            Params... inParams
        )
        {
            m_layers.add(new Target(inParams...), inStrategy);
        }

        const std::vector<CMesh*>& getMeshes() const;

    protected:
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
        Vec2                  m_size;
        Vec2                  m_position;

        // Window
        Window*               m_window;

        // Layer
        List<RendererLayer*>  m_layers;

        // Game
        std::vector<CCamera*> m_cameras;
        std::vector<CLight*>  m_lights;
        std::vector<CMesh*>   m_meshes;
    };
}