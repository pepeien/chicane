#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Debug/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        DebugResource::DebugResource()
            : m_mode(DebugMode::None),
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

        bool DebugResource::hasTraces() const
        {
            if (!has(DebugMode::Traces))
            {
                return false;
            }

            for (const Vertex::List& overlay : m_traces)
            {
                if (!overlay.empty())
                {
                    return true;
                }
            }

            return false;
        }

        Vertex::List DebugResource::getTraceVertices() const
        {
            Vertex::List result;

            if (!has(DebugMode::Traces))
            {
                return result;
            }

            for (const Vertex::List& overlay : m_traces)
            {
                result.insert(result.end(), overlay.begin(), overlay.end());
            }

            return result;
        }
    }
}
