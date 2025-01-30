#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Instance::Instance(Window::Instance* inWindow)
            : m_window(inWindow),
            m_viewport({}),
            m_viewportObservable(std::make_unique<Observable<const Viewport&>>()),
            m_layers({}),
            m_cameras({}),
            m_lights({}),
            m_meshes({})
        {
            loadEvents();
        }

        const Viewport& Instance::getViewport() const
        {
            return m_viewport;
        }

        void Instance::setViewportSize(std::uint32_t inWidth, std::uint32_t inHeight)
        {
            setViewportSize(Vec<2, std::uint32_t>(inWidth, inHeight));
        }

        void Instance::setViewportSize(const Vec<2, std::uint32_t>& inSize)
        {
            m_viewport.size = inSize;

            m_viewportObservable->next(m_viewport);
        }

        void Instance::setViewportPosition(float inX, float inY)
        {
            setViewportPosition(Vec<2, float>(inX, inY));
        }

        void Instance::setViewportPosition(const Vec<2, float>& inPosition)
        {
            m_viewport.position = inPosition;

            m_viewportObservable->next(m_viewport);
        }

        void Instance::setViewport(const Viewport& inViewport)
        {
            m_viewport = inViewport;

            m_viewportObservable->next(m_viewport);
        }

        Subscription<const Viewport&>* Instance::watchViewport(
            std::function<void (const Viewport&)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        )
        {
            Subscription<const Viewport&>* subscription = m_viewportObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_viewport);

            return subscription;
        }

        bool Instance::hasLayer(Layer::Instance* inLayer)
        {
            if (!inLayer)
            {
                return false;
            }

            return hasLayer(inLayer->getId());
        }

        bool Instance::hasLayer(const std::string& inId)
        {
            return std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            ) != m_layers.end();
        }

        void Instance::pushLayerStart(Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            m_layers.insert(
                m_layers.begin(),
                inLayer
            );

            inLayer->build();
        }

        void Instance::pushLayer(
            Layer::Instance* inLayer,
            Layer::PushStrategy inPushStrategy,
            const std::string& inId
        )
        {
            if (!inLayer)
            {
                return;
            }

            switch (inPushStrategy)
            {
            case Layer::PushStrategy::Front:
                pushLayerStart(inLayer);

                return;

            case Layer::PushStrategy::BeforeLayer:
                pushLayerBefore(
                    inId,
                    inLayer
                );

                return;

            case Layer::PushStrategy::AfterLayer:
                pushLayerAfter(
                    inId,
                    inLayer
                );

                return;

            default:
                pushLayerBack(inLayer);

                return;
            }
        }

        void Instance::pushLayerBack(Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            m_layers.push_back(inLayer);

            inLayer->build();
        }

        void Instance::pushLayerBefore(const std::string& inId, Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            auto foundLayer = std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            );

            if (foundLayer == m_layers.end())
            {
                return;
            }

            m_layers.insert(
                foundLayer,
                inLayer
            );

            inLayer->build();
        }

        void Instance::pushLayerAfter(const std::string& inId, Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            auto foundLayer = std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            );

            if (foundLayer == m_layers.end())
            {
                return;
            }

            m_layers.insert(
                foundLayer + 1,
                inLayer
            );

            inLayer->build();
        }

        void Instance::emmitEventToLayers(const SDL_Event& inEvent)
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onEvent(inEvent);
            }
        }

        void Instance::destroyLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->destroy();
            }
        }

        void Instance::rebuildLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->rebuild();
            }
        }

        void Instance::deleteLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                delete layer;
            }

            m_layers.clear();
        }

        void Instance::loadEvents()
        {
            watchViewport(
                [this](const Viewport& inViewport)
                {
                    updateViewComponents();
                }
            );

            Application::watchLevel(
                [this](Level* inLevel) {
                    if (!inLevel)
                    {
                        return;
                    }

                    inLevel->watchComponents(
                        [this](const std::vector<Component*>& inComponents) {
                            m_cameras.clear();
                            m_lights.clear();
                            m_meshes.clear();

                            for (Component* component : inComponents)
                            {
                                if (component->isType<CCamera>())
                                {
                                    m_cameras.push_back(static_cast<CCamera*>(component));
                                }

                                if (component->isType<CLight>())
                                {
                                    m_lights.push_back(static_cast<CLight*>(component));
                                }

                                if (component->isType<CMesh>())
                                {
                                    m_meshes.push_back(static_cast<CMesh*>(component));
                                }
                            }

                            std::sort(
                                m_meshes.begin(),
                                m_meshes.end(),
                                [](CMesh* inA, CMesh* inB)
                                {
                                    return strcmp(inA->getModel().c_str(), inB->getModel().c_str()) > 0;
                                }
                            );

                            updateViewComponents();
                        }
                    );
                }
            );
        }

        void Instance::updateViewComponents()
        {
            for (CCamera* camera : m_cameras)
            {
                camera->setViewport(m_viewport.size);
            }

            for (CLight* light : m_lights)
            {
                light->setViewport(m_viewport.size);
            }
        }
    }
}