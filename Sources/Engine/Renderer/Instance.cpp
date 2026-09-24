#include "Chicane/Renderer/Instance.hpp"

#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Resource.hpp"

#if CHICANE_OPENGL
    #include "Backend/OpenGL.hpp"
#endif

#if CHICANE_VULKAN
    #include "Backend/Vulkan.hpp"
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
              m_features(static_cast<std::uint16_t>(RendererFeature::Default)),
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

        std::unique_ptr<Image> Instance::captureScreen()
        {
            if (!hasBackend())
            {
                return nullptr;
            }

            std::uint32_t              width  = 0;
            std::uint32_t              height = 0;
            std::vector<unsigned char> rgba;
            if (!m_backend->captureScreen(width, height, rgba) || width == 0 || height == 0 || rgba.empty())
            {
                return nullptr;
            }

            return std::make_unique<Image>(rgba.data(), static_cast<int>(width), static_cast<int>(height), 4, 4);
        }

        void Instance::render()
        {
            if (!hasBackend())
            {
                return;
            }

            syncDirtyResources();

            Frame& currentFrame = getCurrentFrame();
            currentFrame.setFeature(getFeature());
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

        static bool hasSkySample(const Image::Instance& inImage)
        {
            return inImage && inImage->getWidth() > 0 && inImage->getHeight() > 0 &&
                   (inImage->getPixels() != nullptr || inImage->getFloatPixels() != nullptr);
        }

        Draw::Id Instance::loadSky(const DrawSkyData& inData)
        {
            m_skyResource.exposure = inData.exposure;
            m_skyResource.bVisible = inData.bVisible;

            DrawSky sky;
            sky.reference = inData.reference;
            sky.kind      = inData.kind;

            for (const Draw::Reference& texture : inData.textures)
            {
                const DrawTexture& draw = m_textureResources.getDraw(texture);
                if (!hasSkySample(draw.getSampleImage()))
                {
                    return Draw::InvalidId;
                }

                sky.textures.push_back(draw);
            }

            sky.model = getPolyResource(DrawPolyType::e3D).getDraw(inData.model);

            if (sky.model.id <= Draw::InvalidId)
            {
                return Draw::InvalidId;
            }

            const Draw::Id existing = findSky(inData.reference);
            if (existing != Draw::InvalidId)
            {
                const DrawSky& current = m_skyResource.getDraw();
                bool           bSame   = current.kind == sky.kind && current.model.id == sky.model.id &&
                             current.textures.size() == sky.textures.size();
                if (bSame)
                {
                    for (std::size_t index = 0; index < current.textures.size(); index++)
                    {
                        if (!current.textures[index].reference.equals(sky.textures[index].reference) ||
                            current.textures[index].width != sky.textures[index].width ||
                            current.textures[index].height != sky.textures[index].height)
                        {
                            bSame = false;

                            break;
                        }
                    }
                }

                if (bSame)
                {
                    return existing;
                }
            }

            m_skyResource.add(sky);

            return m_skyResource.findId(sky.reference);
        }

        void Instance::drawParticle(const DrawParticle& inData)
        {
            getCurrentFrame().drawParticle(inData);
        }

        void Instance::drawParticles(const DrawParticle::List& inData)
        {
            getCurrentFrame().drawParticles(inData);
        }

        void Instance::enableFeature(RendererFeature inFeature)
        {
            m_features.fetch_or(static_cast<std::uint16_t>(inFeature), std::memory_order_relaxed);
        }

        void Instance::disableFeature(RendererFeature inFeature)
        {
            m_features.fetch_and(
                static_cast<std::uint16_t>(~static_cast<std::uint16_t>(inFeature)),
                std::memory_order_relaxed
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