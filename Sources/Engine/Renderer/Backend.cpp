#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Backend::Backend(const Instance* inRenderer)
            : m_renderer(inRenderer),
              m_layers({})
        {}

        Backend::~Backend()
        {
            for (Layer* layer : m_layers)
            {
                delete layer;
                layer = nullptr;
            }

            m_layers.clear();
        }

        void Backend::init()
        {
            onInit();

            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->init();
            }
        }

        void Backend::setup(const Frame& inFrame)
        {
            onSetup(inFrame);

            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->setup(inFrame);
            }
        }

        void Backend::render(const Frame& inFrame)
        {
            onRender(inFrame);

            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->render(inFrame);
            }
        }

        void Backend::handle(const WindowEvent& inEvent)
        {
            onHandle(inEvent);

            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->handle(inEvent);
            }
        }

        void Backend::cleanup()
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->cleanup();
            }

            onCleanup();
        }
    }
}