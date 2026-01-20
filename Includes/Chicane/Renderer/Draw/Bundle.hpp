#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename I>
        struct CHICANE_RENDERER DrawBundle
        {
        public:
            using List          = std::vector<DrawBundle>;
            using DrawList      = std::vector<Draw<I>>;
            using DrawInstances = std::vector<I>;

        public:
            inline const DrawList& getDraws() const { return m_draws; }

            inline const Vertex::List& getVertices() const { return m_vertices; }

            inline const Vertex::Indices& getIndices() const { return m_indices; }

            inline DrawInstances getInstances() const
            {
                DrawInstances result = {};

                for (const Draw<I>& draw : m_draws)
                {
                    result.insert(result.end(), draw.instances.begin(), draw.instances.end());
                }

                return result;
            }

            inline Draw<I>* find(const DrawData& inData)
            {
                for (Draw<I>& draw : m_draws)
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

                    return &draw;
                }

                return nullptr;
            }

            inline void add(const DrawData& inData, const I& inInstance)
            {
                if (Draw<I>* draw = find(inData))
                {
                    draw->instances.push_back(inInstance);

                    return;
                }

                Draw<I> draw     = {};
                draw.vertexStart = m_vertices.empty() ? 0U : m_vertices.size();
                draw.vertexCount = inData.vertices.size();
                draw.indexStart  = m_indices.empty() ? 0U : m_indices.size();
                draw.indexCount  = inData.indices.size();
                draw.instances.push_back(inInstance);
                m_draws.push_back(draw);

                m_vertices.reserve(m_vertices.size() + inData.vertices.size());
                m_vertices.insert(m_vertices.end(), inData.vertices.begin(), inData.vertices.end());

                m_indices.reserve(m_indices.size() + inData.indices.size());
                m_indices.insert(m_indices.end(), inData.indices.begin(), inData.indices.end());
            }

            void reset()
            {
                m_draws.clear();
                m_vertices.clear();
                m_indices.clear();
            }

        private:
            DrawList        m_draws    = {};
            Vertex::List    m_vertices = {};
            Vertex::Indices m_indices  = {};
        };
    }
}