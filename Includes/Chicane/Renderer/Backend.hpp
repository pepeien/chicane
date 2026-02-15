#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/Window.hpp"

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
        class CHICANE_RENDERER Backend
        {
        public:
            using LayerList = std::vector<std::unique_ptr<Layer>>;

        public:
            Backend();

            inline virtual ~Backend() = default;

        public:
            virtual void onInit();
            virtual void onResize(const Vec<2, std::uint32_t>& inResolution);
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource);
            virtual void onLoad(const DrawTextureResource& inResources);
            virtual void onLoad(const DrawSky& inResource);
            virtual void onSetup();
            virtual void onRender(const Frame& inFrame);
            virtual void onCleanup();
            virtual void onHandle(const WindowEvent& inEvent);

        public:
            // Window
            const Window* getWindow() const;
            void setWindow(const Window* inWindow);

            // Resolution
            const Vec<2, std::uint32_t>& getResolution() const;

            // Layer
            std::vector<Layer*> findLayers(std::function<bool(Layer* inLayer)> inPredicate) const;

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
                location->get()->init();
            }

        protected:
            // Layer
            void renderLayers(const Frame& inFrame, void* inData = nullptr);
            void destroyLayers();
            void rebuildLayers();
            void deleteLayers();

        protected:
            const Window*         m_window;
            Vec<2, std::uint32_t> m_resolution;
            LayerList             m_layers;
        };
    }
}