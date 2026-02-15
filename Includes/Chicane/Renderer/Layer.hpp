#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Backend;

        class CHICANE_RENDERER Layer
        {
        public:
            using Id = String;

        public:
            Layer(const Id& inId);
            Layer();

            inline virtual ~Layer() = default;

        protected:
            inline virtual bool onInit() { return true; }
            inline virtual bool onDestroy() { return true; }
            inline virtual bool onRebuild() { return true; }
            inline virtual void onResize(const Vec<2, std::uint32_t>& inResolution) { return; }
            inline virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            inline virtual void onLoad(const DrawTextureResource& inResources) { return; }
            inline virtual void onLoad(const DrawSky& inResource) { return; }
            inline virtual bool onSetup(const Frame& inFrame) { return true; }
            inline virtual void onRender(const Frame& inFrame, void* inData = nullptr) { return; }
            inline virtual void onCleanup() { return; }

            inline virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            // Lifecycle
            void init();
            void destroy();
            void rebuild();
            void resize(const Vec<2, std::uint32_t>& inResolution);
            void load(DrawPolyType inType, const DrawPolyResource& inResource);
            void load(const DrawTextureResource& inResources);
            void load(const DrawSky& inResource);
            bool setup(const Frame& inFrame);
            void render(const Frame& inFrame, void* inData = nullptr);
            void cleanup();

            // Event
            void handle(const WindowEvent& inEvent);

            // Settings
            const String& getId() const;

            bool is(LayerStatus inStatus) const;
            void setStatus(LayerStatus inStatus);

            Viewport getViewport() const;
            void setViewport(const Viewport& inValue);

            template <typename T = Backend>
            inline T* getBackend()
            {
                return static_cast<T*>(m_backend);
            }
            void setBackend(Backend* inBackend);

        protected:
            Id          m_id;
            LayerStatus m_status;
            Backend*    m_backend;

        private:
            Viewport m_viewport;
        };
    }
}