#include "Chicane/Renderer/Instance.hpp"

#if CHICANE_OPENGL
    #include "Chicane/Renderer/Backend/OpenGL.hpp"
#endif

#if CHICANE_VULKAN
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
              m_resolution(Vec<2, int>(0)),
              m_resolutionObservable({}),
              m_viewport({}),
              m_viewportObservable({}),
              m_backend(nullptr)
        {}

        Instance::~Instance()
        {
            destroy();
        }

        void Instance::init(Window* inWindow, WindowBackend inBackend, const Settings& inSettings)
        {
            if (!inWindow)
            {
                return;
            }

            // Window
            setWindow(inWindow);

            // Renderer
            setResolution(inSettings.resolution);
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
            currentFrame.setup(m_skyResource);

            m_backend->onSetup(currentFrame);
            m_backend->onRender(currentFrame);
            m_backend->onCleanup();

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
                m_backend->onLoad(inType, getPolyResource(inType));
            }

            return id;
        }

        Draw::Id Instance::findTexture(const Draw::Reference& inReference)
        {
            const auto& found = std::find_if(
                m_textureResources.begin(),
                m_textureResources.end(),
                [inReference](const DrawTexture& inTexture) { return inTexture.reference.equals(inReference); }
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
                [inData](const DrawTexture& inTexture) { return inTexture.reference.equals(inData.reference); }
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
                m_backend->onLoad(m_textureResources);
            }

            return texture.id;
        }

        Draw::Id Instance::findSky(const Draw::Reference& inReference)
        {
            return m_skyResource.id;
        }

        Draw::Id Instance::loadSky(const DrawSkyData& inData)
        {
            if (m_skyResource.reference.equals(inData.reference))
            {
                return m_skyResource.id;
            }

            DrawSky sky;
            sky.id        = 1;
            sky.reference = inData.reference;

            for (const Draw::Reference& texture : inData.textures)
            {
                const auto& found = std::find_if(
                    m_textureResources.begin(),
                    m_textureResources.end(),
                    [texture](const DrawTexture& resource) { return resource.reference.equals(texture); }
                );

                if (found == m_textureResources.end())
                {
                    return Draw::UnknownId;
                }

                sky.textures.push_back(*found);
            }

            sky.textures.at(0).image.rotate(-90.0f);  // Right
            sky.textures.at(1).image.rotate(90.0f);   // Left
            sky.textures.at(3).image.rotate(-180.0f); // Back
            sky.textures.at(4).image.rotate(180.0f);  // Up

            sky.model = getPolyResource(DrawPolyType::e3D).getDraw(inData.model);

            if (sky.model.id == Draw::UnknownId)
            {
                return Draw::UnknownId;
            }

            m_skyResource = sky;

            if (hasBackend())
            {
                m_backend->onLoad(m_skyResource);
            }

            return m_skyResource.id;
        }

        const Vec<2, int>& Instance::getResolution() const
        {
            return m_resolution;
        }

        void Instance::setResolution(const Vec<2, int>& inValue)
        {
            if (m_resolution == inValue)
            {
                return;
            }

            m_resolution = inValue;

            m_resolutionObservable.next(m_resolution);

            refreshViewport();
        }

        ResolutionSubscription Instance::watchResolution(
            ResolutionSubscription::NextCallback     inNext,
            ResolutionSubscription::ErrorCallback    inError,
            ResolutionSubscription::CompleteCallback inComplete
        )
        {
            return m_resolutionObservable.subscribe(inNext, inError, inComplete).next(m_resolution);
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

        ViewportSubscription Instance::watchViewport(
            ViewportSubscription::NextCallback     inNext,
            ViewportSubscription::ErrorCallback    inError,
            ViewportSubscription::CompleteCallback inComplete
        )
        {
            return m_viewportObservable.subscribe(inNext, inError, inComplete).next(m_viewport);
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
            m_window->watchSize([&](const Vec<2, int>& inSize) { refreshViewport(); });
            m_window->watchEvent([&](WindowEvent inEvent) { handle(inEvent); });
        }

        void Instance::handle(const WindowEvent& inEvent)
        {
            if (!hasBackend())
            {
                return;
            }

            m_backend->onHandle(inEvent);
        }

        bool Instance::hasBackend() const
        {
            return m_backend && m_backend.get() != nullptr;
        }

        Backend* Instance::getBackend()
        {
            return m_backend.get();
        }

        BackendSubscription Instance::watchBackend(
            BackendSubscription::NextCallback     inNext,
            BackendSubscription::ErrorCallback    inError,
            BackendSubscription::CompleteCallback inComplete
        )
        {
            return m_backendObservable.subscribe(inNext, inError, inComplete).next(m_backend->getType());
        }

        void Instance::setBackend(WindowBackend inType)
        {
            if (hasBackend())
            {
                m_backend.reset();
            }

            switch (inType)
            {
#if CHICANE_OPENGL
            case WindowBackend::OpenGL:
                m_backend = std::make_unique<OpenGLBackend>(this);

                break;
#endif

#if CHICANE_VULKAN
            case WindowBackend::Vulkan:
                m_backend = std::make_unique<VulkanBackend>(this);

                break;
#endif

            default:
                m_backend = std::make_unique<Backend>(this);

                break;
            }

            m_backend->onResize(m_viewport);
            m_backend->onInit();

            m_backendObservable.next(inType);

            reloadResources();
        }

        Frame& Instance::getCurrentFrame()
        {
            return m_frames.at(m_currentFrame);
        }

        DrawPolyResource& Instance::getPolyResource(DrawPolyType inType)
        {
            return m_polyResources[inType];
        }

        void Instance::reloadResources()
        {
            if (!hasBackend())
            {
                return;
            }

            for (const auto& [type, resource] : m_polyResources)
            {
                m_backend->onLoad(type, resource);
            }

            m_backend->onLoad(m_textureResources);
            m_backend->onLoad(m_skyResource);
        }

        void Instance::refreshViewport()
        {
            if (m_resolution.x == 0 || m_resolution.y == 0)
            {
                return;
            }

            Vec<2, int> windowSize   = getWindow()->getSize();
            Vec<2, int> rendererSize = m_resolution;

            bool bIsRenderingScalable = rendererSize.x <= 0.0f || rendererSize.y <= 0.0f;
            bool bIsRenderingUp       = windowSize.x < rendererSize.x || windowSize.y < rendererSize.y;

            if (bIsRenderingUp || bIsRenderingScalable)
            {
                rendererSize = windowSize;
            }

            int scale = std::min(windowSize.x / rendererSize.x, windowSize.y / rendererSize.y);

            setViewportSize(rendererSize * scale);
            setViewportPosition((windowSize - rendererSize) / 2);
        }

        void Instance::propagateResize()
        {
            m_viewportObservable.next(m_viewport);

            if (hasBackend())
            {
                m_backend->onResize(m_viewport);
            }
        }
    }
}