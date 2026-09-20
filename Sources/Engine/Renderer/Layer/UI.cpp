#include "Chicane/Renderer/Layer/UI.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Feature.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/RHI/Screen/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LUI::LUI()
            : Layer(UI_LAYER_ID)
        {}

        void LUI::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BufferCreateInfo vertex;
            vertex.size  = std::min(m_backend->getResourceBudget(Resource::UIVertices), RESOURCE_MESH_INITIAL_BYTES);
            vertex.usage = RHI::BufferUsage::Vertex;
            vertex.bHasHostAccess = false;
            m_vertexBuffer        = device->createBuffer(vertex);

            RHI::BufferCreateInfo index;
            index.size  = std::min(m_backend->getResourceBudget(Resource::UIIndices), RESOURCE_MESH_INITIAL_BYTES);
            index.usage = RHI::BufferUsage::Index;
            index.bHasHostAccess = false;
            m_indexBuffer        = device->createBuffer(index);

            RHI::BufferCreateInfo glyph;
            glyph.size  = std::min(m_backend->getResourceBudget(Resource::UIGlyphs), RESOURCE_MESH_INITIAL_BYTES);
            glyph.usage = RHI::BufferUsage::Storage;
            glyph.bHasHostAccess = true;
            m_glyphBuffer        = device->createBuffer(glyph);

            RHI::SamplerCreateInfo sampler;
            sampler.minFilter = RHI::SamplerFilter::Linear;
            sampler.magFilter = RHI::SamplerFilter::Linear;
            sampler.address   = RHI::SamplerAddress::ClampToEdge;
            sampler.bHasMip   = true;
            m_backdropSampler = device->createSampler(sampler);

            RHI::BindGroupLayoutCreateInfo layout;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_UI_INSTANCES;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_UI_GLYPHS;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_UI_BACKDROP;
            binding.type        = RHI::BindingType::SampledImage;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_UI_SCREEN;
            binding.type        = RHI::BindingType::SampledImage;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            m_layout = device->createBindGroupLayout(layout);

            RHI::BindGroupLayoutCreateInfo textures;
            binding.binding           = RHI_BINDING_TEXTURES;
            binding.type              = RHI::BindingType::SampledImage;
            binding.count             = TEXTURE_SLOT_MAX;
            binding.bIsVertex         = false;
            binding.bIsFragment       = true;
            binding.bIsPartiallyBound = true;
            textures.bindings.push_back(binding);
            m_textureLayout = device->createBindGroupLayout(textures);

            RHI::PipelineCreateInfo desc;
            desc.vertexPath       = "Assets/Engine/Shaders/UI";
            desc.fragmentPath     = "Assets/Engine/Shaders/UI";
            desc.vertexStride     = sizeof(Vertex);
            desc.vertexAttributes = rhiPoly2DAttributes();
            desc.cull             = CullingMode::None;
            desc.blend            = RHI::BlendMode::Alpha;
            desc.bHasColor        = true;
            desc.bHasDepth        = false;
            desc.colorFormat      = device->presentColorFormat();
            desc.pushConstantSize = sizeof(RHI::ScreenPush);
            desc.bHasPushFragment = true;
            desc.layouts          = {m_layout, m_textureLayout};
            m_pipeline            = device->createPipeline(desc);
        }

        void LUI::onDestruction()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            for (UIBackdrop& slot : m_backdrops)
            {
                if (slot.image.handle)
                {
                    device->destroyImage(slot.image);
                }
            }
            for (RHI::BindGroup group : m_groups)
            {
                device->destroyBindGroup(group);
            }
            device->destroySampler(m_backdropSampler);
            device->destroyBuffer(m_vertexBuffer);
            device->destroyBuffer(m_indexBuffer);
            device->destroyBuffer(m_glyphBuffer);
            device->destroyPipeline(m_pipeline);
            device->destroyBindGroupLayout(m_textureLayout);
            device->destroyBindGroupLayout(m_layout);
        }

        void LUI::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType != DrawPolyType::e2D)
            {
                return;
            }

            RHI::Device* device = m_backend->getRHIDevice();
            device->updateBuffer(
                m_vertexBuffer,
                inResource.getVertices().data(),
                sizeof(Vertex) * inResource.getVertices().size()
            );
            device->updateBuffer(
                m_indexBuffer,
                inResource.getIndices().data(),
                sizeof(Vertex::Index) * inResource.getIndices().size()
            );
            device->updateBuffer(
                m_glyphBuffer,
                inResource.getGlyphOutlines().data(),
                sizeof(float) * inResource.getGlyphOutlines().size()
            );
        }

        bool LUI::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasDraws(DrawPolyType::e2D, DrawPolyMode::Fill);
        }

        void LUI::ensureBackdrop(
            RHI::Device* inDevice, std::uint32_t inIndex, std::uint32_t inWidth, std::uint32_t inHeight
        )
        {
            if (m_backdrops.size() <= inIndex)
            {
                m_backdrops.resize(inIndex + 1);
            }

            UIBackdrop& slot = m_backdrops[inIndex];
            if (slot.image.handle && slot.width == inWidth && slot.height == inHeight)
            {
                return;
            }

            if (slot.image.handle)
            {
                inDevice->destroyImage(slot.image);
                slot.image = {};
            }

            std::uint32_t levels = 1;
            std::uint32_t size   = std::max(inWidth, inHeight);
            while (size > 1)
            {
                size >>= 1;
                levels++;
            }

            RHI::ImageCreateInfo desc;
            desc.kind       = RHI::ImageKind::Color2D;
            desc.format     = RHI::ImageFormat::RGBA8;
            desc.width      = inWidth;
            desc.height     = inHeight;
            desc.mipLevels  = levels;
            desc.bIsSampled = true;
            desc.bHasColor  = true;
            slot.image      = inDevice->createImage(desc);
            slot.width      = inWidth;
            slot.height     = inHeight;
        }

        void LUI::onRender(const Frame& inFrame, void* inData)
        {
            RHI::Frame*         rhi      = static_cast<RHI::Frame*>(inData);
            RHI::Device*        device   = m_backend->getRHIDevice();
            const RHI::Viewport viewport = m_backend->getRHIViewport(this);
            const std::uint32_t width    = std::max(1u, static_cast<std::uint32_t>(viewport.size.x));
            const std::uint32_t height   = std::max(1u, static_cast<std::uint32_t>(viewport.size.y));

            ensureBackdrop(device, rhi->frameIndex, width, height);

            auto bindUi = [&]()
            {
                rhiReplaceGroup(
                    device,
                    m_groups,
                    rhi->frameIndex,
                    m_layout,
                    {
                        {RHI_BINDING_UI_INSTANCES, RHI::BindingType::StorageBuffer, rhi->instance2DBuffer, {}, {}, 0},
                        {RHI_BINDING_UI_GLYPHS,    RHI::BindingType::StorageBuffer, m_glyphBuffer,         {}, {}, 0},
                        {RHI_BINDING_UI_BACKDROP,
                         RHI::BindingType::SampledImage,
                         {},
                         m_backdrops[rhi->frameIndex].image,
                         m_backdropSampler,                                                                        0},
                        {RHI_BINDING_UI_SCREEN,
                         RHI::BindingType::SampledImage,
                         {},
                         rhi->sceneColor,
                         rhi->linearSampler,
                         0                                                                                          }
                }
                );
            };

            bool bIsPass = false;
            auto endPass = [&]()
            {
                if (!bIsPass)
                {
                    return;
                }
                rhi->commands->endPass();
                bIsPass = false;
            };
            auto beginPass = [&]()
            {
                if (bIsPass)
                {
                    return;
                }
                RHI::PassCreateInfo pass;
                pass.bHasColor   = true;
                pass.color.image = rhi->presentColor;
                pass.color.load  = RHI::LoadOp::Load;
                pass.color.store = RHI::StoreOp::Store;
                pass.width       = rhi->width;
                pass.height      = rhi->height;
                rhi->commands->beginPass(pass);
                rhiApplyView(rhi->commands, m_backend, this);
                rhi->commands->bindPipeline(m_pipeline);
                rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                rhi->commands->bindGroup(1, rhi->textureTable);
                RHI::ScreenPush push;
                push.data[0] = static_cast<std::int32_t>(m_backend->getScreenTextureId());
                push.data[1] = inFrame.hasFeature(RendererFeature::HDR) ? 1 : 0;
                rhi->commands->pushConstants(&push, sizeof(push));
                rhi->commands->bindVertexBuffer(m_vertexBuffer);
                rhi->commands->bindIndexBuffer(m_indexBuffer);
                bIsPass = true;
            };

            bindUi();

            const DrawPoly2DInstance::List& instances = inFrame.getInstances2D();
            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e2D, DrawPolyMode::Fill))
            {
                std::uint32_t       runStart        = draw.instanceStart;
                std::uint32_t       runCount        = 0;
                bool                bHasRunBackdrop = false;
                const std::uint32_t instanceEnd     = draw.instanceStart + draw.instanceCount;

                auto flush = [&]()
                {
                    if (runCount == 0)
                    {
                        return;
                    }
                    if (bHasRunBackdrop)
                    {
                        endPass();
                        rhi->commands->blitColor(
                            rhi->presentColor,
                            m_backdrops[rhi->frameIndex].image,
                            static_cast<std::int32_t>(viewport.position.x),
                            static_cast<std::int32_t>(viewport.position.y),
                            width,
                            height
                        );
                    }
                    beginPass();
                    rhi->commands->drawIndexed(draw.indexCount, runCount, draw.indexStart, draw.vertexStart, runStart);
                    runCount = 0;
                };

                for (std::uint32_t i = draw.instanceStart; i < instanceEnd; i++)
                {
                    const bool bHasBackdrop = i < instances.size() && instances.at(i).backdropBlur > 0.0f;
                    if (runCount == 0)
                    {
                        runStart        = i;
                        bHasRunBackdrop = bHasBackdrop;
                        runCount        = 1;
                        continue;
                    }
                    if (bHasBackdrop != bHasRunBackdrop || bHasBackdrop)
                    {
                        flush();
                        runStart        = i;
                        bHasRunBackdrop = bHasBackdrop;
                        runCount        = 1;
                        continue;
                    }
                    runCount++;
                }
                flush();
            }

            endPass();
        }
    }
}
