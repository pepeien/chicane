#include "Chicane/Renderer/Layer.hpp"

#include "Chicane/Renderer/Backend.hpp"

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

        Viewport Layer::getViewport() const
        {
            const Vec<2, std::uint32_t>& resolution = m_backend->getResolution();

            Viewport result = m_viewport;
            result.size.x   = result.size.x <= 0.0f ? resolution.x : result.size.x;
            result.size.y   = result.size.y <= 0.0f ? resolution.y : result.size.y;

            return result;
        }

        void Layer::setViewport(const Viewport& inValue)
        {
            m_viewport = inValue;
        }

        void Layer::setBackend(Backend* inBackend)
        {
            m_backend = inBackend;
        }
    }
}