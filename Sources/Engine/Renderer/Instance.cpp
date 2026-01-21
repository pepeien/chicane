#include "Chicane/Renderer/Instance.hpp"

#if defined(CHICANE_OPENGL)
    #include "Chicane/Renderer/Backend/OpenGL.hpp"
#endif

#if defined(CHICANE_VULKAN)
    #include "Chicane/Renderer/Backend/Vulkan.hpp"
#endif

namespace Chicane
{
    namespace Renderer
    {
        Instance::Instance()
            : m_window(nullptr),
              m_frames({}),
              m_currentFrame(0U),
              m_viewport({}),
              m_backend(nullptr)
        {}

        Instance::~Instance()
        {
            destroy();
        }

        void Instance::init(Window* inWindow, WindowBackend inBackend)
        {
            if (!inWindow)
            {
                return;
            }

            setViewport(Vec2::Zero, Vec2::Zero);
            setWindow(inWindow);
            setBackend(inBackend);
        }

        void Instance::render()
        {
            if (!hasBackend())
            {
                return;
            }

            Frame& currentFrame = getCurrentFrame();
            currentFrame.setup(m_drawResources);

            m_backend->setup(currentFrame);
            m_backend->render(currentFrame);
            m_backend->cleanup();

            currentFrame.reset();

            m_drawResources[DrawType::e2D].clear();

            m_currentFrame = (m_currentFrame + 1) % FRAME_COUNT;
        }

        void Instance::destroy()
        {
            m_backend.reset();
        }

        Frame& Instance::getCurrentFrame()
        {
            return m_frames.at(m_currentFrame);
        }

        void Instance::useCamera(const View& inData)
        {
            getCurrentFrame().useCamera(inData);
        }

        void Instance::addLight(const View::List& inData)
        {
            getCurrentFrame().addLights(inData);
        }

        void Instance::addLight(const View& inData)
        {
            getCurrentFrame().addLight(inData);
        }

        const Draw::List& Instance::getDraws(DrawType inType) const
        {
            return getResource(inType).getDraws();
        }

        const Vertex::List& Instance::getVertices(DrawType inType) const
        {
            return getResource(inType).getVertices();
        }

        const Vertex::Indices& Instance::getIndices(DrawType inType) const
        {
            return getResource(inType).getIndices();
        }

        Draw::Id Instance::load(DrawType inType, const DrawData& inData)
        {
            return getResource(inType).add(inData);
        }

        const Viewport& Instance::getViewport() const
        {
            return m_viewport;
        }

        void Instance::setViewport(const Viewport& inValue)
        {
            setViewport(inValue.position, inValue.size);
        }

        void Instance::setViewport(const Vec2& inPosition, const Vec2& inSize)
        {
            setViewportPosition(inPosition);
            setViewportSize(inSize);
        }

        void Instance::setViewportPosition(const Vec2& inPosition)
        {
            setViewportPosition(inPosition.x, inPosition.y);
        }

        void Instance::setViewportPosition(float inX, float inY)
        {
            if (std::fabs(m_viewport.position.x - inX) < FLT_EPSILON &&
                std::fabs(m_viewport.position.y - inY) < FLT_EPSILON)
            {
                return;
            }

            m_viewport.position.x = inX;
            m_viewport.position.y = inY;
        }

        void Instance::setViewportSize(const Vec2& inSize)
        {
            setViewportSize(inSize.x, inSize.y);
        }

        void Instance::setViewportSize(float inWidth, float inHeight)
        {
            if (std::fabs(m_viewport.size.x - inWidth) < FLT_EPSILON &&
                std::fabs(m_viewport.size.y - inHeight) < FLT_EPSILON)
            {
                return;
            }

            m_viewport.size.x = inWidth;
            m_viewport.size.y = inHeight;
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
            m_window->watchSize([&](const Vec<2, int>& inSize) { setViewportSize(inSize.x, inSize.y); });
            m_window->watchEvent([&](WindowEvent inEvent) { handle(inEvent); });
        }

        void Instance::handle(const WindowEvent& inEvent)
        {
            if (!hasBackend())
            {
                return;
            }

            m_backend->handle(inEvent);
        }

        bool Instance::hasBackend() const
        {
            return m_backend && m_backend.get() != nullptr;
        }

        void Instance::setBackend(WindowBackend inType)
        {
            if (hasBackend())
            {
                m_backend.reset();
            }

            switch (inType)
            {
#if defined(CHICANE_OPENGL)
            case WindowBackend::OpenGL:
                m_backend = std::make_unique<OpenGLBackend>(this);

                break;
#endif

#if defined(CHICANE_VULKAN)
            case WindowBackend::Vulkan:
                //m_backend = std::make_unique<VulkanBackend>();

                //break;
#endif

            default:
                m_backend = std::make_unique<Backend>(this);

                break;
            }

            m_backend->init();
        }

        DrawResource& Instance::getResource(DrawType inType)
        {
            return m_drawResources[inType];
        }

        const DrawResource& Instance::getResource(DrawType inType) const
        {
            return m_drawResources.at(inType);
        }
    }
}