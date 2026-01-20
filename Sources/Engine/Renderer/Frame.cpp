#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void Frame::reset()
        {
            resetCamera();
            resetLights();
            resetDraw2D();
            resetDraw3D();
        }

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

        const DrawBundle<Draw2DInstance>::DrawList& Frame::getDraws2D() const
        {
            return m_draws2D.getDraws();
        }

        const Vertex::List& Frame::getVertices2D() const
        {
            return m_draws2D.getVertices();
        }

        const Vertex::Indices& Frame::getIndices2D() const
        {
            return m_draws2D.getIndices();
        }

        const DrawBundle<Draw2DInstance>::DrawInstances Frame::getInstances2D() const
        {
            return m_draws2D.getInstances();
        }

        void Frame::addDraw(const DrawData2D& inData)
        {
            Draw2DInstance instance;
            instance.screen   = inData.screen;
            instance.size     = inData.size;
            instance.position = inData.position;

            m_draws2D.add(inData, instance);
        }

        const DrawBundle<Draw3DInstance>::DrawList& Frame::getDraws3D() const
        {
            return m_draws3D.getDraws();
        }

        const Vertex::List& Frame::getVertices3D() const
        {
            return m_draws3D.getVertices();
        }

        const Vertex::Indices& Frame::getIndices3D() const
        {
            return m_draws3D.getIndices();
        }

        const DrawBundle<Draw3DInstance>::DrawInstances Frame::getInstances3D() const
        {
            return m_draws3D.getInstances();
        }

        void Frame::addDraw(const DrawData3D& inData)
        {
            Draw3DInstance instance;
            instance.model = inData.model;

            m_draws3D.add(inData, instance);
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
            m_draws2D.reset();
        }

        void Frame::resetDraw3D()
        {
            m_draws3D.reset();
        }
    }
}