#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout/CreateInfo.hpp"
#include "Chicane/Renderer/RHI/BindResource.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Image/CreateInfo.hpp"
#include "Chicane/Renderer/RHI/Image/Format.hpp"
#include "Chicane/Renderer/RHI/Pipeline.hpp"
#include "Chicane/Renderer/RHI/Pipeline/CreateInfo.hpp"
#include "Chicane/Renderer/RHI/Sampler.hpp"
#include "Chicane/Renderer/RHI/Sampler/CreateInfo.hpp"
#include "Chicane/Renderer/Shader/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            class CHICANE_RENDERER Device
            {
            public:
                virtual ~Device() = default;

            public:
                virtual Buffer createBuffer(const BufferCreateInfo& inCreateInfo) = 0;
                virtual void destroyBuffer(Buffer inBuffer)                       = 0;
                virtual void updateBuffer(
                    Buffer inBuffer, const void* inData, std::size_t inSize, std::size_t inOffset = 0
                ) = 0;

                virtual Image createImage(const ImageCreateInfo& inCreateInfo) = 0;
                virtual void destroyImage(Image inImage)                       = 0;
                virtual void updateImage(
                    Image         inImage,
                    const void*   inData,
                    std::uint32_t inWidth,
                    std::uint32_t inHeight,
                    std::uint32_t inLayer = 0
                )                                                                   = 0;
                virtual void generateMips(Image inImage)                            = 0;
                virtual Image createImageView(Image inImage, std::uint32_t inLayer) = 0;

                virtual Sampler createSampler(const SamplerCreateInfo& inCreateInfo) = 0;
                virtual void destroySampler(Sampler inSampler)                       = 0;

                virtual BindGroupLayout createBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo) = 0;
                virtual void destroyBindGroupLayout(BindGroupLayout inLayout)                                = 0;

                virtual BindGroup createBindGroup(
                    BindGroupLayout inLayout, const std::vector<BindResource>& inResources
                )                                                                                             = 0;
                virtual void updateBindGroup(BindGroup inGroup, const std::vector<BindResource>& inResources) = 0;
                virtual void destroyBindGroup(BindGroup inGroup)                                              = 0;

                virtual Pipeline createPipeline(const PipelineCreateInfo& inCreateInfo) = 0;
                virtual void destroyPipeline(Pipeline inPipeline)                       = 0;

                virtual String shaderPath(const String& inLogical, ShaderType inType) const = 0;
                virtual ImageFormat sceneColorFormat() const                                = 0;
                virtual ImageFormat sceneDepthFormat() const                                = 0;
                virtual ImageFormat presentColorFormat() const                              = 0;
            };
        }
    }
}
