#include "Chicane/Renderer/Layer/Scene/Sky.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Sky/Kind.hpp"
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

        static void cubeFaceDirection(std::uint32_t inFace, float inU, float inV, float& outX, float& outY, float& outZ)
        {
            const float sc = inU * 2.0f - 1.0f;
            const float tc = inV * 2.0f - 1.0f;

            switch (inFace)
            {
            case 0:
                outX = 1.0f;
                outY = -tc;
                outZ = -sc;
                break;
            case 1:
                outX = -1.0f;
                outY = -tc;
                outZ = sc;
                break;
            case 2:
                outX = sc;
                outY = 1.0f;
                outZ = tc;
                break;
            case 3:
                outX = sc;
                outY = -1.0f;
                outZ = -tc;
                break;
            case 4:
                outX = sc;
                outY = -tc;
                outZ = 1.0f;
                break;
            default:
                outX = -sc;
                outY = -tc;
                outZ = -1.0f;
                break;
            }
        }

        static void samplePanorama(
            const std::vector<float>& inPixels,
            std::uint32_t             inWidth,
            std::uint32_t             inHeight,
            float                     inX,
            float                     inY,
            float                     inZ,
            float*                    outRgba
        )
        {
            const float length = std::sqrt(inX * inX + inY * inY + inZ * inZ);
            if (length <= 0.00000001f || inWidth == 0 || inHeight == 0 || inPixels.empty())
            {
                outRgba[0] = 0.0f;
                outRgba[1] = 0.0f;
                outRgba[2] = 0.0f;
                outRgba[3] = 1.0f;

                return;
            }

            const float x     = inX / length;
            const float y     = inY / length;
            const float z     = inZ / length;
            const float yaw   = std::atan2(x, y);
            const float pitch = std::asin(std::clamp(z, -1.0f, 1.0f));
            float       u     = yaw * 0.15915494309f + 0.5f;
            float       v     = 0.5f - pitch * 0.31830988618f;
            u                 = u - std::floor(u);
            v                 = std::clamp(v, 0.0f, 1.0f);

            const float fx = u * static_cast<float>(inWidth - 1);
            const float fy = v * static_cast<float>(inHeight - 1);
            const int   x0 = static_cast<int>(fx);
            const int   y0 = static_cast<int>(fy);
            const int   x1 = std::min(x0 + 1, static_cast<int>(inWidth - 1));
            const int   y1 = std::min(y0 + 1, static_cast<int>(inHeight - 1));
            const float tx = fx - static_cast<float>(x0);
            const float ty = fy - static_cast<float>(y0);

            auto pixel = [&inPixels, inWidth](int inX, int inY) -> const float*
            {
                return inPixels.data() + (static_cast<std::size_t>(inY) * inWidth + static_cast<std::size_t>(inX)) * 4u;
            };

            const float* a = pixel(x0, y0);
            const float* b = pixel(x1, y0);
            const float* c = pixel(x0, y1);
            const float* d = pixel(x1, y1);
            for (int channel = 0; channel < 4; channel++)
            {
                const float top    = a[channel] + (b[channel] - a[channel]) * tx;
                const float bottom = c[channel] + (d[channel] - c[channel]) * tx;
                outRgba[channel]   = top + (bottom - top) * ty;
            }
        }

        static void fillCubeFaceFromPanorama(
            const std::vector<float>& inPanorama, std::vector<std::uint16_t>& outLinear, std::uint32_t inFace
        )
        {
            for (std::uint32_t y = 0; y < SKY_TEXTURE_SIZE; y++)
            {
                const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(SKY_TEXTURE_SIZE);
                for (std::uint32_t x = 0; x < SKY_TEXTURE_SIZE; x++)
                {
                    const float u  = (static_cast<float>(x) + 0.5f) / static_cast<float>(SKY_TEXTURE_SIZE);
                    float       dx = 0.0f;
                    float       dy = 0.0f;
                    float       dz = 0.0f;
                    cubeFaceDirection(inFace, u, v, dx, dy, dz);

                    float sample[4] = {};
                    samplePanorama(inPanorama, SKY_PANORAMA_WIDTH, SKY_PANORAMA_HEIGHT, dx, dy, dz, sample);

                    const std::size_t offset = (static_cast<std::size_t>(y) * SKY_TEXTURE_SIZE + x) * 4u;
                    outLinear[offset]        = floatToHalf(sample[0]);
                    outLinear[offset + 1]    = floatToHalf(sample[1]);
                    outLinear[offset + 2]    = floatToHalf(sample[2]);
                    outLinear[offset + 3]    = floatToHalf(sample[3]);
                }
            }
        }

        static void fillHalfImage(
            const Image::Instance&      inImage,
            std::vector<std::uint16_t>& outLinear,
            std::uint32_t               inWidth,
            std::uint32_t               inHeight
        )
        {
            if (!inImage || inImage->getWidth() <= 0 || inImage->getHeight() <= 0 ||
                (inImage->getPixels() == nullptr && inImage->getFloatPixels() == nullptr))
            {
                std::fill(outLinear.begin(), outLinear.end(), 0);

                return;
            }

            if (inImage->isHdr())
            {
                std::vector<float> linear(outLinear.size());
                inImage->blitFloat(linear.data(), static_cast<int>(inWidth), static_cast<int>(inHeight));
                for (std::size_t index = 0; index < outLinear.size(); index++)
                {
                    outLinear[index] = floatToHalf(linear[index]);
                }

                return;
            }

            std::vector<unsigned char> srgb(outLinear.size());
            inImage->blit(srgb.data(), static_cast<int>(inWidth), static_cast<int>(inHeight));
            for (std::size_t index = 0; index < outLinear.size(); index++)
            {
                const float channel = static_cast<float>(srgb[index]) / 255.0f;
                const bool  bAlpha  = (index % 4u) == 3u;
                outLinear[index]    = floatToHalf(bAlpha ? channel : srgbToLinear(channel));
            }
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

            desc.fragmentPath  = "Assets/Engine/Shaders/Scene/SkyPanorama";
            m_panoramaPipeline = device->createPipeline(desc);

            LScene* parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            m_textureGroup = device->createBindGroup(
                m_textureLayout,
                {
                    {0, RHI::BindingType::SampledImage, {}, parent->skyImage, parent->skySampler, 0}
            }
            );
            m_panoramaGroup = device->createBindGroup(
                m_textureLayout,
                {
                    {0, RHI::BindingType::SampledImage, {}, parent->skyPanoramaImage, parent->skyPanoramaSampler, 0}
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
            device->destroyBindGroup(m_panoramaGroup);
            device->destroyBindGroup(m_textureGroup);
            device->destroyPipeline(m_panoramaPipeline);
            device->destroyPipeline(m_pipeline);
            device->destroyBindGroupLayout(m_frameLayout);
            device->destroyBindGroupLayout(m_textureLayout);
        }

        void LSceneSky::onLoad(const DrawSkyResource& inResource)
        {
            if (inResource.isEmpty())
            {
                m_bHasSky     = false;
                m_bIsPanorama = false;
                return;
            }

            LScene*        parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            RHI::Device*   device = m_backend->getRHIDevice();
            const DrawSky& sky    = inResource.getDraw();
            m_bIsPanorama         = sky.kind == DrawSkyKind::Panorama;

            if (m_bIsPanorama)
            {
                const Image::Instance image =
                    sky.textures.empty() ? Image::Instance() : sky.textures.front().getSampleImage();

                std::vector<std::uint16_t> panorama(
                    static_cast<std::size_t>(SKY_PANORAMA_WIDTH) * SKY_PANORAMA_HEIGHT * 4u
                );
                fillHalfImage(image, panorama, SKY_PANORAMA_WIDTH, SKY_PANORAMA_HEIGHT);
                device->updateImage(parent->skyPanoramaImage, panorama.data(), SKY_PANORAMA_WIDTH, SKY_PANORAMA_HEIGHT);
                device->updateBindGroup(
                    m_panoramaGroup,
                    {
                        {0, RHI::BindingType::SampledImage, {}, parent->skyPanoramaImage, parent->skyPanoramaSampler, 0}
                }
                );

                std::vector<float> source(
                    static_cast<std::size_t>(SKY_PANORAMA_WIDTH) * SKY_PANORAMA_HEIGHT * 4u,
                    0.0f
                );
                if (image)
                {
                    image->blitFloat(
                        source.data(),
                        static_cast<int>(SKY_PANORAMA_WIDTH),
                        static_cast<int>(SKY_PANORAMA_HEIGHT)
                    );
                }

                std::vector<std::uint16_t> face(static_cast<std::size_t>(SKY_TEXTURE_SIZE) * SKY_TEXTURE_SIZE * 4u);
                for (std::uint32_t index = 0; index < 6; index++)
                {
                    fillCubeFaceFromPanorama(source, face, index);
                    device->updateImage(parent->skyImage, face.data(), SKY_TEXTURE_SIZE, SKY_TEXTURE_SIZE, index);
                }
                device->generateMips(parent->skyImage);
                device->updateBindGroup(
                    m_textureGroup,
                    {
                        {0, RHI::BindingType::SampledImage, {}, parent->skyImage, parent->skySampler, 0}
                }
                );
            }
            else
            {
                std::vector<std::uint16_t> linear(static_cast<std::size_t>(SKY_TEXTURE_SIZE) * SKY_TEXTURE_SIZE * 4u);
                std::uint32_t              face = 0;
                for (const DrawTexture& texture : sky.textures)
                {
                    fillHalfImage(texture.getSampleImage(), linear, SKY_TEXTURE_SIZE, SKY_TEXTURE_SIZE);
                    device->updateImage(parent->skyImage, linear.data(), SKY_TEXTURE_SIZE, SKY_TEXTURE_SIZE, face);
                    face++;
                }
                device->generateMips(parent->skyImage);
                device->updateBindGroup(
                    m_textureGroup,
                    {
                        {0, RHI::BindingType::SampledImage, {}, parent->skyImage, parent->skySampler, 0}
                }
                );
            }

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

            RHI::PassCreateInfo pass = rhiScenePass(*rhi, false, RHI::LoadOp::Clear, RHI::LoadOp::DontCare);
            if (!inFrame.getSkyInstance().bVisible)
            {
                pass.color.clear = Vec4::sZero();
            }
            rhi->commands->beginPass(pass);
            rhiApplyView(rhi->commands, m_backend, this);

            const DrawPoly& draw = inFrame.getSkyInstance().model;
            if (m_bHasSky && inFrame.getSkyInstance().bVisible && draw.id > Draw::InvalidId)
            {
                rhi->commands->bindPipeline(m_bIsPanorama ? m_panoramaPipeline : m_pipeline);
                rhi->commands->bindGroup(0, m_frameGroups[rhi->frameIndex]);
                rhi->commands->bindGroup(1, m_bIsPanorama ? m_panoramaGroup : m_textureGroup);
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
