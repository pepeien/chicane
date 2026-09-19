#include "Chicane/Renderer/Backend/OpenGL/RHI/CommandList.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLRHICommandList::OpenGLRHICommandList(OpenGLRHIDevice* inDevice)
            : m_device(inDevice)
        {}

        void OpenGLRHICommandList::applyPipelineState(const RHI::PipelineCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.bHasDepthTest)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthMask(inCreateInfo.bHasDepthWrite ? GL_TRUE : GL_FALSE);
                GLenum depthFunc = GL_LESS;
                switch (inCreateInfo.depthCompare)
                {
                case DepthCompare::LessOrEqual:
                    depthFunc = GL_LEQUAL;
                    break;
                case DepthCompare::Always:
                    depthFunc = GL_ALWAYS;
                    break;
                case DepthCompare::NotEqual:
                    depthFunc = GL_NOTEQUAL;
                    break;
                default:
                    break;
                }
                glDepthFunc(depthFunc);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }

            if (inCreateInfo.cull == CullingMode::None)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glCullFace(inCreateInfo.cull == CullingMode::Front ? GL_FRONT : GL_BACK);
                glFrontFace(inCreateInfo.frontFace == CullingFrontFace::Clockwise ? GL_CW : GL_CCW);
            }

            glPolygonMode(GL_FRONT_AND_BACK, inCreateInfo.fill == RHI::FillMode::Line ? GL_LINE : GL_FILL);

            if (inCreateInfo.blend == RHI::BlendMode::None)
            {
                glDisable(GL_BLEND);
            }
            else
            {
                glEnable(GL_BLEND);
                if (inCreateInfo.blend == RHI::BlendMode::Additive)
                {
                    glBlendFunc(GL_ONE, GL_ONE);
                }
                else
                {
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
            }

            glColorMask(
                inCreateInfo.bHasColorWrite ? GL_TRUE : GL_FALSE,
                inCreateInfo.bHasColorWrite ? GL_TRUE : GL_FALSE,
                inCreateInfo.bHasColorWrite ? GL_TRUE : GL_FALSE,
                inCreateInfo.bHasColorWrite ? GL_TRUE : GL_FALSE
            );

            if (inCreateInfo.stencil == RHI::StencilMode::None)
            {
                glDisable(GL_STENCIL_TEST);
            }
            else
            {
                glEnable(GL_STENCIL_TEST);
                if (inCreateInfo.stencil == RHI::StencilMode::WriteReplace)
                {
                    glStencilMask(0xFF);
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                }
                else
                {
                    glStencilMask(0x00);
                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                }
            }
        }

        void OpenGLRHICommandList::applyGroup(const OpenGLRHIGroupData* inGroup)
        {
            for (const RHI::BindResource& resource : inGroup->resources)
            {
                if (resource.type == RHI::BindingType::SampledImage)
                {
                    auto* image   = static_cast<OpenGLRHIImageData*>(resource.image.handle);
                    auto* sampler = static_cast<OpenGLRHISamplerData*>(resource.sampler.handle);
                    glBindTextureUnit(resource.binding, image->texture);
                    if (sampler)
                    {
                        GLenum minFilter = GL_LINEAR;
                        if (sampler->createInfo.bHasMip)
                        {
                            minFilter = sampler->createInfo.minFilter == RHI::SamplerFilter::Nearest
                                            ? GL_NEAREST_MIPMAP_LINEAR
                                            : GL_LINEAR_MIPMAP_LINEAR;
                        }
                        else if (sampler->createInfo.minFilter == RHI::SamplerFilter::Nearest)
                        {
                            minFilter = GL_NEAREST;
                        }
                        const GLenum magFilter =
                            sampler->createInfo.magFilter == RHI::SamplerFilter::Nearest ? GL_NEAREST : GL_LINEAR;
                        glTextureParameteri(image->texture, GL_TEXTURE_MIN_FILTER, minFilter);
                        glTextureParameteri(image->texture, GL_TEXTURE_MAG_FILTER, magFilter);
                        glTextureParameteri(image->texture, GL_TEXTURE_MAX_LOD, sampler->createInfo.bHasMip ? 16 : 0);
                        GLenum wrap = GL_CLAMP_TO_EDGE;
                        if (sampler->createInfo.address == RHI::SamplerAddress::ClampToBorder)
                        {
                            wrap = GL_CLAMP_TO_BORDER;
                        }
                        else if (sampler->createInfo.address == RHI::SamplerAddress::Repeat)
                        {
                            wrap = GL_REPEAT;
                        }
                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_S, wrap);
                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_T, wrap);
                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_R, wrap);
                    }
                }
                else
                {
                    auto*        buffer = static_cast<OpenGLRHIBufferData*>(resource.buffer.handle);
                    const GLenum target =
                        resource.type == RHI::BindingType::StorageBuffer ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER;
                    glBindBufferBase(target, resource.binding, buffer->id);
                }
            }
        }

        void OpenGLRHICommandList::beginPass(const RHI::PassCreateInfo& inCreateInfo)
        {
            bool bDefault = false;
            if (inCreateInfo.bHasColor)
            {
                auto* color = static_cast<OpenGLRHIImageData*>(inCreateInfo.color.image.handle);
                if (!color || (!color->texture && !color->fbo))
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glDrawBuffer(GL_BACK);
                    bDefault = true;
                }
                else if (!color->bOwned && color->fbo)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, color->fbo);
                }
                else
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, color->fbo);
                    if (color->kind == RHI::ImageKind::Color2D || color->kind == RHI::ImageKind::Cube)
                    {
                        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color->texture, 0);
                        glDrawBuffer(GL_COLOR_ATTACHMENT0);
                    }
                }
            }
            if (!bDefault && inCreateInfo.bHasDepth)
            {
                auto* depth = static_cast<OpenGLRHIImageData*>(inCreateInfo.depth.image.handle);
                if (!inCreateInfo.bHasColor)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, depth->fbo);
                    glDrawBuffer(GL_NONE);
                    glReadBuffer(GL_NONE);
                }
                const bool bOwnedColor =
                    inCreateInfo.bHasColor && static_cast<OpenGLRHIImageData*>(inCreateInfo.color.image.handle)->bOwned;
                if (!inCreateInfo.bHasColor || bOwnedColor)
                {
                    if (depth->kind == RHI::ImageKind::Depth2DArray || depth->layer > 0 || depth->bView)
                    {
                        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->texture, 0, depth->layer);
                    }
                    else if (depth->texture)
                    {
                        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->texture, 0);
                    }
                }
            }

            GLbitfield mask = 0;
            if (inCreateInfo.bHasColor && inCreateInfo.color.load == RHI::LoadOp::Clear)
            {
                glClearColor(
                    inCreateInfo.color.clear.x,
                    inCreateInfo.color.clear.y,
                    inCreateInfo.color.clear.z,
                    inCreateInfo.color.clear.w
                );
                mask |= GL_COLOR_BUFFER_BIT;
            }
            if (inCreateInfo.bHasDepth && inCreateInfo.depth.load == RHI::LoadOp::Clear)
            {
                glDepthMask(GL_TRUE);
                glStencilMask(0xFF);
                glClearDepth(1.0);
                mask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
            }
            if (mask)
            {
                glClear(mask);
            }

            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
        }

        void OpenGLRHICommandList::endPass()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            glStencilMask(0xFF);
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_SCISSOR_TEST);
        }

        void OpenGLRHICommandList::bindPipeline(RHI::Pipeline inPipeline)
        {
            m_pipeline = static_cast<OpenGLRHIPipelineData*>(inPipeline.handle);
            m_topology = m_pipeline->createInfo.topology;
            glUseProgram(m_pipeline->program);
            glBindVertexArray(m_pipeline->vao);
            applyPipelineState(m_pipeline->createInfo);
        }

        void OpenGLRHICommandList::bindGroup(std::uint32_t, RHI::BindGroup inGroup)
        {
            applyGroup(static_cast<OpenGLRHIGroupData*>(inGroup.handle));
        }

        void OpenGLRHICommandList::bindVertexBuffer(RHI::Buffer inBuffer)
        {
            auto* buffer = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
            glVertexArrayVertexBuffer(
                m_pipeline->vao,
                0,
                buffer->id,
                0,
                static_cast<GLsizei>(m_pipeline->createInfo.vertexStride)
            );
        }

        void OpenGLRHICommandList::bindIndexBuffer(RHI::Buffer inBuffer)
        {
            auto* buffer  = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
            m_indexBuffer = buffer->id;
            glVertexArrayElementBuffer(m_pipeline->vao, buffer->id);
        }

        void OpenGLRHICommandList::pushConstants(const void* inData, std::uint32_t inSize)
        {
            glNamedBufferSubData(m_pipeline->pushUbo, 0, inSize, inData);
            glBindBufferBase(GL_UNIFORM_BUFFER, RHI_BINDING_PUSH_CONSTANTS, m_pipeline->pushUbo);
        }

        static GLenum toGLTopology(RHI::PrimitiveTopology inTopology)
        {
            if (inTopology == RHI::PrimitiveTopology::TriangleStrip)
            {
                return GL_TRIANGLE_STRIP;
            }
            if (inTopology == RHI::PrimitiveTopology::LineList)
            {
                return GL_LINES;
            }
            return GL_TRIANGLES;
        }

        void OpenGLRHICommandList::draw(
            std::uint32_t inVertexCount,
            std::uint32_t inInstanceCount,
            std::uint32_t inFirstVertex,
            std::uint32_t inFirstInstance
        )
        {
            glDrawArraysInstancedBaseInstance(
                toGLTopology(m_topology),
                static_cast<GLint>(inFirstVertex),
                static_cast<GLsizei>(inVertexCount),
                static_cast<GLsizei>(inInstanceCount),
                inFirstInstance
            );
        }

        void OpenGLRHICommandList::drawIndexed(
            std::uint32_t inIndexCount,
            std::uint32_t inInstanceCount,
            std::uint32_t inFirstIndex,
            std::int32_t  inVertexOffset,
            std::uint32_t inFirstInstance
        )
        {
            glDrawElementsInstancedBaseVertexBaseInstance(
                toGLTopology(m_topology),
                static_cast<GLsizei>(inIndexCount),
                GL_UNSIGNED_INT,
                reinterpret_cast<void*>(sizeof(std::uint32_t) * inFirstIndex),
                static_cast<GLsizei>(inInstanceCount),
                inVertexOffset,
                inFirstInstance
            );
        }

        void OpenGLRHICommandList::setViewport(const RHI::Viewport& inViewport)
        {
            glViewport(
                static_cast<GLint>(inViewport.position.x),
                static_cast<GLint>(inViewport.position.y),
                static_cast<GLsizei>(inViewport.size.x),
                static_cast<GLsizei>(inViewport.size.y)
            );
        }

        void OpenGLRHICommandList::setScissor(const RHI::Scissor& inScissor)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(
                inScissor.x,
                inScissor.y,
                static_cast<GLsizei>(inScissor.width),
                static_cast<GLsizei>(inScissor.height)
            );
        }

        void OpenGLRHICommandList::setLineWidth(float inWidth)
        {
            glLineWidth(inWidth);
        }

        void OpenGLRHICommandList::blitColor(
            RHI::Image    inSource,
            RHI::Image    inDestination,
            std::int32_t  inX,
            std::int32_t  inY,
            std::uint32_t inWidth,
            std::uint32_t inHeight
        )
        {
            auto* source = static_cast<OpenGLRHIImageData*>(inSource.handle);
            auto* dest   = static_cast<OpenGLRHIImageData*>(inDestination.handle);
            if (!source || !dest)
            {
                return;
            }

            GLint previous = 0;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previous);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, source->fbo);
            glReadBuffer(source->fbo == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->fbo);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glBlitFramebuffer(
                inX,
                inY,
                inX + static_cast<std::int32_t>(inWidth),
                inY + static_cast<std::int32_t>(inHeight),
                0,
                0,
                static_cast<GLint>(inWidth),
                static_cast<GLint>(inHeight),
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST
            );
            glBindFramebuffer(GL_FRAMEBUFFER, previous);
            if (dest->texture)
            {
                glGenerateTextureMipmap(dest->texture);
            }
            if (previous == 0)
            {
                glReadBuffer(GL_BACK);
                glDrawBuffer(GL_BACK);
            }
        }

        void OpenGLRHICommandList::prepareShaderRead(RHI::Image)
        {}

        void OpenGLRHICommandList::preparePresent(RHI::Image)
        {}
    }
}
