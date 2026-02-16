#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"

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

        public:
            // Lifecycle
            virtual void onInit() { return; }
            virtual void onShutdown() { return; }
            virtual void onRestart() { return; }
            virtual void onDestruction() { return; }

            // Event
            virtual void onResize(const Vec<2, std::uint32_t>& inResolution) { return; }
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            virtual void onLoad(const DrawTextureResource& inResources) { return; }
            virtual void onLoad(const DrawSky& inResource) { return; }

            // Render
            virtual bool onBeginRender(const Frame& inFrame) { return true; }
            virtual void onRender(const Frame& inFrame, void* inData = nullptr) { return; }
            virtual void onEndRender() { return; }

        public:
            // Settings
            const String& getId() const;

            Viewport getViewport() const;
            void setViewport(const Viewport& inValue);

            template <typename T = Backend>
            inline T* getBackend()
            {
                return static_cast<T*>(m_backend);
            }
            void setBackend(Backend* inBackend);

        protected:
            Id       m_id;
            Backend* m_backend;
            Viewport m_viewport;
        };
    }
}