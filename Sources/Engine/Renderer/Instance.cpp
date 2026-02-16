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
            : m_resolution(Vec<2, std::uint32_t>(0)),
              m_resolutionObservable({}),
              m_frames({}),
              m_currentFrame(0U),
              m_polyResources({}),
              m_textureResources({}),
              m_skyResource({}),
              m_backend(nullptr)
        {}

        void Instance::init(const Settings& inSettings)
        {
            setFrameCount(inSettings.bufferCount);
            setResolution(inSettings.resolution);
        }

        void Instance::shutdown()
        {
            if (hasBackend())
            {
                m_backend->onShutdown();
            }
        }

        void Instance::render()
        {
            if (!hasBackend())
            {
                return;
            }

            for (auto& [type, resource] : m_polyResources)
            {
                if (resource.isDirty())
                {
                    m_backend->onLoad(type, resource);

                    resource.markAsClean();
                }
            }

            if (m_textureResources.isDirty())
            {
                m_backend->onLoad(m_textureResources);

                m_textureResources.markAsClean();
            }

            Frame& currentFrame = getCurrentFrame();
            currentFrame.setup(m_polyResources);
            currentFrame.setup(m_skyResource);

            m_backend->onBeginRender();
            m_backend->onRender(currentFrame);
            m_backend->onEndRender();

            currentFrame.reset();

            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
        }

        void Instance::useCamera(const View& inData)
        {
            getCurrentFrame().useCamera(inData);
        }

        void Instance::addLight(const View::List& inData)
        {
            getCurrentFrame().addLight(inData);
        }

        void Instance::addLight(const View& inData)
        {
            getCurrentFrame().addLight(inData);
        }

        void Instance::setFrameCount(std::uint32_t inValue)
        {
            m_frames.resize(inValue);
        }

        Frame& Instance::getCurrentFrame()
        {
            return m_frames.at(m_currentFrame);
        }

        Draw::Id Instance::loadPoly(DrawPolyType inType, const DrawPolyData& inData)
        {
            return getPolyResource(inType).add(inData);
        }

        Draw::Id Instance::findTexture(const Draw::Reference& inReference)
        {
            return m_textureResources.findId(inReference);
        }

        Draw::Id Instance::loadTexture(const DrawTextureData& inData)
        {
            return m_textureResources.add(inData);
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
                sky.textures.push_back(m_textureResources.getDraw(texture));
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

        const Vec<2, std::uint32_t>& Instance::getResolution() const
        {
            return m_resolution;
        }

        void Instance::setResolution(const Vec<2, std::uint32_t>& inValue)
        {
            if (m_resolution == inValue)
            {
                return;
            }

            m_resolution = inValue;

            if (hasBackend())
            {
                m_backend->setResolution(m_resolution);
                m_backend->onResize();
            }

            m_resolutionObservable.next(m_resolution);
        }

        ResolutionSubscription Instance::watchResolution(
            ResolutionSubscription::NextCallback     inNext,
            ResolutionSubscription::ErrorCallback    inError,
            ResolutionSubscription::CompleteCallback inComplete
        )
        {
            return m_resolutionObservable.subscribe(inNext, inError, inComplete).next(m_resolution);
        }

        bool Instance::hasBackend() const
        {
            return m_backend && m_backend.get() != nullptr;
        }

        void Instance::reloadBackend(const Window* inWindow)
        {
            switch (inWindow->getBackend())
            {
#if CHICANE_OPENGL
            case WindowBackend::OpenGL:
                m_backend = std::make_unique<OpenGLBackend>();

                break;
#endif

#if CHICANE_VULKAN
            case WindowBackend::Vulkan:
                m_backend = std::make_unique<VulkanBackend>();

                break;
#endif

            default:
                m_backend = std::make_unique<Backend>();

                break;
            }

            m_backend->setWindow(inWindow);
            m_backend->setResolution(m_resolution);

            m_backend->onInit();

            for (const auto& [type, resource] : m_polyResources)
            {
                m_backend->onLoad(type, resource);
            }

            m_backend->onLoad(m_textureResources);
            m_backend->onLoad(m_skyResource);
        }

        DrawPolyResource& Instance::getPolyResource(DrawPolyType inType)
        {
            return m_polyResources[inType];
        }
    }
}