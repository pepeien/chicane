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
            inline Layer(const Id& inId)
                : m_id(inId),
                  m_status(LayerStatus::Offline),
                  m_backend(nullptr)
            {}

            inline Layer()
                : m_id("Undefined"),
                  m_status(LayerStatus::Offline),
                  m_backend(nullptr)
            {}

            inline virtual ~Layer() = default;

        protected:
            inline virtual bool onInit() { return true; }
            inline virtual bool onDestroy() { return true; }
            inline virtual bool onRebuild() { return true; }
            inline virtual void onResize(const Vec<2, std::uint32_t>& inResolution) { return; }
            inline virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            inline virtual void onLoad(const DrawTexture::List& inResources) { return; }
            inline virtual void onLoad(const DrawSky& inResource) { return; }
            inline virtual bool onSetup(const F& inFrame) { return true; }
            inline virtual void onRender(const F& inFrame, void* inData = nullptr) { return; }
            inline virtual void onCleanup() { return; }

            inline virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            inline void init()
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

            inline void destroy()
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

            inline void rebuild()
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

            inline void resize(const Vec<2, std::uint32_t>& inResolution)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onResize(inResolution);
            }

            inline void load(DrawPolyType inType, const DrawPolyResource& inResource)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inType, inResource);
            }

            inline void load(const DrawTexture::List& inResources)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inResources);
            }

            inline void load(const DrawSky& inResource)
            {
                if (is(LayerStatus::Offline))
                {
                    return;
                }

                onLoad(inResource);
            }

            inline bool setup(const F& inFrame)
            {
                if (!is(LayerStatus::Running))
                {
                    return false;
                }

                if (!onSetup(inFrame))
                {
                    return false;
                }

                setStatus(LayerStatus::Running);

                return true;
            }

            inline void render(const F& inFrame, void* inData = nullptr)
            {
                if (!is(LayerStatus::Running))
                {
                    return;
                }

                onRender(inFrame, inData);
            }

            inline void cleanup()
            {
                if (!is(LayerStatus::Running))
                {
                    return;
                }

                onCleanup();
            }

            inline void handle(const WindowEvent& inEvent) { onEvent(inEvent); }

        public:
            inline const String& getId() const { return m_id; }

            inline bool is(LayerStatus inStatus) const { return m_status == inStatus; }

            inline void setStatus(LayerStatus inStatus) { m_status = inStatus; }

            inline const Viewport& getViewport() const { return m_viewport; }

            inline void setViewport(const Viewport& inValue) { m_viewport = inValue; }

            template <typename T>
            inline T* getBackend()
            {
                return static_cast<T*>(m_backend);
            }

            inline void setBackend(void* inBackend) { m_backend = inBackend; }

        protected:
            Id          m_id;
            LayerStatus m_status;
            Viewport    m_viewport;
            void*       m_backend;
        };
    }
}