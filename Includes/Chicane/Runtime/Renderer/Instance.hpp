#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"
#include "Chicane/Runtime/Renderer/Viewport.hpp"

namespace Chicane
{
    class CCamera;
    class CLight;
    class CMesh;

    namespace Renderer
    {
        class CHICANE_RUNTIME Instance
        {
        public:
            using ViewportObservable   = Observable<Viewport>;
            using ViewportSubscription = Subscription<Viewport>;

        protected:
            using Super = Instance;

        public:
            Instance();
            virtual ~Instance() = default;

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
            bool hasLayer(const String& inId) const;

            void pushLayer(
                Layer::Instance* inLayer,
                Layer::PushStrategy inPushStrategy = Layer::PushStrategy::Back,
                const String& inId = ""
            );

        protected:
            void pushLayerStart(Layer::Instance* inLayer);
            void pushLayerBack(Layer::Instance* inLayer);
            void pushLayerBefore(const String& inId, Layer::Instance* inLayer);
            void pushLayerAfter(const String& inId, Layer::Instance* inLayer);

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
}