#include "Chicane/Renderer/Layer/Scene/Sky.hpp"

#include <cmath>
#include <cstdint>
#include <vector>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/RHI/Sky/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static float srgbToLinear(float inValue)
        {
            if (inValue <= 0.04045f)
            {
                return inValue / 12.92f;
            }

            return std::pow((inValue + 0.055f) / 1.055f, 2.4f);
        }

        static std::uint16_t floatToHalf(float inValue)
        {
            union
            {
                float         f;
                std::uint32_t u;
            } bits{inValue};

            const std::uint32_t sign = (bits.u >> 16u) & 0x8000u;
            std::int32_t        exp  = static_cast<std::int32_t>((bits.u >> 23u) & 0xFFu) - 127 + 15;
            std::uint32_t       mant = bits.u & 0x7FFFFFu;

            if (exp <= 0)
            {
                return static_cast<std::uint16_t>(sign);
            }

            if (exp >= 31)
            {
                return static_cast<std::uint16_t>(sign | 0x7C00u);
            }

            return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(exp) << 10u) | (mant >> 13u));
        }
        LSceneSky::LSceneSky()
            : Layer(SCENE_SKY_LAYER_ID)
        {}

        void LSceneSky::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BindGroupLayoutCreateInfo frame;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_CAMERA;
            binding.type        = RHI::BindingType::UniformBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            frame.bindings.push_back(binding);
            m_frameLayout = device->createBindGroupLayout(frame);

            RHI::BindGroupLayoutCreateInfo texture;
            binding.binding     = 0;
            binding.type        = RHI::BindingType::SampledImage;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            texture.bindings.push_back(binding);
            m_textureLayout = device->createBindGroupLayout(texture);

            RHI::PipelineCreateInfo desc;
            desc.vertexPath       = "Assets/Engine/Shaders/Scene/Sky";
            desc.fragmentPath     = "Assets/Engine/Shaders/Scene/Sky";
            desc.vertexStride     = sizeof(Vertex);
            desc.vertexAttributes = rhiPoly3DPositionAttributes();
            desc.cull             = CullingMode::Front;
            desc.frontFace        = CullingFrontFace::CounterClockwise;
            desc.blend            = RHI::BlendMode::None;
            desc.bHasColor        = true;
            desc.bHasDepth        = false;
            desc.colorFormat      = device->sceneColorFormat();
            desc.pushConstantSize = sizeof(RHI::SkyPush);
            desc.bHasPushFragment = true;
            desc.layouts          = {m_frameLayout, m_textureLayout};
            m_pipeline            = device->createPipeline(desc);

            LScene* parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            m_textureGroup = device->createBindGroup(
                m_textureLayout,
                {
                    {0, RHI::BindingType::SampledImage, {}, parent->skyImage, parent->skySampler, 0}
            }
            );
        }

        void LSceneSky::onDestruction()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            for (RHI::BindGroup group : m_frameGroups)
            {
                device->destroyBindGroup(group);
            }
            device->destroyBindGroup(m_textureGroup);
            device->destroyPipeline(m_pipeline);
            device->destroyBindGroupLayout(m_frameLayout);
            device->destroyBindGroupLayout(m_textureLayout);
        }

        void LSceneSky::onLoad(const DrawSkyResource& inResource)
        {
            if (inResource.isEmpty())
            {
                m_bHasSky = false;
                return;
            }

            LScene*        parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            RHI::Device*   device = m_backend->getRHIDevice();
            const DrawSky& sky    = inResource.getDraw();

            std::vector<unsigned char> srgb(static_cast<std::size_t>(SKY_TEXTURE_SIZE) * SKY_TEXTURE_SIZE * 4u);
            std::vector<std::uint16_t> linear(srgb.size());
            std::uint32_t              face = 0;
            for (const DrawTexture& texture : sky.textures)
            {
                if (const Image::Instance image = texture.getSampleImage())
                {
                    image->blit(srgb.data(), static_cast<int>(SKY_TEXTURE_SIZE), static_cast<int>(SKY_TEXTURE_SIZE));
                    for (std::size_t index = 0; index < linear.size(); index++)
                    {
                        const float channel = static_cast<float>(srgb[index]) / 255.0f;
                        const bool  bAlpha  = (index % 4u) == 3u;
                        linear[index]       = floatToHalf(bAlpha ? channel : srgbToLinear(channel));
                    }
                    device->updateImage(parent->skyImage, linear.data(), SKY_TEXTURE_SIZE, SKY_TEXTURE_SIZE, face);
                }
                face++;
            }
            device->generateMips(parent->skyImage);
            device->updateBindGroup(
                m_textureGroup,
                {
                    {0, RHI::BindingType::SampledImage, {}, parent->skyImage, parent->skySampler, 0}
            }
            );
            m_bHasSky = true;
        }

        bool LSceneSky::onBeginRender(const Frame&)
        {
            return true;
        }

        void LSceneSky::onRender(const Frame& inFrame, void* inData)
        {
            RHI::Frame*  rhi    = static_cast<RHI::Frame*>(inData);
            LScene*      parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            RHI::Device* device = m_backend->getRHIDevice();

            rhiReplaceGroup(
                device,
                m_frameGroups,
                rhi->frameIndex,
                m_frameLayout,
                {
                    {RHI_BINDING_CAMERA, RHI::BindingType::UniformBuffer, rhi->cameraBuffer, {}, {}, 0}
            }
            );

            rhi->commands->beginPass(rhiScenePass(*rhi, false, RHI::LoadOp::Clear, RHI::LoadOp::DontCare));
            rhiApplyView(rhi->commands, m_backend, this);

            const DrawPoly& draw = inFrame.getSkyInstance().model;
            if (m_bHasSky && draw.id > Draw::InvalidId)
            {
                rhi->commands->bindPipeline(m_pipeline);
                rhi->commands->bindGroup(0, m_frameGroups[rhi->frameIndex]);
                rhi->commands->bindGroup(1, m_textureGroup);
                RHI::SkyPush push;
                push.intensity = inFrame.getSkyInstance().exposure;
                rhi->commands->pushConstants(&push, sizeof(push));
                rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
                rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);
                rhi->commands->drawIndexed(draw.indexCount, 1, draw.indexStart, draw.vertexStart, 0);
            }

            rhi->commands->endPass();
        }
    }
}
