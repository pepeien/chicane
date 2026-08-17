#pragma once

#include <map>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Glyph/Data.hpp"
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
            using Map      = std::unordered_map<DrawPolyType, DrawPolyResource>;
            using Draws    = std::unordered_map<String, DrawPoly>;
            using Glyphs   = std::unordered_map<String, Draw::Id>;
            using GlyphOutlines = std::vector<float>;

        public:
            // Floats per glyph header, followed by six floats per curve.
            // Layout: bounds min xy, bounds max xy, curve count, padding
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

            Draw::Id add(const DrawPolyData& inData);

            // Glyphs are addressed by their offset into the outline buffer, which keeps a header and its curves
            // contiguous and lets new glyphs be appended without relocating the existing ones
            Draw::Id findGlyph(const Draw::Reference& inReference) const;
            Draw::Id addGlyph(const DrawGlyphData& inData);

            void reset();

        private:
            String generateInternalReference(DrawPolyMode inMode, Draw::Id inId) const;

        private:
            Draws           m_draws         = {};
            Vertex::List    m_vertices      = {};
            Vertex::Indices m_indices       = {};

            Glyphs          m_glyphs        = {};
            GlyphOutlines   m_glyphOutlines = {};
        };
    }
}