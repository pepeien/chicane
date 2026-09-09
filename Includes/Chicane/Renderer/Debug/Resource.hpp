#pragma once

#include <cstddef>
#include <vector>

#include "Chicane/Core/Math/Bounds/3D.hpp"
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

            void setBoundsOverlay(const Vertex::List& inVertices);
            void clearBoundsOverlay();
            void appendBoundsOverlay(const Bounds3D& inBounds);

            void setCollidersOverlay(const Vertex::List& inVertices);
            void clearCollidersOverlay();
            void appendCollidersOverlay(const Vertex::List& inVertices);

            void pushTraceOverlay(const Vertex::List& inVertices);
            void clearTraces();

            bool hasOverlay() const;
            Vertex::List getOverlayVertices() const;

        private:
            DebugMode                 m_mode;
            Vertex::List              m_bounds;
            Vertex::List              m_colliders;
            std::vector<Vertex::List> m_traces;
        };
    }
}
