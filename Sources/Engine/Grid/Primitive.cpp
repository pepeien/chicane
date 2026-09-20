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

            m_sharedVertices = nullptr;
            m_sharedIndices  = nullptr;
            m_sharedOutline  = nullptr;
        }

        void Primitive::seal()
        {
            m_sharedVertices = std::make_shared<const Vertex::List>(vertices);
            m_sharedIndices  = std::make_shared<const Vertex::Indices>(indices);
            m_sharedOutline  = outline.empty() ? nullptr : std::make_shared<const std::vector<Vec2>>(outline);
        }

        const std::shared_ptr<const Vertex::List>& Primitive::getSharedVertices() const
        {
            return m_sharedVertices;
        }

        const std::shared_ptr<const Vertex::Indices>& Primitive::getSharedIndices() const
        {
            return m_sharedIndices;
        }

        const std::shared_ptr<const std::vector<Vec2>>& Primitive::getSharedOutline() const
        {
            return m_sharedOutline;
        }
    }
}