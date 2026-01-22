#include "Chicane/Renderer/Instance.hpp"

#if defined(CHICANE_OPENGL)
    #include "Chicane/Renderer/Backend/OpenGL.hpp"
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
            currentFrame.setup(m_polyResources);

            m_backend->setup(currentFrame);
            m_backend->render(currentFrame);
            m_backend->cleanup();

            currentFrame.reset();

            getPolyResource(DrawPolyType::e2D).reset();

            m_currentFrame = (m_currentFrame + 1) % FRAME_COUNT;
        }

        void Instance::destroy()
        {
            m_backend.reset();
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

        Draw::Id Instance::loadPoly(DrawPolyType inType, const DrawPolyData& inData)
        {
            const Draw::Id id = getPolyResource(inType).add(inData);

            if (hasBackend())
            {
                m_backend->load(inType, getPolyResource(inType));
            }

            return id;
        }

        Draw::Id Instance::findTexture(const String& inReference)
        {
            const auto& found = std::find_if(
                m_textureResources.begin(),
                m_textureResources.end(),
                [inReference](const DrawTexture& inTexture) {
                    return inTexture.reference.equals(inReference);
                }
            );

            if (found == m_textureResources.end())
            {
                return Draw::UnknownId;
            }

            return found->id;
        }

        Draw::Id Instance::loadTexture(const DrawTextureData& inData)
        {
            const auto& found = std::find_if(
                m_textureResources.begin(),
                m_textureResources.end(),
                [inData](const DrawTexture& inTexture) {
                    return inTexture.reference.equals(inData.reference);
                }
            );

            if (found != m_textureResources.end())
            {
                return found->id;
            }

            DrawTexture texture;
            texture.id        = m_textureResources.size();
            texture.reference = inData.reference;
            texture.image     = inData.image;

            m_textureResources.push_back(texture);

            if (hasBackend())
            {
                m_backend->load(m_textureResources);
            }

            return texture.id;
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

            propagateResize();
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

            propagateResize();
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
            m_backend->resize(m_viewport);

            for (const auto& [type, resource] : m_polyResources)
            {
                m_backend->load(type, resource);
            }

            m_backend->load(m_textureResources);
        }

        Frame& Instance::getCurrentFrame()
        {
            return m_frames.at(m_currentFrame);
        }

        DrawPolyResource& Instance::getPolyResource(DrawPolyType inType)
        {
            return m_polyResources[inType];
        }

        void Instance::propagateResize()
        {
            if (hasBackend())
            {
                m_backend->resize(m_viewport);
            }
        }
    }
}