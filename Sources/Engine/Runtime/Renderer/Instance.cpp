#include "Chicane/Runtime/Renderer/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Instance::Instance()
            : m_size(Vec2::Zero),
            m_position(Vec2::Zero),
            m_layers({}),
            m_cameras({}),
            m_lights({}),
            m_meshes({})
        {
            loadEvents();
        }

        bool Instance::canRender() const
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer->is(Layer::Status::Running))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        void Instance::init(const CreateInfo& inCreateInfo)
        {
            setSize(inCreateInfo.resolution);
            setPosition(inCreateInfo.position);

            onInit();
        }

        void Instance::render()
        {
            setupLayers();

            if (!canRender())
            {
                return;
            }

            onRender();
        }

        const Vec2& Instance::getSize() const
        {
            return m_size;
        }

        void Instance::setSize(const Vec2& inValue)
        {
            setSize(inValue.x, inValue.y);
        }

        void Instance::setSize(float inWidth, float inHeight)
        {
            if (
                std::fabs(m_size.x - inWidth) < FLT_EPSILON &&
                std::fabs(m_size.y - inHeight) < FLT_EPSILON
            )
            {
                return;
            }

            m_size.x = inWidth;
            m_size.y = inHeight;

            onResizing();

            updateViewComponents();
        }

        const Vec2& Instance::getPosition() const
        {
            return m_position;
        }

        void Instance::setPosition(const Vec2& inValue)
        {
            setPosition(inValue.x, inValue.y);
        }

        void Instance::setPosition(float inX, float inY)
        {
            if (
                std::fabs(m_position.x - inX) < FLT_EPSILON &&
                std::fabs(m_position.y - inY) < FLT_EPSILON
            )
            {
                return;
            }

            m_position.x = inX;
            m_position.y = inY;

            onRepositioning();
        }

        bool Instance::hasLayer(Layer::Instance* inLayer) const
        {
            if (!inLayer)
            {
                return false;
            }

            return hasLayer(inLayer->getId());
        }

        bool Instance::hasLayer(const String& inId) const
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

            inLayer->init();
        }

        void Instance::pushLayer(
            Layer::Instance* inLayer,
            Layer::PushStrategy inPushStrategy,
            const String& inId
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

            inLayer->init();
        }

        void Instance::pushLayerBefore(const String& inId, Layer::Instance* inLayer)
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

            inLayer->init();
        }

        void Instance::pushLayerAfter(const String& inId, Layer::Instance* inLayer)
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

            m_layers.insert(foundLayer + 1, inLayer);

            inLayer->init();
        }

        void Instance::setupLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                layer->setup();
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
                layer = nullptr;
            }

            m_layers.clear();
        }

        void Instance::loadEvents()
        {
            Application::getWindow()->watchEvent(
                [this](Window::Event inEvent)
                {
                    onEvent(inEvent);

                    for (Layer::Instance* layer : m_layers)
                    {
                        if (!layer)
                        {
                            continue;
                        }

                        layer->handle(inEvent);
                    }
                }
            );

            Application::watchScene(
                [this](Scene* inLevel) {
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
                                    return inA->getModel().compare(inB->getModel()) > 0;
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
                camera->setViewport(m_size);
            }

            for (CLight* light : m_lights)
            {
                light->setViewport(m_size);
            }
        }
    }
}