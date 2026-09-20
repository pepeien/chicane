#include "Backend/OpenGL/RHI/Device.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Backend/OpenGL.hpp"
#include "Backend/OpenGL/RHI/CommandList.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLRHIDevice::OpenGLRHIDevice(OpenGLBackend* inBackend)
            : m_backend(inBackend),
              m_commands(std::make_unique<OpenGLRHICommandList>(this))
        {}

        OpenGLRHIDevice::~OpenGLRHIDevice() = default;

        RHI::Buffer OpenGLRHIDevice::createBuffer(const RHI::BufferCreateInfo& inCreateInfo)
        {
            auto* data  = new OpenGLRHIBufferData();
            data->size  = inCreateInfo.size;
            data->usage = inCreateInfo.usage;
            glCreateBuffers(1, &data->id);
            glNamedBufferData(data->id, static_cast<GLsizeiptr>(inCreateInfo.size), nullptr, GL_DYNAMIC_DRAW);
            return {data};
        }

        void OpenGLRHIDevice::destroyBuffer(RHI::Buffer inBuffer)
        {
            auto* data = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
            if (!data)
            {
                return;
            }
            if (data->bOwned && data->id)
            {
                glDeleteBuffers(1, &data->id);
            }
            delete data;
        }

        void OpenGLRHIDevice::updateBuffer(
            RHI::Buffer inBuffer, const void* inData, std::size_t inSize, std::size_t inOffset
        )
        {
            auto* data = static_cast<OpenGLRHIBufferData*>(inBuffer.handle);
            if (!data || !inData)
            {
                return;
            }

            if (inOffset + inSize > data->size)
            {
                const std::size_t newSize = std::max(inOffset + inSize, data->size * 2);
                GLuint            grown   = 0;
                glCreateBuffers(1, &grown);
                glNamedBufferData(grown, static_cast<GLsizeiptr>(newSize), nullptr, GL_DYNAMIC_DRAW);
                if (data->id && data->size > 0)
                {
                    glCopyNamedBufferSubData(
                        data->id,
                        grown,
                        0,
                        0,
                        static_cast<GLsizeiptr>(data->size)
                    );
                }
                if (data->bOwned && data->id)
                {
                    glDeleteBuffers(1, &data->id);
                }
                data->id   = grown;
                data->size = newSize;
            }

            glNamedBufferSubData(data->id, static_cast<GLintptr>(inOffset), static_cast<GLsizeiptr>(inSize), inData);
        }

        static GLenum toGLInternal(RHI::ImageFormat inFormat, RHI::ImageKind inKind)
        {
            if (inKind == RHI::ImageKind::Depth2D || inKind == RHI::ImageKind::Depth2DArray)
            {
                return inFormat == RHI::ImageFormat::Depth24Stencil8 ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32F;
            }
            return inFormat == RHI::ImageFormat::RGBA16F ? GL_RGBA16F : GL_RGBA8;
        }

        RHI::Image OpenGLRHIDevice::createImage(const RHI::ImageCreateInfo& inCreateInfo)
        {
            auto* data   = new OpenGLRHIImageData();
            data->kind   = inCreateInfo.kind;
            data->format = inCreateInfo.format;
            data->width  = inCreateInfo.width;
            data->height = inCreateInfo.height;
            data->layers = inCreateInfo.layers;
            data->mips   = inCreateInfo.mipLevels;
            data->bOwned = true;

            const GLenum internal = toGLInternal(inCreateInfo.format, inCreateInfo.kind);
            if (inCreateInfo.kind == RHI::ImageKind::Cube)
            {
                glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &data->texture);
                glTextureStorage2D(
                    data->texture,
                    static_cast<GLsizei>(inCreateInfo.mipLevels),
                    internal,
                    inCreateInfo.width,
                    inCreateInfo.height
                );
            }
            else if (inCreateInfo.kind == RHI::ImageKind::Depth2DArray)
            {
                glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &data->texture);
                glTextureStorage3D(
                    data->texture,
                    1,
                    internal,
                    inCreateInfo.width,
                    inCreateInfo.height,
                    inCreateInfo.layers
                );
            }
            else
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &data->texture);
                glTextureStorage2D(
                    data->texture,
                    static_cast<GLsizei>(inCreateInfo.mipLevels),
                    internal,
                    inCreateInfo.width,
                    inCreateInfo.height
                );
            }

            glCreateFramebuffers(1, &data->fbo);
            if (inCreateInfo.kind == RHI::ImageKind::Color2D)
            {
                glNamedFramebufferTexture(data->fbo, GL_COLOR_ATTACHMENT0, data->texture, 0);
                const GLenum buffer = GL_COLOR_ATTACHMENT0;
                glNamedFramebufferDrawBuffers(data->fbo, 1, &buffer);
            }
            return {data};
        }

        void OpenGLRHIDevice::destroyImage(RHI::Image inImage)
        {
            auto* data = static_cast<OpenGLRHIImageData*>(inImage.handle);
            if (!data)
            {
                return;
            }
            if (data->bOwned)
            {
                if (data->fbo)
                {
                    glDeleteFramebuffers(1, &data->fbo);
                }
                if (data->texture && !data->bView)
                {
                    glDeleteTextures(1, &data->texture);
                }
            }
            delete data;
        }

        void OpenGLRHIDevice::updateImage(
            RHI::Image inImage, const void* inData, std::uint32_t inWidth, std::uint32_t inHeight, std::uint32_t inLayer
        )
        {
            auto* data = static_cast<OpenGLRHIImageData*>(inImage.handle);
            if (!data || !inData)
            {
                return;
            }
            const GLenum type = data->format == RHI::ImageFormat::RGBA16F ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE;
            if (data->kind == RHI::ImageKind::Cube)
            {
                glTextureSubImage3D(
                    data->texture,
                    0,
                    0,
                    0,
                    static_cast<GLint>(inLayer),
                    inWidth,
                    inHeight,
                    1,
                    GL_RGBA,
                    type,
                    inData
                );
            }
            else
            {
                glTextureSubImage2D(data->texture, 0, 0, 0, inWidth, inHeight, GL_RGBA, type, inData);
            }
        }

        void OpenGLRHIDevice::generateMips(RHI::Image inImage)
        {
            auto* data = static_cast<OpenGLRHIImageData*>(inImage.handle);
            if (data)
            {
                glGenerateTextureMipmap(data->texture);
            }
        }

        RHI::Image OpenGLRHIDevice::createImageView(RHI::Image inImage, std::uint32_t inLayer)
        {
            auto* source = static_cast<OpenGLRHIImageData*>(inImage.handle);
            auto* data   = new OpenGLRHIImageData(*source);
            data->bOwned = true;
            data->bView  = true;
            data->layer  = inLayer;
            data->kind   = RHI::ImageKind::Depth2D;
            data->fbo    = 0;
            glCreateFramebuffers(1, &data->fbo);
            data->texture = source->texture;
            return {data};
        }

        RHI::Sampler OpenGLRHIDevice::createSampler(const RHI::SamplerCreateInfo& inCreateInfo)
        {
            return {new OpenGLRHISamplerData{inCreateInfo}};
        }

        void OpenGLRHIDevice::destroySampler(RHI::Sampler inSampler)
        {
            delete static_cast<OpenGLRHISamplerData*>(inSampler.handle);
        }

        RHI::BindGroupLayout OpenGLRHIDevice::createBindGroupLayout(const RHI::BindGroupLayoutCreateInfo& inCreateInfo)
        {
            return {new OpenGLRHILayoutData{inCreateInfo}};
        }

        void OpenGLRHIDevice::destroyBindGroupLayout(RHI::BindGroupLayout inLayout)
        {
            delete static_cast<OpenGLRHILayoutData*>(inLayout.handle);
        }

        RHI::BindGroup OpenGLRHIDevice::createBindGroup(
            RHI::BindGroupLayout, const std::vector<RHI::BindResource>& inResources
        )
        {
            return {new OpenGLRHIGroupData{inResources}};
        }

        void OpenGLRHIDevice::updateBindGroup(RHI::BindGroup inGroup, const std::vector<RHI::BindResource>& inResources)
        {
            auto* data = static_cast<OpenGLRHIGroupData*>(inGroup.handle);
            if (data)
            {
                data->resources = inResources;
            }
        }

        void OpenGLRHIDevice::destroyBindGroup(RHI::BindGroup inGroup)
        {
            delete static_cast<OpenGLRHIGroupData*>(inGroup.handle);
        }

        namespace
        {
            bool isIdentifier(const std::string& inValue)
            {
                if (inValue.empty() || std::isdigit(static_cast<unsigned char>(inValue.front())))
                {
                    return false;
                }

                return std::all_of(
                    inValue.begin(),
                    inValue.end(),
                    [](char inCharacter)
                    { return inCharacter == '_' || std::isalnum(static_cast<unsigned char>(inCharacter)); }
                );
            }

            std::string toBlockName(const std::string& inInstance)
            {
                const std::size_t separator = inInstance.find_last_of('_');
                if (separator == std::string::npos || separator + 1 >= inInstance.size())
                {
                    return inInstance + "_block";
                }

                const std::string suffix   = inInstance.substr(separator + 1);
                const bool        bIsDedup = std::all_of(
                    suffix.begin(),
                    suffix.end(),
                    [](char inCharacter) { return std::isdigit(static_cast<unsigned char>(inCharacter)); }
                );

                return (bIsDedup ? inInstance.substr(0, separator) : inInstance) + "_block";
            }

            std::string renameStorageBlocks(std::string inSource)
            {
                static const std::string keyword = "buffer ";

                std::size_t cursor = 0;
                while ((cursor = inSource.find(keyword, cursor)) != std::string::npos)
                {
                    const std::size_t nameStart = cursor + keyword.size();
                    const std::size_t nameEnd   = inSource.find_first_of(" \t\r\n{", nameStart);
                    if (nameEnd == std::string::npos)
                    {
                        break;
                    }

                    const std::size_t bodyStart = inSource.find_first_not_of(" \t\r\n", nameEnd);
                    const std::string name      = inSource.substr(nameStart, nameEnd - nameStart);
                    if (!isIdentifier(name) || bodyStart == std::string::npos || inSource[bodyStart] != '{')
                    {
                        cursor = nameEnd;
                        continue;
                    }

                    std::size_t index = bodyStart;
                    int         depth = 0;
                    do
                    {
                        depth += inSource[index] == '{' ? 1 : (inSource[index] == '}' ? -1 : 0);
                        index++;
                    } while (depth > 0 && index < inSource.size());

                    const std::size_t instanceStart = inSource.find_first_not_of(" \t\r\n", index);
                    const std::size_t instanceEnd   = inSource.find_first_of(" \t\r\n;[", instanceStart);
                    if (instanceStart == std::string::npos || instanceEnd == std::string::npos)
                    {
                        break;
                    }

                    const std::string instance = inSource.substr(instanceStart, instanceEnd - instanceStart);
                    if (!isIdentifier(instance))
                    {
                        cursor = nameEnd;
                        continue;
                    }

                    const std::string block = toBlockName(instance);
                    inSource.replace(nameStart, name.size(), block);
                    cursor = nameStart + block.size();
                }

                return inSource;
            }
        }

        std::uint32_t OpenGLRHIDevice::compileProgram(const String& inVertex, const String& inFragment) const
        {
            auto compile = [](GLenum type, const String& path)
            {
                const std::vector<char> code = FileSystem::read(path);
                if (code.empty())
                {
                    throw std::runtime_error("Failed to load shader");
                }
                const std::string patched = renameStorageBlocks(std::string(code.begin(), code.end()));
                GLuint            shader  = glCreateShader(type);
                const GLchar*     source  = patched.c_str();
                const GLint       length  = static_cast<GLint>(patched.size());
                glShaderSource(shader, 1, &source, &length);
                glCompileShader(shader);
                GLint result = GL_FALSE;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
                if (!result)
                {
                    GLint logSize = 0;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
                    std::string log(std::max(logSize, 1), '\0');
                    glGetShaderInfoLog(shader, logSize, nullptr, log.data());
                    glDeleteShader(shader);
                    throw std::runtime_error(
                        "Failed to compile shader " + path.toStandard() + ": " + log + "\n" + patched
                    );
                }
                return shader;
            };

            GLuint       vertex   = compile(GL_VERTEX_SHADER, inVertex);
            GLuint       fragment = 0;
            const GLuint program  = glCreateProgram();
            glAttachShader(program, vertex);
            if (!inFragment.isEmpty())
            {
                fragment = compile(GL_FRAGMENT_SHADER, inFragment);
                glAttachShader(program, fragment);
            }
            glLinkProgram(program);
            GLint result = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &result);
            glDeleteShader(vertex);
            if (fragment)
            {
                glDeleteShader(fragment);
            }
            if (!result)
            {
                GLint logSize = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
                std::string log(std::max(logSize, 1), '\0');
                glGetProgramInfoLog(program, logSize, nullptr, log.data());
                glDeleteProgram(program);
                throw std::runtime_error("Failed link shader program " + inVertex.toStandard() + ": " + log);
            }
            return program;
        }

        RHI::Pipeline OpenGLRHIDevice::createPipeline(const RHI::PipelineCreateInfo& inCreateInfo)
        {
            auto* data       = new OpenGLRHIPipelineData();
            data->createInfo = inCreateInfo;
            data->program    = compileProgram(
                shaderPath(inCreateInfo.vertexPath, ShaderType::Vertex),
                inCreateInfo.fragmentPath.isEmpty() ? String()
                                                       : shaderPath(inCreateInfo.fragmentPath, ShaderType::Fragment)
            );
            glCreateVertexArrays(1, &data->vao);
            for (const RHI::VertexAttribute& attribute : inCreateInfo.vertexAttributes)
            {
                GLint size = attribute.format == RHI::VertexFormat::Float2
                                 ? 2
                                 : (attribute.format == RHI::VertexFormat::Float4 ? 4 : 3);
                glEnableVertexArrayAttrib(data->vao, attribute.location);
                glVertexArrayAttribFormat(data->vao, attribute.location, size, GL_FLOAT, GL_FALSE, attribute.offset);
                glVertexArrayAttribBinding(data->vao, attribute.location, 0);
            }
            glCreateBuffers(1, &data->pushUbo);
            glNamedBufferData(data->pushUbo, 256, nullptr, GL_DYNAMIC_DRAW);
            return {data};
        }

        void OpenGLRHIDevice::destroyPipeline(RHI::Pipeline inPipeline)
        {
            auto* data = static_cast<OpenGLRHIPipelineData*>(inPipeline.handle);
            if (!data)
            {
                return;
            }
            glDeleteProgram(data->program);
            glDeleteVertexArrays(1, &data->vao);
            glDeleteBuffers(1, &data->pushUbo);
            delete data;
        }

        String OpenGLRHIDevice::shaderPath(const String& inLogical, ShaderType inType) const
        {
            return inLogical + (inType == ShaderType::Vertex ? ".vert.glsl" : ".frag.glsl");
        }

        RHI::ImageFormat OpenGLRHIDevice::sceneColorFormat() const
        {
            return RHI::ImageFormat::RGBA16F;
        }

        RHI::ImageFormat OpenGLRHIDevice::sceneDepthFormat() const
        {
            return RHI::ImageFormat::Depth32F;
        }

        RHI::ImageFormat OpenGLRHIDevice::presentColorFormat() const
        {
            return RHI::ImageFormat::RGBA8;
        }

        RHI::Image OpenGLRHIDevice::wrapTexture(
            std::uint32_t    inTexture,
            RHI::ImageKind   inKind,
            RHI::ImageFormat inFormat,
            std::uint32_t    inWidth,
            std::uint32_t    inHeight,
            std::uint32_t    inFbo
        )
        {
            auto* data    = new OpenGLRHIImageData();
            data->texture = inTexture;
            data->fbo     = inFbo;
            data->kind    = inKind;
            data->format  = inFormat;
            data->width   = inWidth;
            data->height  = inHeight;
            data->bOwned  = false;
            return {data};
        }

        RHI::Buffer OpenGLRHIDevice::wrapBuffer(std::uint32_t inId, std::size_t inSize, RHI::BufferUsage inUsage)
        {
            auto* data   = new OpenGLRHIBufferData();
            data->id     = inId;
            data->size   = inSize;
            data->usage  = inUsage;
            data->bOwned = false;
            return {data};
        }

        RHI::CommandList* OpenGLRHIDevice::commandList()
        {
            return m_commands.get();
        }
    }
}
