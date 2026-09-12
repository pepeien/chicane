#include "Chicane/Renderer/Instance.hpp"

#include <algorithm>
#include <atomic>

#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Resource.hpp"

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
            : m_settings({}),
              m_frames({}),
              m_currentFrame(0U),
              m_polyResources({}),
              m_textureResources({}),
              m_skyResource({}),
              m_features(static_cast<std::uint8_t>(RendererFeature::Fill)),
              m_backend(nullptr)
        {}

        void Instance::init(const Settings& inSettings)
        {
            m_settings = inSettings;

            setFramesInFlight(m_settings.framesInFlight);
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

            syncDirtyResources();

            Frame& currentFrame = getCurrentFrame();
            currentFrame.setup(m_polyResources);
            currentFrame.setup(m_skyResource);

            m_textureStreamer.tick(
                currentFrame,
                m_textureResources,
                m_backend->getResourceBudget(Resource::Texture),
                getResolution().y
            );

            if (m_textureResources.isDirty())
            {
                m_backend->onLoad(m_textureResources);
                m_textureResources.markAsClean();
            }

            m_backend->onBeginRender();
            m_backend->onRender(currentFrame);
            m_backend->onEndRender();

            currentFrame.reset();
            resetResources();
            Debug::prune(true);

            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
        }

        void Instance::useCamera(const View& inData)
        {
            getCurrentFrame().useCamera(inData);
        }

        void Instance::addLight(const Light::List& inData)
        {
            getCurrentFrame().addLight(inData);
        }

        void Instance::addLight(const Light& inData)
        {
            getCurrentFrame().addLight(inData);
        }

        Frame& Instance::getCurrentFrame()
        {
            return m_frames.at(m_currentFrame);
        }

        const Frame& Instance::getCurrentFrame() const
        {
            return m_frames.at(m_currentFrame);
        }

        Draw::Id Instance::findPoly(DrawPolyType inType, const Draw::Reference& inReference)
        {
            return getPolyResource(inType).findId(inReference);
        }

        Draw::Id Instance::loadPoly(DrawPolyType inType, const DrawPolyData& inData)
        {
            return getPolyResource(inType).add(inType, inData);
        }

        Draw::Id Instance::findGlyph(const Draw::Reference& inReference)
        {
            return getPolyResource(DrawPolyType::e2D).findGlyph(inReference);
        }

        Draw::Id Instance::loadGlyph(const DrawGlyphData& inData)
        {
            return getPolyResource(DrawPolyType::e2D).addGlyph(inData);
        }

        Draw::Id Instance::findTexture(const Draw::Reference& inReference)
        {
            return m_textureResources.findId(inReference);
        }

        Draw::Id Instance::loadTexture(const DrawTextureData& inData)
        {
            return m_textureResources.add(inData);
        }

        Draw::Id Instance::findTarget(const Draw::Reference& inReference)
        {
            return findTexture(inReference);
        }

        Draw::Id Instance::findSky(const Draw::Reference& inReference)
        {
            return m_skyResource.findId(inReference);
        }

        Draw::Id Instance::loadSky(const DrawSkyData& inData)
        {
            if (findSky(inData.reference) != Draw::InvalidId)
            {
                return m_skyResource.getDraw().id;
            }

            DrawSky sky;
            sky.reference = inData.reference;

            for (const Draw::Reference& texture : inData.textures)
            {
                sky.textures.push_back(m_textureResources.getDraw(texture));
            }

            sky.model = getPolyResource(DrawPolyType::e3D).getDraw(inData.model);

            if (sky.model.id <= Draw::InvalidId)
            {
                return Draw::InvalidId;
            }

            m_skyResource.add(sky);

            return m_skyResource.findId(sky.reference);
        }

        void Instance::enableFeature(RendererFeature inFeature)
        {
            m_features.fetch_or(static_cast<std::uint8_t>(inFeature), std::memory_order_relaxed);
        }

        void Instance::disableFeature(RendererFeature inFeature)
        {
            m_features.fetch_and(
                static_cast<std::uint8_t>(~static_cast<std::uint8_t>(inFeature)), std::memory_order_relaxed
            );
        }

        void Instance::toggleFeature(RendererFeature inFeature)
        {
            if (hasFeature(inFeature))
            {
                disableFeature(inFeature);

                return;
            }

            enableFeature(inFeature);
        }

        RendererFeature Instance::getFeature() const
        {
            return static_cast<RendererFeature>(m_features.load(std::memory_order_relaxed));
        }

        bool Instance::hasFeature(RendererFeature inFeature) const
        {
            return (getFeature() & inFeature) == inFeature;
        }

        float Instance::getGpuDelta() const
        {
            if (!hasBackend())
            {
                return 0.0f;
            }

            return m_backend->getGpuDelta();
        }

        bool Instance::hasWindow() const
        {
            return m_window != nullptr;
        }

        const Window* Instance::getWindow() const
        {
            return m_window;
        }

        void Instance::setWindow(const Window* inWindow)
        {
            m_window = inWindow;

            reloadBackend();
        }

        Vec<2, std::uint32_t> Instance::getResolution() const
        {
            if (!hasWindow())
            {
                return m_settings.resolution;
            }

            const Vec<2, std::uint32_t>& windowSize = m_window->getSize();

            return {std::max(m_settings.resolution.x, windowSize.x), std::max(m_settings.resolution.y, windowSize.y)};
        }

        void Instance::setResolution(const Vec<2, std::uint32_t>& inValue)
        {
            m_settings.resolution = inValue;

            if (hasBackend())
            {
                m_backend->onResize();
            }
        }

        std::uint32_t Instance::getFrameInFlighCount() const
        {
            return m_settings.framesInFlight;
        }

        void Instance::setFramesInFlight(std::uint32_t inValue)
        {
            m_settings.framesInFlight = inValue;

            m_frames.resize(inValue);
        }

        const ResourceBudget& Instance::getResourceBudget() const
        {
            return m_settings.resourceBudget;
        }

        void Instance::setResourceBudget(const ResourceBudget& inValue)
        {
            m_settings.resourceBudget = inValue;
        }

        bool Instance::hasBackend() const
        {
            return m_backend && m_backend.get() != nullptr;
        }

        void Instance::reloadBackend()
        {
            if (!hasWindow())
            {
                return;
            }

            switch (m_window->getBackend())
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

            m_backend->setRenderer(this);

            m_backend->onInit();

            DrawTextureData screen;
            screen.reference   = SCREEN_TARGET_ID;
            screen.bStreamable = false;
            loadTexture(screen);

            markResourcesAsDirty();
        }

        void Instance::syncDirtyResources()
        {
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

            if (m_skyResource.isDirty())
            {
                m_backend->onLoad(m_skyResource);

                m_skyResource.markAsClean();
            }
        }

        void Instance::markResourcesAsDirty()
        {
            for (auto& [type, resource] : m_polyResources)
            {
                resource.markAsDirty();
            }

            m_textureResources.markAsDirty();

            m_skyResource.markAsDirty();
        }

        void Instance::resetResources()
        {
            for (auto& [type, resource] : m_polyResources)
            {
                resource.reset();
            }

            m_textureResources.reset();
            m_skyResource.reset();
        }

        DrawPolyResource& Instance::getPolyResource(DrawPolyType inType)
        {
            return m_polyResources[inType];
        }
    }
}