#include "Backend/OpenGL/RHI/CommandList.hpp"

#include "Backend/OpenGL.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Renderer
    {
        OpenGLRHICommandList::OpenGLRHICommandList(OpenGLRHIDevice* inDevice)
            : m_device(inDevice)
        {}

        void OpenGLRHICommandList::applyPipelineState(const RHI::PipelineCreateInfo& inCreateInfo)
        {
            const bool bDepthTest = static_cast<bool>(inCreateInfo.bHasDepthTest);

            if (bDepthTest)
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

            if (!bDepthTest)
            {
                glDisable(GL_DEPTH_TEST);
            }

            const bool bCullNone = static_cast<bool>(inCreateInfo.cull == CullingMode::None);

            if (bCullNone)
            {
                glDisable(GL_CULL_FACE);
            }

            if (!bCullNone)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(inCreateInfo.cull == CullingMode::Front ? GL_FRONT : GL_BACK);
                glFrontFace(inCreateInfo.frontFace == CullingFrontFace::Clockwise ? GL_CW : GL_CCW);
            }

            glPolygonMode(GL_FRONT_AND_BACK, inCreateInfo.fill == RHI::FillMode::Line ? GL_LINE : GL_FILL);
            const bool bFillLine = static_cast<bool>(inCreateInfo.fill == RHI::FillMode::Line);

            if (bFillLine)
            {
                glEnable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(-1.25f, -1.0f);
            }

            if (!bFillLine)
            {
                glDisable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(0.0f, 0.0f);
            }

            const bool bBlendNone = static_cast<bool>(inCreateInfo.blend == RHI::BlendMode::None);

            if (bBlendNone)
            {
                glDisable(GL_BLEND);
            }

            if (!bBlendNone)
            {
                glEnable(GL_BLEND);
                const bool bBlendAdditive = static_cast<bool>(inCreateInfo.blend == RHI::BlendMode::Additive);

                if (bBlendAdditive)
                {
                    glBlendFunc(GL_ONE, GL_ONE);
                }

                if (!bBlendAdditive)
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

            const bool bStencilNone = static_cast<bool>(inCreateInfo.stencil == RHI::StencilMode::None);

            if (bStencilNone)
            {
                glDisable(GL_STENCIL_TEST);
            }

            if (!bStencilNone)
            {
                glEnable(GL_STENCIL_TEST);
                const bool bStencilWriteReplace = static_cast<bool>(inCreateInfo.stencil == RHI::StencilMode::WriteReplace);

                if (bStencilWriteReplace)
                {
                    glStencilMask(0xFF);
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                }

                if (!bStencilWriteReplace)
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
                const bool bTypeSampledImage = static_cast<bool>(resource.type == RHI::BindingType::SampledImage);

                if (bTypeSampledImage)
                {
                    OpenGLRHIImageData* image = static_cast<OpenGLRHIImageData*>(resource.image.handle);
                    glBindTextureUnit(resource.binding + resource.arrayIndex, image->texture);

                    OpenGLRHISamplerData* sampler = static_cast<OpenGLRHISamplerData*>(resource.sampler.handle);
                    if (sampler)
                    {
                        GLenum     minFilter = GL_LINEAR;
                        const bool bMip    = static_cast<bool>(sampler->createInfo.bHasMip);

                        if (bMip)
                        {
                            minFilter = sampler->createInfo.minFilter == RHI::SamplerFilter::Nearest
                                            ? GL_NEAREST_MIPMAP_LINEAR
                                            : GL_LINEAR_MIPMAP_LINEAR;
                        }

                        const bool bMinFilterNearest = !bMip && (sampler->createInfo.minFilter == RHI::SamplerFilter::Nearest);

                        if (bMinFilterNearest)
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

                        if (sampler->createInfo.address == RHI::SamplerAddress::Repeat)
                        {
                            wrap = GL_REPEAT;
                        }

                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_S, wrap);
                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_T, wrap);
                        glTextureParameteri(image->texture, GL_TEXTURE_WRAP_R, wrap);
                    }
                }

                if (!bTypeSampledImage)
                {
                    OpenGLRHIBufferData* buffer = static_cast<OpenGLRHIBufferData*>(resource.buffer.handle);
                    const GLenum         target =
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
                Renderer::OpenGLRHIImageData* color = static_cast<OpenGLRHIImageData*>(inCreateInfo.color.image.handle);
                const bool                    bNotHasColorOrFbo = static_cast<bool>(!color || (!color->texture && !color->fbo));

                if (bNotHasColorOrFbo)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glDrawBuffer(GL_BACK);
                    bDefault = true;
                }

                const bool bNotOwnedAndFbo = !bNotHasColorOrFbo && (!color->bOwned && color->fbo);

                if (bNotOwnedAndFbo)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, color->fbo);
                }

                if (!bNotHasColorOrFbo && !bNotOwnedAndFbo)
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
                Renderer::OpenGLRHIImageData* depth = static_cast<OpenGLRHIImageData*>(inCreateInfo.depth.image.handle);
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
                    const bool bKindDepth2DArrayOrLayerPositive = static_cast<bool>(
                        depth->kind == RHI::ImageKind::Depth2DArray || depth->layer > 0 || depth->bView
                    );

                    if (bKindDepth2DArrayOrLayerPositive)
                    {
                        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->texture, 0, depth->layer);
                    }

                    const bool bTexture = !bKindDepth2DArrayOrLayerPositive && (depth->texture);

                    if (bTexture)
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
            glDisable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(0.0f, 0.0f);
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
            Renderer::OpenGLRHIBufferData* buffer = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
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
            Renderer::OpenGLRHIBufferData* buffer = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
            m_indexBuffer                         = buffer->id;
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
            Renderer::OpenGLRHIImageData* source = static_cast<OpenGLRHIImageData*>(inSource.handle);
            Renderer::OpenGLRHIImageData* dest   = static_cast<OpenGLRHIImageData*>(inDestination.handle);
            if (!source || !dest)
            {
                return;
            }

            const std::int32_t srcExtentX = static_cast<std::int32_t>(std::max(1u, source->width));
            const std::int32_t srcExtentY = static_cast<std::int32_t>(std::max(1u, source->height));
            const std::int32_t dstExtentX = static_cast<std::int32_t>(std::max(1u, dest->width));
            const std::int32_t dstExtentY = static_cast<std::int32_t>(std::max(1u, dest->height));

            const std::int32_t srcX0 = std::clamp(inX, 0, srcExtentX);
            const std::int32_t srcY0 = std::clamp(inY, 0, srcExtentY);
            const std::int32_t srcX1 = std::clamp(inX + static_cast<std::int32_t>(inWidth), 0, srcExtentX);
            const std::int32_t srcY1 = std::clamp(inY + static_cast<std::int32_t>(inHeight), 0, srcExtentY);
            if (srcX1 <= srcX0 || srcY1 <= srcY0)
            {
                return;
            }

            GLint previous = 0;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previous);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, source->fbo);
            glReadBuffer(source->fbo == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->fbo);
            if (dest->texture)
            {
                glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dest->texture, 0);
            }
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glBlitFramebuffer(
                srcX0,
                srcY0,
                srcX1,
                srcY1,
                0,
                0,
                dstExtentX,
                dstExtentY,
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
