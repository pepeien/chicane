#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include <glad/gl.h>

#include "Chicane/Renderer/Backend/OpenGL/RHI/BindGroup/Data.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/BindGroup/Layout/Data.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/Buffer/Data.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/Image/Data.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/Pipeline/Data.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/Sampler/Data.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class OpenGLBackend;
        class OpenGLRHICommandList;

        namespace RHI
        {
            class CommandList;
        }

        class CHICANE_RENDERER OpenGLRHIDevice : public RHI::Device
        {
        public:
            explicit OpenGLRHIDevice(OpenGLBackend* inBackend);
            ~OpenGLRHIDevice() override;

            RHI::Buffer createBuffer(const RHI::BufferCreateInfo& inCreateInfo) override;
            void destroyBuffer(RHI::Buffer inBuffer) override;
            void updateBuffer(
                RHI::Buffer inBuffer, const void* inData, std::size_t inSize, std::size_t inOffset = 0
            ) override;

            RHI::Image createImage(const RHI::ImageCreateInfo& inCreateInfo) override;
            void destroyImage(RHI::Image inImage) override;
            void updateImage(
                RHI::Image    inImage,
                const void*   inData,
                std::uint32_t inWidth,
                std::uint32_t inHeight,
                std::uint32_t inLayer = 0
            ) override;
            void generateMips(RHI::Image inImage) override;
            RHI::Image createImageView(RHI::Image inImage, std::uint32_t inLayer) override;

            RHI::Sampler createSampler(const RHI::SamplerCreateInfo& inCreateInfo) override;
            void destroySampler(RHI::Sampler inSampler) override;

            RHI::BindGroupLayout createBindGroupLayout(const RHI::BindGroupLayoutCreateInfo& inCreateInfo) override;
            void destroyBindGroupLayout(RHI::BindGroupLayout inLayout) override;

            RHI::BindGroup createBindGroup(
                RHI::BindGroupLayout inLayout, const std::vector<RHI::BindResource>& inResources
            ) override;
            void updateBindGroup(RHI::BindGroup inGroup, const std::vector<RHI::BindResource>& inResources) override;
            void destroyBindGroup(RHI::BindGroup inGroup) override;

            RHI::Pipeline createPipeline(const RHI::PipelineCreateInfo& inCreateInfo) override;
            void destroyPipeline(RHI::Pipeline inPipeline) override;

            String shaderPath(const String& inLogical, ShaderType inType) const override;
            RHI::ImageFormat sceneColorFormat() const override;
            RHI::ImageFormat sceneDepthFormat() const override;
            RHI::ImageFormat presentColorFormat() const override;

            RHI::Image wrapTexture(
                std::uint32_t    inTexture,
                RHI::ImageKind   inKind,
                RHI::ImageFormat inFormat,
                std::uint32_t    inWidth,
                std::uint32_t    inHeight,
                std::uint32_t    inFbo = 0
            );
            RHI::Buffer wrapBuffer(std::uint32_t inId, std::size_t inSize, RHI::BufferUsage inUsage);
            RHI::CommandList* commandList();
            OpenGLBackend* backend() const { return m_backend; }
            std::uint32_t compileProgram(const String& inVertex, const String& inFragment) const;

        private:
            OpenGLBackend*                        m_backend;
            std::unique_ptr<OpenGLRHICommandList> m_commands;
        };
    }
}
