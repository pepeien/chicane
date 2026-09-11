#pragma once

#include <cstddef>
#include <vector>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER DebugResource
        {
        public:
            DebugResource();

        public:
            DebugMode getModes() const;
            void enable(DebugMode inMode);
            void disable(DebugMode inMode);
            void toggle(DebugMode inMode);
            bool has(DebugMode inMode) const;

            void pushTraceOverlay(const Vertex::List& inVertices);
            void clearTraces();
            bool hasTraces() const;
            Vertex::List getTraceVertices() const;

        private:
            DebugMode                 m_mode;
            std::vector<Vertex::List> m_traces;
        };
    }
}
