#pragma once

#include <map>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPolyResource : public DrawResource<Vertex::Index, Vertex>
        {
        public:
            using Map   = std::unordered_map<DrawPolyType, DrawPolyResource>;
            using Draws = std::unordered_map<String, DrawPoly>;

        public:
            bool isEmpty() const;

            const Draws& getDraws() const;
            const Vertex::List& getVertices() const;
            const Vertex::Indices& getIndices() const;

            Draw::Id findId(const DrawPolyData& inData);
            Draw::Id findId(const Draw::Reference& inReference);

            const DrawPoly& getDraw(const Draw::Reference& inReference);
            const DrawPoly& getDraw(Draw::Id inId);

            Draw::Id add(const DrawPolyData& inData);

            void reset();

        private:
            String generateInternalReference(DrawPolyMode inMode, Draw::Id inId) const;

        private:
            Draws           m_draws    = {};
            Vertex::List    m_vertices = {};
            Vertex::Indices m_indices  = {};
        };
    }
}