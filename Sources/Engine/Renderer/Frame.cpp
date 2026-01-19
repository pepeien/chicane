#include "Chicane/Renderer/Frame.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Renderer
    {
        const View& Frame::getCamera() const
        {
            return m_camera;
        }

        void Frame::useCamera(const View& inData)
        {
            m_camera = std::move(inData);
        }

        const View::List& Frame::getLights() const
        {
            return m_lights;
        }

        void Frame::addLights(const View::List& inData)
        {
            m_lights.insert(m_lights.begin(), inData.begin(), inData.end());
        }

        void Frame::addLight(const View& inData)
        {
            m_lights.push_back(std::move(inData));
        }

        const Draw2D::List& Frame::getDraws2D() const
        {
            return m_draws2D;
        }

        const Vertex::List& Frame::getVertices2D() const
        {
            return m_vertices2D;
        }

        const Vertex::Indices& Frame::getIndices2D() const
        {
            return m_indices2D;
        }

        void Frame::addDraw(const DrawData2D& inData)
        {
            Draw::Id location = findDraw(inData);

            if (location < 0)
            {
                location = m_draws2D.size();

                Draw2D draw      = {};
                draw.id          = m_draws2D.empty() ? 0 : m_draws2D.size();
                draw.vertexStart = m_vertices2D.empty() ? 0U : m_vertices2D.size();
                draw.vertexCount = inData.vertices.size();
                draw.indexStart  = m_indices2D.empty() ? 0U : m_indices2D.size();
                draw.indexCount  = inData.indices.size();
                m_draws2D.push_back(draw);

                m_vertices2D.reserve(m_vertices2D.size() + inData.vertices.size());
                m_vertices2D.insert(m_vertices2D.end(), inData.vertices.begin(), inData.vertices.end());

                m_indices2D.reserve(m_indices2D.size() + inData.indices.size());
                m_indices2D.insert(m_indices2D.end(), inData.indices.begin(), inData.indices.end());
            }

            Draw2DInstance instance;
            instance.position = inData.position;
            instance.size     = inData.size;

            Draw2D& draw = m_draws2D.at(location);
            draw.instances.push_back(instance);
        }

        const Draw3D::List& Frame::getDraws3D() const
        {
            return m_draws3D;
        }

        const Vertex::List& Frame::getVertices3D() const
        {
            return m_vertices3D;
        }

        const Vertex::Indices& Frame::getIndices3D() const
        {
            return m_indices3D;
        }

        void Frame::addDraw(const DrawData3D& inData)
        {
            Draw::Id location = findDraw(inData);

            if (location <= Draw::UNKNOWN_ID)
            {
                location = m_draws3D.size();

                Draw3D draw      = {};
                draw.id          = m_draws3D.empty() ? 0 : m_draws3D.size();
                draw.vertexStart = m_vertices3D.empty() ? 0U : m_vertices3D.size();
                draw.vertexCount = inData.vertices.size();
                draw.indexStart  = m_indices3D.empty() ? 0U : m_indices3D.size();
                draw.indexCount  = inData.indices.size();
                m_draws3D.push_back(draw);

                m_vertices3D.reserve(m_vertices3D.size() + inData.vertices.size());
                m_vertices3D.insert(m_vertices3D.end(), inData.vertices.begin(), inData.vertices.end());

                m_indices3D.reserve(m_indices3D.size() + inData.indices.size());
                m_indices3D.insert(m_indices3D.end(), inData.indices.begin(), inData.indices.end());
            }

            Draw3DInstance instance;
            instance.model = inData.model;

            Draw3D& draw = m_draws3D.at(location);
            draw.instances.push_back(instance);
        }

        void Frame::reset()
        {
            resetCamera();
            resetLights();
            resetDraw2D();
            resetDraw3D();
        }

        void Frame::resetCamera()
        {
            m_camera = {};
        }

        void Frame::resetLights()
        {
            m_lights.clear();
        }

        Draw::Id Frame::findDraw(const DrawData2D& inData) const
        {
            for (const Draw2D& draw : m_draws2D)
            {
                if (draw.vertexCount != inData.vertices.size() || draw.indexCount != inData.indices.size())
                {
                    continue;
                }

                bool bIsSame = true;

                for (std::uint32_t index = 0; index < draw.indexCount; index++)
                {
                    const Vertex& incoming = inData.vertices.at(inData.indices.at(index));
                    const Vertex& stored =
                        m_vertices2D.at(draw.vertexStart + m_indices2D.at(draw.indexStart + index));

                    if (incoming == stored)
                    {
                        continue;
                    }

                    bIsSame = false;

                    break;
                }

                if (!bIsSame)
                {
                    continue;
                }

                return draw.id;
            }

            return Draw::UNKNOWN_ID;
        }

        void Frame::resetDraw2D()
        {
            m_draws2D.clear();
            m_vertices2D.clear();
            m_indices2D.clear();
        }

        Draw::Id Frame::findDraw(const DrawData3D& inData) const
        {
            for (const Draw3D& draw : m_draws3D)
            {
                if (draw.vertexCount != inData.vertices.size() || draw.indexCount != inData.indices.size())
                {
                    continue;
                }

                bool bIsSame = true;

                for (std::uint32_t index = 0; index < draw.indexCount; index++)
                {
                    const Vertex& incomingVertex = inData.vertices.at(inData.indices.at(index));
                    const Vertex& storedVertex =
                        m_vertices3D.at(draw.vertexStart + m_indices3D.at(draw.indexStart + index));

                    if (incomingVertex == storedVertex)
                    {
                        continue;
                    }

                    bIsSame = false;

                    break;
                }

                if (!bIsSame)
                {
                    continue;
                }

                return draw.id;
            }

            return Draw::UNKNOWN_ID;
        }

        void Frame::resetDraw3D()
        {
            m_draws3D.clear();
            m_vertices3D.clear();
            m_indices3D.clear();
        }
    }
}