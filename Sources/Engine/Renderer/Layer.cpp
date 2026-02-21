#include "Chicane/Renderer/Layer.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Layer::Layer(const Id& inId)
            : m_id(inId),
              m_viewport({}),
              m_backend(nullptr)
        {}

        Layer::Layer()
            : m_id("Undefined"),
              m_viewport({}),
              m_backend(nullptr)
        {}

        const String& Layer::getId() const
        {
            return m_id;
        }

        const ViewportSettings& Layer::getViewport() const
        {
            return m_viewport;
        }

        void Layer::setViewport(const ViewportSettings& inValue)
        {
            m_viewport = inValue;
        }

        void Layer::setBackend(Backend* inBackend)
        {
            m_backend = inBackend;
        }
    }
}