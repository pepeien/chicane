#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Debug/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        DebugResource::DebugResource()
            : m_mode(DebugMode::None),
              m_bounds({}),
              m_colliders({}),
              m_traces({})
        {}

        DebugMode DebugResource::getModes() const
        {
            return m_mode;
        }

        void DebugResource::enable(DebugMode inMode)
        {
            m_mode |= inMode;
        }

        void DebugResource::disable(DebugMode inMode)
        {
            m_mode &= ~inMode;
        }

        void DebugResource::toggle(DebugMode inMode)
        {
            if (has(inMode))
            {
                disable(inMode);

                return;
            }

            enable(inMode);
        }

        bool DebugResource::has(DebugMode inMode) const
        {
            return (m_mode & inMode) == inMode;
        }

        void DebugResource::setBoundsOverlay(const Vertex::List& inVertices)
        {
            m_bounds = inVertices;
        }

        void DebugResource::clearBoundsOverlay()
        {
            m_bounds.clear();
        }

        void DebugResource::appendBoundsOverlay(const Bounds3D& inBounds)
        {
            Debug::appendBounds(m_bounds, inBounds, Debug::BOUNDS_COLOR);
        }

        void DebugResource::setCollidersOverlay(const Vertex::List& inVertices)
        {
            m_colliders = inVertices;
        }

        void DebugResource::clearCollidersOverlay()
        {
            m_colliders.clear();
        }

        void DebugResource::appendCollidersOverlay(const Vertex::List& inVertices)
        {
            m_colliders.insert(m_colliders.end(), inVertices.begin(), inVertices.end());
        }

        void DebugResource::pushTraceOverlay(const Vertex::List& inVertices)
        {
            if (inVertices.empty())
            {
                return;
            }

            if (m_traces.size() >= Debug::TRACE_CAPACITY)
            {
                m_traces.erase(m_traces.begin());
            }

            m_traces.push_back(inVertices);
        }

        void DebugResource::clearTraces()
        {
            m_traces.clear();
        }

        bool DebugResource::hasOverlay() const
        {
            if (has(DebugMode::Bounds) && !m_bounds.empty())
            {
                return true;
            }

            if (has(DebugMode::Colliders) && !m_colliders.empty())
            {
                return true;
            }

            if (has(DebugMode::Traces))
            {
                for (const Vertex::List& overlay : m_traces)
                {
                    if (!overlay.empty())
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        Vertex::List DebugResource::getOverlayVertices() const
        {
            Vertex::List result;

            if (has(DebugMode::Bounds))
            {
                result = m_bounds;
            }

            if (has(DebugMode::Colliders))
            {
                result.insert(result.end(), m_colliders.begin(), m_colliders.end());
            }

            if (has(DebugMode::Traces))
            {
                for (const Vertex::List& overlay : m_traces)
                {
                    result.insert(result.end(), overlay.begin(), overlay.end());
                }
            }

            return result;
        }
    }
}
