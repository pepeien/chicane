#include "Chicane/Renderer/Instance.hpp"

#if defined(CHICANE_OPENGL)
//#include "Chicane/Renderer/Backend/OpenGL.hpp"
#endif

#if defined(CHICANE_VULKAN)
//#include "Chicane/Renderer/Backend/Vulkan.hpp"
#endif

namespace Chicane
{
    namespace Renderer
    {
        Instance::Instance()
            : m_window(nullptr),
              m_draws2D({}),
              m_vertices2D({}),
              m_indices2D({}),
              m_draws3D({}),
              m_vertices3D({}),
              m_indices3D({}),
              m_camera({}),
              m_lights({}),
              m_size(Vec2::Zero),
              m_sizeOberservable({}),
              m_position(Vec2::Zero),
              m_positionOberservable({})
        {}

        Instance::~Instance()
        {
            destroy();
        }

        bool Instance::canRender() const
        {
            return !areDrawsEmpty();
        }

        void Instance::init(Window* inWindow)
        {
            if (!inWindow)
            {
                return;
            }

            setSize(Vec2::Zero);
            setPosition(Vec2::Zero);
            setWindow(inWindow);
        }

        void Instance::render()
        {
            if (!canRender())
            {
                reset();

                return;
            }

            reset();
        }

        void Instance::destroy()
        {
            resetViews();
            resetDraws();
        }

        void Instance::useCamera(const View& inData)
        {
            m_camera = inData;
        }

        void Instance::addLight(const View& inData)
        {
            m_lights.push_back(inData);
        }

        void Instance::draw(const DrawData2D& inData)
        {
            Draw2D draw      = {};
            draw.vertexStart = m_vertices2D.size() - 1;
            draw.vertexCount = inData.vertices.size();
            draw.indexStart  = m_indices2D.size() - 1;
            draw.indexCount  = inData.indices.size();
            draw.position    = inData.position;
            draw.size        = inData.size;
            m_draws2D.push_back(draw);

            m_vertices2D.insert(m_vertices2D.end(), inData.vertices.begin(), inData.vertices.end());
            m_indices2D.insert(m_indices2D.end(), inData.indices.begin(), inData.indices.end());
        }

        void Instance::draw(const DrawData3D& inData)
        {
            Draw3D draw      = {};
            draw.vertexStart = m_vertices3D.size() - 1;
            draw.vertexCount = inData.vertices.size();
            draw.indexStart  = m_indices3D.size() - 1;
            draw.indexCount  = inData.indices.size();
            draw.model       = inData.model;
            m_draws3D.push_back(draw);

            m_vertices3D.insert(m_vertices3D.end(), inData.vertices.begin(), inData.vertices.end());
            m_indices3D.insert(m_indices3D.end(), inData.indices.begin(), inData.indices.end());
        }

        const Vec2& Instance::getSize() const
        {
            return m_size;
        }

        void Instance::setSize(const Vec2& inValue)
        {
            setSize(inValue.x, inValue.y);
        }

        void Instance::setSize(float inWidth, float inHeight)
        {
            if (std::fabs(m_size.x - inWidth) < FLT_EPSILON &&
                std::fabs(m_size.y - inHeight) < FLT_EPSILON)
            {
                return;
            }

            m_size.x = inWidth;
            m_size.y = inHeight;

            m_sizeOberservable.next(m_size);
        }

        Instance::SizeSubscription Instance::watchSize(
            SizeObservable::NextCallback     inNext,
            SizeObservable::ErrorCallback    inError,
            SizeObservable::CompleteCallback inComplete
        )
        {
            return m_sizeOberservable.subscribe(inNext, inError, inComplete).next(m_size);
        }

        const Vec2& Instance::getPosition() const
        {
            return m_position;
        }

        void Instance::setPosition(const Vec2& inValue)
        {
            setPosition(inValue.x, inValue.y);
        }

        void Instance::setPosition(float inX, float inY)
        {
            if (std::fabs(m_position.x - inX) < FLT_EPSILON &&
                std::fabs(m_position.y - inY) < FLT_EPSILON)
            {
                return;
            }

            m_position.x = inX;
            m_position.y = inY;

            m_positionOberservable.next(m_position);
        }

        Instance::PositionSubscription Instance::watchPosition(
            PositionObservable::NextCallback     inNext,
            PositionObservable::ErrorCallback    inError,
            PositionObservable::CompleteCallback inComplete
        )
        {
            return m_positionOberservable.subscribe(inNext, inError, inComplete).next(m_position);
        }

        Window* Instance::getWindow() const
        {
            return m_window;
        }

        void Instance::setWindow(Window* inWindow)
        {
            if (inWindow == m_window)
            {
                return;
            }

            m_window = inWindow;
        }

        void Instance::handle(WindowEvent inEvent)
        {}

        void Instance::reset()
        {
            resetViews();
            resetDraws();
        }

        void Instance::resetViews()
        {
            resetCamera();
            resetLights();
        }

        void Instance::resetCamera()
        {
            m_camera = {};
        }

        void Instance::resetLights()
        {
            m_lights.clear();
        }

        bool Instance::areDrawsEmpty() const
        {
            return areDraws2DEmpty() && areDraws3DEmpty();
        }

        void Instance::resetDraws()
        {
            resetDraws2D();
            resetDraws3D();
        }

        bool Instance::areDraws2DEmpty() const
        {
            return m_vertices2D.empty() || m_indices2D.empty() || m_draws2D.empty();
        }

        void Instance::resetDraws2D()
        {
            m_vertices2D.clear();
            m_indices2D.clear();
            m_draws2D.clear();
        }

        bool Instance::areDraws3DEmpty() const
        {
            return m_vertices3D.empty() || m_indices3D.empty() || m_draws3D.empty();
        }

        void Instance::resetDraws3D()
        {
            m_vertices3D.clear();
            m_indices3D.clear();
            m_draws3D.clear();
        }
    }
}