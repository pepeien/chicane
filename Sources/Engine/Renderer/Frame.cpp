#include "Chicane/Renderer/Frame.hpp"

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
            Draw2D draw      = {};
            draw.vertexStart = m_vertices2D.empty() ? 0U : m_vertices2D.size() - 1;
            draw.vertexCount = inData.vertices.size();
            draw.indexStart  = m_indices2D.empty() ? 0U : m_indices2D.size() - 1;
            draw.indexCount  = inData.indices.size();
            draw.position    = inData.position;
            draw.size        = inData.size;
            m_draws2D.push_back(draw);

            m_vertices2D.insert(m_vertices2D.end(), inData.vertices.begin(), inData.vertices.end());
            m_indices2D.insert(m_indices2D.end(), inData.indices.begin(), inData.indices.end());
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
            Draw3D draw      = {};
            draw.vertexStart = m_vertices3D.empty() ? 0U : m_vertices3D.size() - 1;
            draw.vertexCount = inData.vertices.size();
            draw.indexStart  = m_indices3D.empty() ? 0U : m_indices3D.size() - 1;
            draw.indexCount  = inData.indices.size();
            draw.model       = inData.model;
            m_draws3D.push_back(draw);

            m_vertices3D.insert(m_vertices3D.end(), inData.vertices.begin(), inData.vertices.end());
            m_indices3D.insert(m_indices3D.end(), inData.indices.begin(), inData.indices.end());
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

        void Frame::resetDraw2D()
        {
            m_draws2D.clear();
            m_vertices2D.clear();
            m_indices2D.clear();
        }

        void Frame::resetDraw3D()
        {
            m_draws3D.clear();
            m_vertices3D.clear();
            m_indices3D.clear();
        }
    }
}