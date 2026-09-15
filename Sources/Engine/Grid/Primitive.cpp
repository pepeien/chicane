#include "Chicane/Grid/Primitive.hpp"

namespace Chicane
{
    namespace Grid
    {
        bool Primitive::isEmpty() const
        {
            return (vertices.empty() || indices.empty()) && reference.isEmpty();
        }

        bool Primitive::hasGlyph() const
        {
            return !glyph.isEmpty() && !outline.empty();
        }

        void Primitive::clear()
        {
            reference  = String::empty();
            glyph      = String::empty();
            dilation   = 0.0f;
            outlineMin = Vec2::Zero();
            outlineMax = Vec2::Zero();

            outline.clear();
            vertices.clear();
            indices.clear();
        }
    }
}