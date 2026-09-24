#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "Chicane/Core/List/Push.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Status.hpp"
#include "Chicane/Renderer/Feature.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Sky/Resource.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Resource.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"
#include "Chicane/Renderer/RHI/Scissor.hpp"
#include "Chicane/Renderer/RHI/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Instance;

        class CHICANE_RENDERER Backend
        {
        public:
            friend Instance;

        public:
            using LayerList = std::vector<std::shared_ptr<Layer>>;

        public:
            Backend();
            virtual ~Backend() = default;

        public:
            // Lifecycle
            virtual void onInit();
            virtual void onShutdown();

            // Event
            virtual void onResize();
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource);
            virtual void onLoad(const DrawTextureResource& inResources);
            virtual void onLoad(const DrawSkyResource& inResource);

            // Render
            virtual void onBeginRender();
            virtual void onRender(const Frame& inFrame);
            virtual void onEndRender();

            virtual Draw::Id getScreenTextureId() const;
            virtual bool captureScreen(
                std::uint32_t& outWidth, std::uint32_t& outHeight, std::vector<unsigned char>& outRgba
            );

        public:
            // Renderer
            const Instance* getRenderer() const;
            bool hasFeature(RendererFeature inFeature) const;

            // Layer
            std::vector<Layer*> findLayers(std::function<bool(const Layer* inLayer)> inPredicate) const;

            template <typename Target = Layer>
            inline Target* getLayer(const String& inId) const
            {
                auto found = std::find_if(
                    m_layers.begin(),
                    m_layers.end(),
                    [&inId](const std::shared_ptr<Layer>& inLayer) { return inLayer->getId().equals(inId); }
                );

                if (found == m_layers.end())
                {
                    return nullptr;
                }

                return static_cast<Target*>(found->get());
            }

            template <typename Target = Layer, typename... Params>
            inline void addLayer(const ListPush<Layer*>& inSettings, Params... inParams)
            {
                LayerList::iterator location =
                    inSettings.predicate
                        ? std::find_if(
                              m_layers.begin(),
                              m_layers.end(),
                              [&](const std::shared_ptr<Layer>& inLayer) { return inSettings.predicate(inLayer.get()); }
                          )
                        : m_layers.end();

                switch (inSettings.strategy)
                {
                case ListPushStrategy::Back:
                    location = m_layers.insert(m_layers.end(), std::make_shared<Target>(inParams...));

                    break;

                case ListPushStrategy::Front:
                    location = m_layers.insert(m_layers.begin(), std::make_shared<Target>(inParams...));

                    break;

                case ListPushStrategy::After:
                    location = location != m_layers.end()
                                   ? m_layers.insert(location + 1, std::make_shared<Target>(inParams...))
                                   : m_layers.insert(m_layers.end(), std::make_shared<Target>(inParams...));

                    break;

                case ListPushStrategy::Before:
                    location = location != m_layers.end()
                                   ? m_layers.insert(location, std::make_shared<Target>(inParams...))
                                   : m_layers.insert(m_layers.end(), std::make_shared<Target>(inParams...));

                    break;

                case ListPushStrategy::Replace:
                    if (location == m_layers.end())
                    {
                        location = m_layers.insert(m_layers.end(), std::make_shared<Target>(inParams...));

                        break;
                    }

                    location->get()->onDestruction();
                    *location = std::make_shared<Target>(inParams...);
                    location->get()->setBackend(this);
                    location->get()->onInit();

                    return;

                default:
                    location = m_layers.insert(m_layers.end(), std::make_shared<Target>(inParams...));

                    break;
                }

                location->get()->setBackend(this);
                location->get()->onInit();
            }

            std::size_t getResourceSize(Resource inType);
            std::size_t getResourceBudget(Resource inType);
            std::uint32_t getResourceBudgetCount(Resource inType);

            bool isStatus(BackendStatus inValue) const;

            // RHI
            RHI::Device* getRHIDevice() const;

            virtual RHI::Viewport getRHIViewport(Layer* inLayer) const;
            virtual RHI::Scissor getRHIScissor(Layer* inLayer) const;

            // Telemetry
            float getGpuDelta() const;

        protected:
            bool isScreenComposited(const Frame& inFrame) const;

            // Layer
            Viewport getLayerViewport(Layer* inLayer) const;
            void renderLayers(
                const Frame&                              inFrame,
                void*                                     inData   = nullptr,
                std::function<bool(const Layer* inLayer)> inFilter = nullptr
            );
            void shutdownLayers();
            void rebuildLayers();
            void destroyLayers();

            void setVRAM(std::size_t inBytes);
            void setGpuDelta(float inMilliseconds);

        private:
            void setRenderer(const Instance* inValue);

        protected:
            const Instance*              m_renderer;
            LayerList                    m_layers;
            std::unique_ptr<RHI::Device> m_rhi;
            std::size_t                  m_VRAM;
            float                        m_gpuDelta;

        private:
            BackendStatus m_status;
        };
    }
}