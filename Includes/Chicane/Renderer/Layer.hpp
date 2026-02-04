#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename F = Frame>
        class CHICANE_RENDERER Layer
        {
        public:
            using Id = String;

        public:
            Layer(const Id& inId)
                : m_id(inId),
                  m_status(LayerStatus::Offline),
                  m_backend(nullptr)
            {}

            Layer()
                : m_id("Undefined"),
                  m_status(LayerStatus::Offline),
                  m_backend(nullptr)
            {}

            virtual ~Layer() = default;

        protected:
            virtual bool onInit() { return true; }
            virtual bool onDestroy() { return true; }
            virtual bool onRebuild() { return true; }
            virtual void onResize(const Viewport& inViewport) { return; }
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            virtual void onLoad(const DrawTexture::List& inResources) { return; }
            virtual void onLoad(const DrawSky& inResource) { return; }
            virtual bool onSetup(const F& inFrame) { return true; }
            virtual void onRender(const F& inFrame, void* inData = nullptr) { return; }
            virtual void onCleanup() { return; }

            virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            void init()
            {
                if (!is(LayerStatus::Offline))
                {
                    return;
                }

                if (!onInit())
                {
                    return;
                }

                setStatus(LayerStatus::Running);
            }

            void destroy()
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                if (!onDestroy())
                {
                    return;
                }

                setStatus(LayerStatus::Offline);
            }

            void rebuild()
            {
                if (!is(LayerStatus::Offline))
                {
                    return;
                }

                if (!onRebuild())
                {
                    return;
                }

                setStatus(LayerStatus::Running);
            }

            void resize(const Viewport& inViewport)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onResize(inViewport);
            }

            void load(DrawPolyType inType, const DrawPolyResource& inResource)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inType, inResource);
            }

            void load(const DrawTexture::List& inResources)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inResources);
            }

            void load(const DrawSky& inResource)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inResource);
            }

            void setup(const F& inFrame)
            {
                if (!is(LayerStatus::Running))
                {
                    return;
                }

                if (!onSetup(inFrame))
                {
                    return;
                }

                setStatus(LayerStatus::Running);
            }

            void render(const F& inFrame, void* inData = nullptr)
            {
                if (!is(LayerStatus::Running))
                {
                    return;
                }

                onRender(inFrame, inData);
            }

            void cleanup()
            {
                if (!is(LayerStatus::Running))
                {
                    return;
                }

                onCleanup();
            }

            void handle(const WindowEvent& inEvent) { onEvent(inEvent); }

        public:
            bool is(LayerStatus inStatus) const { return m_status == inStatus; }

            void setStatus(LayerStatus inStatus) { m_status = inStatus; }

            const String& getId() const { return m_id; }

            template <typename T>
            T* getBackend()
            {
                return static_cast<T*>(m_backend);
            }

            void setBackend(void* inBackend) { m_backend = inBackend; }

        protected:
            Id          m_id;
            LayerStatus m_status;

            void*       m_backend;
        };
    }
}