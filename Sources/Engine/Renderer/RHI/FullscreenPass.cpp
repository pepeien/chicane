#include "Chicane/Renderer/RHI/FullscreenPass.hpp"

#include <algorithm>

#include "Chicane/Renderer/RHI/Blur/Push.hpp"
#include "Chicane/Renderer/RHI/Hdr/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            void FullscreenPass::init(Device* inDevice)
            {
                m_device = inDevice;

                SamplerCreateInfo sampler;
                sampler.minFilter = SamplerFilter::Linear;
                sampler.magFilter = SamplerFilter::Linear;
                sampler.address   = SamplerAddress::ClampToEdge;
                m_sampler         = m_device->createSampler(sampler);

                BindGroupLayoutCreateInfo sampleLayout;
                Binding                   binding;
                binding.binding     = RHI_BINDING_POST_SOURCE;
                binding.type        = BindingType::SampledImage;
                binding.bIsVertex   = false;
                binding.bIsFragment = true;
                sampleLayout.bindings.push_back(binding);
                m_sampleLayout = m_device->createBindGroupLayout(sampleLayout);

                BindGroupLayoutCreateInfo compositeLayout;
                compositeLayout.bindings.push_back(binding);
                binding.binding = RHI_BINDING_POST_BLOOM;
                compositeLayout.bindings.push_back(binding);
                m_compositeLayout = m_device->createBindGroupLayout(compositeLayout);

                auto makePipeline =
                    [&](const String& inFragment, BindGroupLayout inLayout, std::uint32_t inPush, bool bPresent)
                {
                    PipelineCreateInfo desc;
                    desc.vertexPath       = "Assets/Engine/Shaders/Post/Fullscreen";
                    desc.fragmentPath     = inFragment;
                    desc.cull             = CullingMode::None;
                    desc.blend            = BlendMode::None;
                    desc.bHasColor        = true;
                    desc.bHasDepth        = false;
                    desc.colorFormat      = bPresent ? m_device->presentColorFormat() : m_device->sceneColorFormat();
                    desc.pushConstantSize = inPush;
                    desc.bHasPushFragment = inPush > 0;
                    desc.layouts.push_back(inLayout);
                    return m_device->createPipeline(desc);
                };

                m_extract = makePipeline("Assets/Engine/Shaders/Post/Extract", m_sampleLayout, 0, false);
                m_blur    = makePipeline("Assets/Engine/Shaders/Post/Blur", m_sampleLayout, sizeof(BlurPush), false);
                m_composite =
                    makePipeline("Assets/Engine/Shaders/Post/Composite", m_compositeLayout, sizeof(HdrPush), true);
            }

            void FullscreenPass::destroyImages()
            {
                if (!m_device)
                {
                    return;
                }

                for (int i = 0; i < 2; i++)
                {
                    if (m_bloom[i].handle)
                    {
                        m_device->destroyImage(m_bloom[i]);
                        m_bloom[i] = {};
                    }
                    if (m_blurGroup[i].handle)
                    {
                        m_device->destroyBindGroup(m_blurGroup[i]);
                        m_blurGroup[i] = {};
                    }
                }
                for (BindGroup& group : m_extractGroups)
                {
                    if (group.handle)
                    {
                        m_device->destroyBindGroup(group);
                    }
                }
                m_extractGroups.clear();
                for (BindGroup& group : m_compositeGroups)
                {
                    if (group.handle)
                    {
                        m_device->destroyBindGroup(group);
                    }
                }
                m_compositeGroups.clear();
            }

            void FullscreenPass::destroy()
            {
                if (!m_device)
                {
                    return;
                }

                destroyImages();
                m_device->destroyPipeline(m_extract);
                m_device->destroyPipeline(m_blur);
                m_device->destroyPipeline(m_composite);
                m_device->destroyBindGroupLayout(m_sampleLayout);
                m_device->destroyBindGroupLayout(m_compositeLayout);
                m_device->destroySampler(m_sampler);
                m_device = nullptr;
            }

            void FullscreenPass::resize(std::uint32_t inWidth, std::uint32_t inHeight, ImageFormat inFormat)
            {
                m_width  = std::max(1u, inWidth / 2u);
                m_height = std::max(1u, inHeight / 2u);
                destroyImages();

                ImageCreateInfo desc;
                desc.kind       = ImageKind::Color2D;
                desc.format     = inFormat;
                desc.width      = m_width;
                desc.height     = m_height;
                desc.bIsSampled = true;
                desc.bHasColor  = true;
                m_bloom[0]      = m_device->createImage(desc);
                m_bloom[1]      = m_device->createImage(desc);

                auto sample = [&](Image inImage)
                {
                    return std::vector<BindResource>{
                        {RHI_BINDING_POST_SOURCE, BindingType::SampledImage, {}, inImage, m_sampler, 0}
                    };
                };
                m_blurGroup[0] = m_device->createBindGroup(m_sampleLayout, sample(m_bloom[0]));
                m_blurGroup[1] = m_device->createBindGroup(m_sampleLayout, sample(m_bloom[1]));
            }

            void FullscreenPass::apply(
                CommandList*  inCommands,
                Image         inSceneColor,
                Image         inOutput,
                std::uint32_t inWidth,
                std::uint32_t inHeight,
                bool          bHdr,
                std::uint32_t inFrameIndex,
                bool          bBloom
            )
            {
                auto writeGroup = [&](std::vector<BindGroup>&          outGroups,
                                      BindGroupLayout                  inLayout,
                                      const std::vector<BindResource>& inResources)
                {
                    if (outGroups.size() <= inFrameIndex)
                    {
                        outGroups.resize(inFrameIndex + 1);
                    }
                    if (outGroups[inFrameIndex].handle)
                    {
                        m_device->updateBindGroup(outGroups[inFrameIndex], inResources);
                    }
                    else
                    {
                        outGroups[inFrameIndex] = m_device->createBindGroup(inLayout, inResources);
                    }
                };

                writeGroup(
                    m_extractGroups,
                    m_sampleLayout,
                    {
                        {RHI_BINDING_POST_SOURCE, BindingType::SampledImage, {}, inSceneColor, m_sampler, 0}
                }
                );
                writeGroup(
                    m_compositeGroups,
                    m_compositeLayout,
                    {
                        {RHI_BINDING_POST_SOURCE, BindingType::SampledImage, {}, inSceneColor, m_sampler, 0},
                        {RHI_BINDING_POST_BLOOM,  BindingType::SampledImage, {}, m_bloom[0],   m_sampler, 0}
                }
                );

                if (bBloom)
                {
                    PassCreateInfo extractPass;
                    extractPass.bHasColor   = true;
                    extractPass.color.image = m_bloom[0];
                    extractPass.color.load  = LoadOp::Clear;
                    extractPass.color.store = StoreOp::Store;
                    extractPass.width       = m_width;
                    extractPass.height      = m_height;
                    inCommands->beginPass(extractPass);
                    inCommands->bindPipeline(m_extract);
                    inCommands->bindGroup(0, m_extractGroups[inFrameIndex]);
                    inCommands->setViewport({Vec2(static_cast<float>(m_width), static_cast<float>(m_height))});
                    inCommands->setScissor({0, 0, m_width, m_height});
                    inCommands->draw(3, 1, 0, 0);
                    inCommands->endPass();

                    auto blur = [&](int src, int dst, float x, float y)
                    {
                        PassCreateInfo pass;
                        pass.bHasColor   = true;
                        pass.color.image = m_bloom[dst];
                        pass.color.load  = LoadOp::Clear;
                        pass.color.store = StoreOp::Store;
                        pass.width       = m_width;
                        pass.height      = m_height;
                        inCommands->beginPass(pass);
                        inCommands->bindPipeline(m_blur);
                        inCommands->bindGroup(0, m_blurGroup[src]);
                        BlurPush push;
                        push.direction[0] = x;
                        push.direction[1] = y;
                        inCommands->pushConstants(&push, sizeof(push));
                        inCommands->setViewport({Vec2(static_cast<float>(m_width), static_cast<float>(m_height))});
                        inCommands->setScissor({0, 0, m_width, m_height});
                        inCommands->draw(3, 1, 0, 0);
                        inCommands->endPass();
                    };

                    blur(0, 1, 1.0f, 0.0f);
                    blur(1, 0, 0.0f, 1.0f);
                    blur(0, 1, 1.0f, 0.0f);
                    blur(1, 0, 0.0f, 1.0f);
                }

                PassCreateInfo compositePass;
                compositePass.bHasColor   = true;
                compositePass.color.image = inOutput;
                compositePass.color.load  = LoadOp::Clear;
                compositePass.color.store = StoreOp::Store;
                compositePass.width       = inWidth;
                compositePass.height      = inHeight;
                inCommands->beginPass(compositePass);
                inCommands->bindPipeline(m_composite);
                inCommands->bindGroup(0, m_compositeGroups[inFrameIndex]);
                HdrPush hdr;
                hdr.enabled = bHdr ? 1 : 0;
                hdr.bloom   = bBloom ? 1 : 0;
                inCommands->pushConstants(&hdr, sizeof(hdr));
                inCommands->setViewport({Vec2(static_cast<float>(inWidth), static_cast<float>(inHeight))});
                inCommands->setScissor({0, 0, inWidth, inHeight});
                inCommands->draw(3, 1, 0, 0);
                inCommands->endPass();
            }
        }
    }
}
