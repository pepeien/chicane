#include "Chicane/Renderer/Draw/Poly/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        bool DrawPolyResource::isEmpty() const
        {
            return m_draws.empty() || m_vertices.empty() || m_indices.empty();
        }

        const DrawPolyResource::Draws& DrawPolyResource::getDraws() const
        {
            return m_draws;
        }

        const Vertex::List& DrawPolyResource::getVertices() const
        {
            return m_vertices;
        }

        const Vertex::Indices& DrawPolyResource::getIndices() const
        {
            return m_indices;
        }

        Draw::Id DrawPolyResource::findId(const DrawPolyData& inData)
        {
            if (!inData.reference.isEmpty())
            {
                return findId(inData.reference);
            }

            return findHash(
                inData.indices.data(),
                inData.indices.size() * sizeof(Vertex::Index),
                inData.vertices.data(),
                inData.vertices.size() * sizeof(Vertex)
            );
        }

        Draw::Id DrawPolyResource::findId(const Draw::Reference& inReference)
        {
            const auto& found = m_draws.find(inReference);

            if (found == m_draws.end())
            {
                return Draw::InvalidId;
            }

            return found->second.id;
        }

        const DrawPoly& DrawPolyResource::getDraw(const Draw::Reference& inReference)
        {
            if (findId(inReference) <= Draw::InvalidId)
            {
                return DrawPoly::empty();
            }

            return m_draws.at(inReference);
        }

        const DrawPoly& DrawPolyResource::getDraw(Draw::Id inId)
        {
            for (const auto& [reference, draw] : m_draws)
            {
                if (draw.id != inId)
                {
                    continue;
                }

                return draw;
            }

            return DrawPoly::empty();
        }

        Draw::Id DrawPolyResource::add(const DrawPolyData& inData)
        {
            const Draw::Id id = findId(inData);

            if (id > Draw::InvalidId)
            {
                return id;
            }

            DrawPoly draw;
            draw.id   = m_draws.size();
            draw.mode = inData.mode;
            draw.reference =
                inData.reference.isEmpty() ? generateInternalReference(draw.mode, draw.id) : inData.reference;
            draw.vertexStart = m_vertices.empty() ? 0U : m_vertices.size();
            draw.vertexCount = inData.vertices.size();
            draw.indexStart  = m_indices.empty() ? 0U : m_indices.size();
            draw.indexCount  = inData.indices.size();

            m_draws.emplace(draw.reference, draw);

            m_vertices.reserve(m_vertices.size() + inData.vertices.size());
            m_vertices.insert(m_vertices.end(), inData.vertices.begin(), inData.vertices.end());

            m_indices.reserve(m_indices.size() + inData.indices.size());
            m_indices.insert(m_indices.end(), inData.indices.begin(), inData.indices.end());

            addHash(
                inData.indices.data(),
                inData.indices.size() * sizeof(Vertex::Index),
                inData.vertices.data(),
                inData.vertices.size() * sizeof(Vertex),
                draw.id
            );

            return draw.id;
        }

        void DrawPolyResource::reset()
        {
            m_draws.clear();
            m_vertices.clear();
            m_indices.clear();
            clearHashes();
        }

        String DrawPolyResource::generateInternalReference(DrawPolyMode inMode, Draw::Id inId) const
        {
            String result = "__internal";
            result.append('_');
            result.append(std::to_string(static_cast<int>(inMode)));
            result.append('_');
            result.append(std::to_string(static_cast<int>(inId)));

            return result;
        }
    }
}