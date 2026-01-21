#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Layer::Layer(const String& inId)
            : m_id(inId),
              m_status(LayerStatus::Offline),
              m_children({})
        {}

        Layer::Layer()
            : m_id("Undefined"),
              m_status(LayerStatus::Offline),
              m_children({})
        {}

        Layer::~Layer()
        {
            for (Layer* child : m_children)
            {
                delete child;
                child = nullptr;
            }

            m_children.clear();
        }

        void Layer::init()
        {
            if (!is(LayerStatus::Offline))
            {
                return;
            }

            if (!onInit())
            {
                return;
            }

            setStatus(LayerStatus::Initialized);

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->init();
            }
        }

        void Layer::destroy()
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

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->destroy();
            }
        }

        void Layer::rebuild()
        {
            if (!is(LayerStatus::Offline))
            {
                return;
            }

            if (!onRebuild())
            {
                return;
            }

            setStatus(LayerStatus::Initialized);

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->rebuild();
            }
        }

        void Layer::setup(const Frame& inFrame, const DrawResource::Map& inResources)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            if (!onSetup(inFrame, inResources))
            {
                return;
            }

            setStatus(LayerStatus::Running);

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->setup(inFrame, inResources);
            }
        }

        void Layer::render(const Frame& inFrame, const DrawResource::Map& inResources)
        {
            if (!is(LayerStatus::Running))
            {
                return;
            }

            onRender(inFrame, inResources);

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->render(inFrame, inResources);
            }
        }

        void Layer::cleanup()
        {
            if (!is(LayerStatus::Running))
            {
                return;
            }

            onCleanup();

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->cleanup();
            }
        }

        void Layer::handle(const WindowEvent& inEvent)
        {
            onEvent(inEvent);

            for (Layer* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->onEvent(inEvent);
            }
        }

        bool Layer::is(LayerStatus inStatus) const
        {
            return m_status == inStatus;
        }

        const String& Layer::getId() const
        {
            return m_id;
        }

        void Layer::setStatus(LayerStatus inStatus)
        {
            m_status = inStatus;
        }
    }
}
