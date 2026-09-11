#pragma once

#include <cstddef>
#include <map>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Glyph/Data.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPolyResource : public DrawResource<Vertex::Index, Vertex>
        {
        public:
            using Map           = std::unordered_map<DrawPolyType, DrawPolyResource>;
            using Draws         = std::unordered_map<String, DrawPoly>;
            using Glyphs        = std::unordered_map<String, Draw::Id>;
            using GlyphOutlines = std::vector<float>;

        public:
            static constexpr inline const Draw::Id GlyphHeaderSize = 6;
            static constexpr inline const Draw::Id GlyphCurveSize  = 6;

        public:
            bool isEmpty() const;

            const Draws& getDraws() const;
            const Vertex::List& getVertices() const;
            const Vertex::Indices& getIndices() const;
            const GlyphOutlines& getGlyphOutlines() const;

            Draw::Id findId(const DrawPolyData& inData);
            Draw::Id findId(const Draw::Reference& inReference);

            const DrawPoly& getDraw(const Draw::Reference& inReference);
            const DrawPoly& getDraw(Draw::Id inId);

            Draw::Id add(DrawPolyType inType, const DrawPolyData& inData);

            std::size_t getStableVertexCount() const { return m_stableVertexCount; }
            std::size_t getStableIndexCount() const { return m_stableIndexCount; }

            // Glyphs are addressed by their offset into the outline buffer, which keeps a header and its curves
            // contiguous and lets new glyphs be appended without relocating the existing ones
            Draw::Id findGlyph(const Draw::Reference& inReference) const;
            Draw::Id addGlyph(const DrawGlyphData& inData);

            void reset();

        private:
            String generateInternalReference(DrawPolyMode inMode, Draw::Id inId) const;

        private:
            Draws           m_draws    = {};
            Vertex::List    m_vertices = {};
            Vertex::Indices m_indices  = {};

            std::size_t     m_stableVertexCount = 0;
            std::size_t     m_stableIndexCount  = 0;

            Glyphs          m_glyphs        = {};
            GlyphOutlines   m_glyphOutlines = {};
        };
    }
}