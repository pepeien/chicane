#pragma once

#include <vector>
#include <map>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"
#include "Chicane/Renderer/Draw/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawResource
        {
        public:
            using Map = std::map<DrawType, DrawResource>;

        public:
            inline bool isEmpty() const { return m_draws.empty() || m_vertices.empty() || m_indices.empty(); }

            inline const Draw::List& getDraws() const { return m_draws; }

            inline const Vertex::List& getVertices() const { return m_vertices; }

            inline const Vertex::Indices& getIndices() const { return m_indices; }

            inline Draw::Id find(const DrawData& inData)
            {
                for (Draw& draw : m_draws)
                {
                    if (draw.vertexCount != inData.vertices.size() ||
                        draw.indexCount != inData.indices.size())
                    {
                        continue;
                    }

                    bool bIsEquals = true;

                    for (std::uint32_t index = 0; index < draw.indexCount; index++)
                    {
                        const Vertex& incoming = inData.vertices.at(inData.indices.at(index));
                        const Vertex& stored =
                            m_vertices.at(draw.vertexStart + m_indices.at(draw.indexStart + index));

                        if (incoming == stored)
                        {
                            continue;
                        }

                        bIsEquals = false;

                        break;
                    }

                    if (!bIsEquals)
                    {
                        continue;
                    }

                    return draw.id;
                }

                return Draw::UnknownId;
            }

            inline Draw::Id find(const String& inReference)
            {
                for (const Draw& draw : m_draws)
                {
                    if (!draw.reference.equals(inReference))
                    {
                        continue;
                    }

                    return draw.id;
                }

                return Draw::UnknownId;
            }

            inline Draw::Id add(const DrawData& inData)
            {
                Draw::Id drawId = find(inData);

                if (drawId > Draw::UnknownId)
                {
                    return drawId;
                }

                Draw draw        = {};
                draw.id          = m_draws.size();
                draw.reference   = inData.reference;
                draw.vertexStart = m_vertices.empty() ? 0U : m_vertices.size();
                draw.vertexCount = inData.vertices.size();
                draw.indexStart  = m_indices.empty() ? 0U : m_indices.size();
                draw.indexCount  = inData.indices.size();
                m_draws.push_back(draw);

                m_vertices.reserve(m_vertices.size() + inData.vertices.size());
                m_vertices.insert(m_vertices.end(), inData.vertices.begin(), inData.vertices.end());

                m_indices.reserve(m_indices.size() + inData.indices.size());
                m_indices.insert(m_indices.end(), inData.indices.begin(), inData.indices.end());

                return draw.id;
            }

            void clear()
            {
                m_draws.clear();
                m_vertices.clear();
                m_indices.clear();
            }

        private:
            Draw::List      m_draws    = {};
            Vertex::List    m_vertices = {};
            Vertex::Indices m_indices  = {};
        };
    }
}