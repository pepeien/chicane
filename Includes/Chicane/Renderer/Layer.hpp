#pragma once

#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Viewport.hpp"
#include "Chicane/Renderer/Viewport/Settings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Backend;

        class CHICANE_RENDERER Layer
        {
            friend Backend;

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

            const ViewportSettings& getViewport() const;
            void setViewport(const ViewportSettings& inValue);

            template <typename T = Backend>
            inline T* getBackend() const
            {
                return static_cast<T*>(m_backend);
            }
            void setBackend(Backend* inBackend);

        protected:
            Id               m_id;
            Backend*         m_backend;
            ViewportSettings m_viewport;
        };
    }
}