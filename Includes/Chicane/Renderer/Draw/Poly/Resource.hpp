#pragma once

#include <vector>
#include <map>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPolyResource
        {
        public:
            using Map = std::map<DrawPolyType, DrawPolyResource>;

        public:
            bool isEmpty() const;

            const DrawPoly::List& getDraws() const;
            const Vertex::List& getVertices() const;
            const Vertex::Indices& getIndices() const;

            Draw::Id findId(const DrawPolyData& inData);
            Draw::Id findId(const Draw::Reference& inReference);

            const DrawPoly& getDraw(const Draw::Reference& inReference);
            const DrawPoly& getDraw(Draw::Id inId);

            Draw::Id add(const DrawPolyData& inData);

            void reset();

        private:
            DrawPoly::List  m_draws    = {};
            Vertex::List    m_vertices = {};
            Vertex::Indices m_indices  = {};
        };
    }
}