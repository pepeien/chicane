#include "Chicane/Renderer/Backend.hpp"

#include <algorithm>

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Renderer/Draw/Particle.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Shadow.hpp"
#include "Chicane/Renderer/Shadow/Light.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Backend::Backend()
            : m_renderer(nullptr),
              m_layers({}),
              m_rhi(nullptr),
              m_VRAM(0U),
              m_gpuDelta(0.0f),
              m_bPreviewPass(false),
              m_previewViewport({}),
              m_viewSlots({}),
              m_viewSlotCount(0),
              m_viewPlaceholder({}),
              m_status(BackendStatus::Shutdown)
        {}

        void Backend::onInit()
        {
            m_status = BackendStatus::Running;
            registerViewTarget(SCREEN_TARGET_ID);
        }

        void Backend::onShutdown()
        {
            m_status = BackendStatus::Shutdown;
        }

        void Backend::onResize()
        {
            return;
        }

        void Backend::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inType, inResource);
            }
        }

        void Backend::onLoad(const DrawTextureResource& inResources)
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inResources);
            }
        }

        void Backend::onLoad(const DrawSkyResource& inResource)
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inResource);
            }
        }

        void Backend::onBeginRender()
        {
            return;
        }

        void Backend::onRender(const Frame& inFrame)
        {
            return;
        }

        void Backend::onEndRender()
        {
            return;
        }

        bool Backend::captureScreen(
            std::uint32_t& outWidth, std::uint32_t& outHeight, std::vector<unsigned char>& outRgba
        )
        {
            outWidth  = 0;
            outHeight = 0;
            outRgba.clear();

            return false;
        }

        const Instance* Backend::getRenderer() const
        {
            return m_renderer;
        }

        bool Backend::hasFeature(RendererFeature inFeature) const
        {
            if (!m_renderer)
            {
                return inFeature == RendererFeature::Fill;
            }

            return m_renderer->hasFeature(inFeature);
        }

        Viewport Backend::getLayerViewport(Layer* inLayer) const
        {
            Vec<2, std::uint32_t>  resolution = m_renderer->getResolution();
            const ViewportSettings viewport   = inLayer->m_viewport;

            Size size;
            size.setIsAsobute(true);
            size.setRoot(resolution);
            size.setParent(resolution);

            Viewport result;
            result.size.x     = size.parse(viewport.width, SizeDirection::Horizontal);
            result.size.y     = size.parse(viewport.height, SizeDirection::Vertical);
            result.position.x = size.parse(viewport.offsetX, SizeDirection::Horizontal);
            result.position.y = size.parse(viewport.offsetY, SizeDirection::Vertical);

            return result;
        }

        std::vector<Layer*> Backend::findLayers(std::function<bool(const Layer* inLayer)> inPredicate) const
        {
            std::vector<Layer*> result;

            for (const std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!inPredicate(layer.get()))
                {
                    continue;
                }

                result.push_back(layer.get());
            }

            return result;
        }

        std::size_t Backend::getResourceSize(Resource inType)
        {
            switch (inType)
            {
            case Resource::SceneIndices:
                return sizeof(Vertex::Index);

            case Resource::SceneVertices:
                return sizeof(Vertex);

            case Resource::SceneCamera:
                return sizeof(View);

            case Resource::SceneLights:
                return sizeof(ShadowLight);

            case Resource::SceneInstances:
                return sizeof(DrawPoly3DInstance) + sizeof(View);

            case Resource::Scene:
                return getResourceSize(Resource::SceneIndices) + getResourceSize(Resource::SceneVertices) +
                       getResourceSize(Resource::SceneInstances) + getResourceSize(Resource::SceneCamera) +
                       getResourceSize(Resource::SceneLights);

            case Resource::Texture:
                return sizeof(Image::Pixel) * 4u * TEXTURE_STREAM_TAIL * TEXTURE_STREAM_TAIL;

            case Resource::UIIndices:
                return sizeof(Vertex::Index);

            case Resource::UIVertices:
                return sizeof(Vertex);

            case Resource::UIInstances:
                return sizeof(DrawPoly2DInstance);

            case Resource::UIGlyphs:
                return sizeof(float);

            case Resource::UI:
                return getResourceSize(Resource::UIIndices) + getResourceSize(Resource::UIVertices) +
                       getResourceSize(Resource::UIInstances) + getResourceSize(Resource::UIGlyphs);

            default:
                return 0U;
            }
        }

        std::size_t Backend::getResourceBudget(Resource inType)
        {
            const ResourceBudget& budget = m_renderer->getResourceBudget();

            if (budget.find(inType) == budget.end())
            {
                return 0U;
            }

            if (inType == Resource::Texture)
            {
                return static_cast<std::size_t>(budget.at(inType) * static_cast<float>(m_VRAM));
            }

            std::size_t bytes = getResourceSize(inType) * getResourceBudgetCount(inType) * 0.5f;
            std::size_t cap   = 0;
            switch (inType)
            {
            case Resource::SceneVertices:
                cap = RESOURCE_CAP_SCENE_VERTICES;
                break;
            case Resource::SceneIndices:
                cap = RESOURCE_CAP_SCENE_INDICES;
                break;
            case Resource::SceneInstances:
                cap = RESOURCE_CAP_SCENE_INSTANCES;
                break;
            case Resource::UIVertices:
                cap = RESOURCE_CAP_UI_VERTICES;
                break;
            case Resource::UIIndices:
                cap = RESOURCE_CAP_UI_INDICES;
                break;
            case Resource::UIInstances:
                cap = RESOURCE_CAP_UI_INSTANCES;
                break;
            case Resource::UIGlyphs:
                cap = RESOURCE_CAP_UI_GLYPHS;
                break;
            default:
                break;
            }

            if (cap > 0)
            {
                bytes = std::min(bytes, cap);
            }

            return bytes;
        }

        std::uint32_t Backend::getResourceBudgetCount(Resource inType)
        {
            const ResourceBudget& budget = m_renderer->getResourceBudget();

            if (budget.find(inType) == budget.end())
            {
                return 0U;
            }

            std::uint32_t count = (budget.at(inType) * m_VRAM) / getResourceSize(inType);
            if (inType == Resource::Texture)
            {
                count = std::min(count, TEXTURE_SLOT_MAX);
            }

            return count;
        }

        bool Backend::isStatus(BackendStatus inValue) const
        {
            return m_status == inValue;
        }

        RHI::Device* Backend::getRHIDevice() const
        {
            return m_rhi.get();
        }

        RHI::Viewport Backend::getRHIViewport(Layer* inLayer) const
        {
            const Viewport viewport = getLayerViewport(inLayer);

            RHI::Viewport result;
            result.size     = viewport.size;
            result.position = viewport.position;

            return result;
        }

        RHI::Scissor Backend::getRHIScissor(Layer* inLayer) const
        {
            const Viewport viewport = getLayerViewport(inLayer);

            RHI::Scissor result;
            result.x      = static_cast<std::int32_t>(viewport.position.x);
            result.y      = static_cast<std::int32_t>(viewport.position.y);
            result.width  = static_cast<std::uint32_t>(viewport.size.x);
            result.height = static_cast<std::uint32_t>(viewport.size.y);

            return result;
        }

        float Backend::getGpuDelta() const
        {
            return m_gpuDelta;
        }

        Draw::Id Backend::getScreenTextureId() const
        {
            return viewTargetTexture(0);
        }

        bool Backend::isScreenComposited(const Frame& inFrame) const
        {
            const Draw::Id screenId = getScreenTextureId();

            if (screenId <= Draw::InvalidId)
            {
                return false;
            }

            for (const DrawPoly2DInstance& instance : inFrame.getInstances2D())
            {
                if (instance.texture == screenId)
                {
                    return true;
                }
            }

            return false;
        }

        void Backend::setGpuDelta(float inMilliseconds)
        {
            m_gpuDelta = inMilliseconds;
        }

        int Backend::registerViewTarget(const String& inName)
        {
            if (inName.isEmpty())
            {
                return -1;
            }

            const int existing = viewTargetSlot(inName);
            if (existing >= 0)
            {
                return existing;
            }

            if (m_viewSlotCount >= VIEW_TARGET_MAX)
            {
                return -1;
            }

            const std::uint32_t slot = m_viewSlotCount++;
            m_viewSlots[slot].name   = inName;

            return static_cast<int>(slot);
        }

        int Backend::viewTargetSlot(const String& inName) const
        {
            for (std::uint32_t slot = 0; slot < m_viewSlotCount; slot++)
            {
                if (m_viewSlots[slot].name.equals(inName))
                {
                    return static_cast<int>(slot);
                }
            }

            return -1;
        }

        void Backend::assignViewTargetTexture(std::uint32_t inSlot, Draw::Id inTexture)
        {
            if (inSlot >= m_viewSlotCount)
            {
                return;
            }

            m_viewSlots[inSlot].texture = inTexture;
        }

        Draw::Id Backend::viewTargetTexture(std::uint32_t inSlot) const
        {
            if (inSlot >= m_viewSlotCount)
            {
                return Draw::InvalidId;
            }

            return m_viewSlots[inSlot].texture;
        }

        RHI::Image Backend::viewTargetImage(std::uint32_t inSlot) const
        {
            if (inSlot == 0 || inSlot >= m_viewSlotCount || !m_viewSlots[inSlot].color.handle)
            {
                return {};
            }

            return m_viewSlots[inSlot].color;
        }

        RHI::Image Backend::viewPlaceholder() const
        {
            return m_viewPlaceholder;
        }

        void Backend::discardViewTarget(RHI::Image)
        {}

        void Backend::ensureViewPlaceholder()
        {
            if (!m_rhi || m_viewPlaceholder.handle)
            {
                return;
            }

            RHI::ImageCreateInfo placeholder;
            placeholder.kind       = RHI::ImageKind::Color2D;
            placeholder.format     = m_rhi->sceneColorFormat();
            placeholder.width      = 1;
            placeholder.height     = 1;
            placeholder.bIsSampled = true;
            placeholder.bHasColor  = true;
            m_viewPlaceholder      = m_rhi->createImage(placeholder);
        }

        void Backend::ensureViewTarget(std::uint32_t inSlot, std::uint32_t inWidth, std::uint32_t inHeight)
        {
            if (!m_rhi || inSlot == 0 || inSlot >= m_viewSlotCount || inWidth == 0 || inHeight == 0)
            {
                return;
            }

            BackendViewTargetSlot& slot = m_viewSlots[inSlot];
            if (!slot.camera.handle)
            {
                RHI::BufferCreateInfo camera;
                camera.size           = sizeof(View);
                camera.usage          = RHI::BufferUsage::Uniform;
                camera.bHasHostAccess = true;
                slot.camera           = m_rhi->createBuffer(camera);

                RHI::BufferCreateInfo light;
                light.size           = sizeof(ShadowLight);
                light.usage          = RHI::BufferUsage::Storage;
                light.bHasHostAccess = true;
                slot.light           = m_rhi->createBuffer(light);

                RHI::BufferCreateInfo instances;
                instances.size  = std::max(getResourceBudget(Resource::SceneInstances), sizeof(DrawPoly3DInstance));
                instances.usage = RHI::BufferUsage::Storage;
                instances.bHasHostAccess = true;
                slot.instances           = m_rhi->createBuffer(instances);

                RHI::BufferCreateInfo particles;
                particles.size           = sizeof(DrawParticle) * MAX_PARTICLES;
                particles.usage          = RHI::BufferUsage::Storage;
                particles.bHasHostAccess = true;
                slot.particles           = m_rhi->createBuffer(particles);
            }

            if (slot.color.handle && slot.width == inWidth && slot.height == inHeight)
            {
                return;
            }

            if (slot.color.handle)
            {
                m_rhi->destroyImage(slot.color);
                slot.color = {};
            }

            if (slot.depth.handle)
            {
                m_rhi->destroyImage(slot.depth);
                slot.depth = {};
            }

            RHI::ImageCreateInfo color;
            color.kind       = RHI::ImageKind::Color2D;
            color.format     = m_rhi->sceneColorFormat();
            color.width      = inWidth;
            color.height     = inHeight;
            color.bIsSampled = true;
            color.bHasColor  = true;
            slot.color       = m_rhi->createImage(color);

            RHI::ImageCreateInfo depth;
            depth.kind       = RHI::ImageKind::Depth2D;
            depth.format     = m_rhi->sceneDepthFormat();
            depth.width      = inWidth;
            depth.height     = inHeight;
            depth.bIsSampled = false;
            depth.bHasColor  = false;
            depth.bHasDepth  = true;
            slot.depth       = m_rhi->createImage(depth);

            slot.width  = inWidth;
            slot.height = inHeight;
        }

        void Backend::destroyViewTargets()
        {
            if (!m_rhi)
            {
                return;
            }

            auto killImage = [&](RHI::Image& inImage)
            {
                if (!inImage.handle)
                {
                    return;
                }

                m_rhi->destroyImage(inImage);
                inImage = {};
            };
            auto killBuffer = [&](RHI::Buffer& inBuffer)
            {
                if (!inBuffer.handle)
                {
                    return;
                }

                m_rhi->destroyBuffer(inBuffer);
                inBuffer = {};
            };

            killImage(m_viewPlaceholder);
            for (BackendViewTargetSlot& slot : m_viewSlots)
            {
                killImage(slot.color);
                killImage(slot.depth);
                killBuffer(slot.camera);
                killBuffer(slot.light);
                killBuffer(slot.instances);
                killBuffer(slot.particles);
                slot.width  = 0;
                slot.height = 0;
            }
        }

        void Backend::prepareViewTargets()
        {
            if (!m_rhi || !m_renderer)
            {
                return;
            }

            ensureViewPlaceholder();
            for (const InstanceViewTarget& target : m_renderer->getViewTargets())
            {
                const int slot = viewTargetSlot(target.name);
                if (slot <= 0)
                {
                    continue;
                }

                ensureViewTarget(static_cast<std::uint32_t>(slot), target.width, target.height);
            }
        }

        void Backend::renderViewTargets(RHI::Frame& ioFrame, bool bFlipY)
        {
            if (!m_renderer)
            {
                return;
            }

            for (const InstanceViewTarget& target : m_renderer->getViewTargets())
            {
                const int slot = viewTargetSlot(target.name);
                if (slot <= 0)
                {
                    continue;
                }

                const std::uint32_t index = static_cast<std::uint32_t>(slot);
                discardViewTarget(m_viewSlots[index].color);
                discardViewTarget(m_viewSlots[index].depth);
                uploadViewTarget(index, target.frame, bFlipY);
                drawViewTarget(index, target.frame, ioFrame);
            }
        }

        void Backend::uploadViewTarget(std::uint32_t inSlot, const Frame& inFrame, bool bFlipY)
        {
            if (!m_rhi || inSlot >= m_viewSlotCount || !m_viewSlots[inSlot].camera.handle)
            {
                return;
            }

            BackendViewTargetSlot& slot = m_viewSlots[inSlot];

            View camera = inFrame.getCamera();
            if (bFlipY)
            {
                camera.flipY();
            }
            camera.depthZeroToOne();
            m_rhi->updateBuffer(slot.camera, &camera, sizeof(View));

            ShadowLight light =
                Shadow::build(inFrame.getCamera(), inFrame.getLights(), inFrame.hasFeature(RendererFeature::Light));
            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; cascade++)
            {
                if (bFlipY)
                {
                    light.projections[cascade][1][1] *= -1.0f;
                }

                Mat4 depth                 = Mat4::One;
                depth[2][2]                = 0.5f;
                depth[3][2]                = 0.5f;
                light.projections[cascade] = depth * light.projections[cascade];
            }
            m_rhi->updateBuffer(slot.light, &light, sizeof(ShadowLight));

            const DrawPoly3DInstance::List& instances = inFrame.getInstances3D();
            if (!instances.empty())
            {
                m_rhi->updateBuffer(slot.instances, instances.data(), sizeof(DrawPoly3DInstance) * instances.size());
            }

            const DrawParticle::List& particles = inFrame.getParticles();
            const std::uint32_t       count     = std::min(static_cast<std::uint32_t>(particles.size()), MAX_PARTICLES);
            if (count > 0)
            {
                m_rhi->updateBuffer(slot.particles, particles.data(), sizeof(DrawParticle) * count);
            }
        }

        void Backend::drawViewTarget(std::uint32_t inSlot, const Frame& inFrame, RHI::Frame& ioFrame)
        {
            if (!m_renderer || inSlot >= m_viewSlotCount || !m_viewSlots[inSlot].color.handle)
            {
                return;
            }

            BackendViewTargetSlot& slot   = m_viewSlots[inSlot];
            const std::uint32_t    width  = slot.width;
            const std::uint32_t    height = slot.height;

            struct Restore
            {
                Backend*      backend;
                RHI::Frame&   frame;
                std::uint32_t index;
                std::uint32_t width;
                std::uint32_t height;
                RHI::Image    color;
                RHI::Image    depth;
                RHI::Buffer   camera;
                RHI::Buffer   light;
                RHI::Buffer   instances;
                RHI::Buffer   particles;

                Restore(
                    Backend*      inBackend,
                    RHI::Frame&   inFrame,
                    std::uint32_t inIndex,
                    std::uint32_t inWidth,
                    std::uint32_t inHeight,
                    RHI::Image    inColor,
                    RHI::Image    inDepth,
                    RHI::Buffer   inCamera,
                    RHI::Buffer   inLight,
                    RHI::Buffer   inInstances,
                    RHI::Buffer   inParticles
                )
                    : backend(inBackend),
                      frame(inFrame),
                      index(inIndex),
                      width(inWidth),
                      height(inHeight),
                      color(inColor),
                      depth(inDepth),
                      camera(inCamera),
                      light(inLight),
                      instances(inInstances),
                      particles(inParticles)
                {}

                ~Restore()
                {
                    backend->m_bPreviewPass = false;
                    frame.frameIndex        = index;
                    frame.width             = width;
                    frame.height            = height;
                    frame.sceneColor        = color;
                    frame.sceneDepth        = depth;
                    frame.cameraBuffer      = camera;
                    frame.lightBuffer       = light;
                    frame.instance3DBuffer  = instances;
                    frame.particleBuffer    = particles;
                }
            } restore(
                this,
                ioFrame,
                ioFrame.frameIndex,
                ioFrame.width,
                ioFrame.height,
                ioFrame.sceneColor,
                ioFrame.sceneDepth,
                ioFrame.cameraBuffer,
                ioFrame.lightBuffer,
                ioFrame.instance3DBuffer,
                ioFrame.particleBuffer
            );

            ioFrame.frameIndex       = m_renderer->getFrameInFlighCount() + inSlot;
            ioFrame.width            = width;
            ioFrame.height           = height;
            ioFrame.sceneColor       = slot.color;
            ioFrame.sceneDepth       = slot.depth;
            ioFrame.cameraBuffer     = slot.camera;
            ioFrame.lightBuffer      = slot.light;
            ioFrame.instance3DBuffer = slot.instances;
            ioFrame.particleBuffer   = slot.particles;

            m_previewViewport.position = Vec2::sZero();
            m_previewViewport.size     = Vec2(static_cast<float>(width), static_cast<float>(height));
            m_previewViewport.depth    = Vec2(0.0f, 1.0f);
            m_bPreviewPass             = true;

            renderLayers(
                inFrame,
                &ioFrame,
                [](const Layer* inLayer)
                {
                    if (!inLayer)
                    {
                        return false;
                    }

                    const String& id = inLayer->getId();
                    return id.equals(SCENE_SKY_LAYER_ID) || id.equals(SCENE_SHADOW_LAYER_ID) ||
                           id.equals(SCENE_MESH_LAYER_ID) || id.equals(SCENE_PARTICLE_LAYER_ID);
                }
            );
        }

        RHI::Scissor Backend::previewScissor() const
        {
            RHI::Scissor result;
            result.x      = static_cast<std::int32_t>(m_previewViewport.position.x);
            result.y      = static_cast<std::int32_t>(m_previewViewport.position.y);
            result.width  = static_cast<std::uint32_t>(m_previewViewport.size.x);
            result.height = static_cast<std::uint32_t>(m_previewViewport.size.y);

            return result;
        }

        void Backend::renderLayers(
            const Frame& inFrame, void* inData, std::function<bool(const Layer* inLayer)> inFilter
        )
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                if (inFilter && !inFilter(layer.get()))
                {
                    continue;
                }

                if (!layer->onBeginRender(inFrame))
                {
                    continue;
                }

                layer->onRender(inFrame, inData);
                layer->onEndRender();
            }
        }

        void Backend::shutdownLayers()
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onShutdown();
            }
        }

        void Backend::rebuildLayers()
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onRestart();
            }
        }

        void Backend::destroyLayers()
        {
            for (std::shared_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onDestruction();
            }

            m_layers.clear();
        }

        void Backend::setVRAM(std::size_t inBytes)
        {
            m_VRAM = inBytes;
        }

        void Backend::setRenderer(const Instance* inValue)
        {
            m_renderer = inValue;
        }
    }
}