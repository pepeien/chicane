#pragma once

#include "Chicane/Core/List.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Instance;

        class CHICANE_RENDERER Backend
        {
            friend Instance;

        public:
            using LayerList = std::vector<std::unique_ptr<Layer>>;

        public:
            static constexpr inline const std::size_t TEXTURE_COUNT = 512;

            static constexpr inline const std::size_t SCENE_INDEX_COUNT    = 1048576; // 1M (2^20)
            static constexpr inline const std::size_t SCENE_VERTEX_COUNT   = SCENE_INDEX_COUNT * 3;
            static constexpr inline const std::size_t SCENE_INSTANCE_COUNT = 4096;

            static constexpr inline const std::size_t UI_INDEX_COUNT    = 65536; // 64K (2^16)
            static constexpr inline const std::size_t UI_VERTEX_COUNT   = UI_INDEX_COUNT * 3;
            static constexpr inline const std::size_t UI_INSTANCE_COUNT = 2048;

        public:
            Backend();

        public:
            // Lifecycle
            virtual void onInit();
            virtual void onShutdown();

            // Event
            virtual void onResize();
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource);
            virtual void onLoad(const DrawTextureResource& inResources);
            virtual void onLoad(const DrawSky& inResource);

            // Render
            virtual void onBeginRender();
            virtual void onRender(const Frame& inFrame);
            virtual void onEndRender();

        public:
            // Renderer
            const Instance* getRenderer() const;

            // Layer
            std::vector<Layer*> findLayers(std::function<bool(const Layer* inLayer)> inPredicate) const;

            template <typename Target = Layer>
            inline Target* getLayer(const String& inId) const
            {
                auto found = std::find_if(
                    m_layers.begin(),
                    m_layers.end(),
                    [&inId](const std::unique_ptr<Layer>& inLayer) { return inLayer->getId().equals(inId); }
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
                              [&](const std::unique_ptr<Layer>& inLayer) { return inSettings.predicate(inLayer.get()); }
                          )
                        : m_layers.end();

                switch (inSettings.strategy)
                {
                case ListPushStrategy::Back:
                    location = m_layers.insert(m_layers.end(), std::make_unique<Target>(inParams...));

                    break;

                case ListPushStrategy::Front:
                    location = m_layers.insert(m_layers.begin(), std::make_unique<Target>(inParams...));

                    break;

                case ListPushStrategy::After:
                    location = location != m_layers.end()
                                   ? m_layers.insert(location + 1, std::make_unique<Target>(inParams...))
                                   : m_layers.insert(m_layers.end(), std::make_unique<Target>(inParams...));

                    break;

                case ListPushStrategy::Before:
                    location = location != m_layers.end()
                                   ? m_layers.insert(location, std::make_unique<Target>(inParams...))
                                   : m_layers.insert(m_layers.end(), std::make_unique<Target>(inParams...));

                    break;

                default:
                    location = m_layers.insert(m_layers.end(), std::make_unique<Target>(inParams...));

                    break;
                }

                location->get()->setBackend(this);
                location->get()->onInit();
            }

        protected:
            // Layer
            Viewport getLayerViewport(Layer* inLayer) const;
            void renderLayers(const Frame& inFrame, void* inData = nullptr);
            void shutdownLayers();
            void rebuildLayers();
            void destroyLayers();

        private:
            void setRenderer(const Instance* inValue);

        protected:
            const Instance* m_renderer;
            LayerList       m_layers;
        };
    }
}