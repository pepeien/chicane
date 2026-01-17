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
            : m_size(Vec2::Zero),
              m_sizeOberservable({}),
              m_position(Vec2::Zero),
              m_positionOberservable({}),
              m_window(nullptr)
        {}

        Instance::~Instance()
        {
            destroy();
        }

        bool Instance::canRender() const
        {
            return true;
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
                return;
            }
        }

        void Instance::destroy()
        {}

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
            if (std::fabs(m_size.x - inWidth) < FLT_EPSILON && std::fabs(m_size.y - inHeight) < FLT_EPSILON)
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
            if (std::fabs(m_position.x - inX) < FLT_EPSILON && std::fabs(m_position.y - inY) < FLT_EPSILON)
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
    }
}