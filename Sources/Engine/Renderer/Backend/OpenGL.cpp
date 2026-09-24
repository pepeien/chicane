#include "Backend/OpenGL.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Backend/OpenGL/Debug.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/UI.hpp"
#include "Backend/OpenGL/RHI/Device.hpp"
#include "Chicane/Renderer/Draw/Particle.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Feature.hpp"
#include "Chicane/Renderer/Shadow.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static SDL_GLContext g_context;

        OpenGLBackend::OpenGLBackend()
            : Backend(),
              frames({}),
              m_currentFrameIndex(0U),
              m_textures({}),
              m_classes({}),
              m_textureTable(0),
              m_maxArrayLayers(2048),
              m_targetFramebuffer(0),
              m_targetColor(0),
              m_targetDepth(0),
              m_targetWidth(0),
              m_targetHeight(0),
              m_screenBlitFramebuffer(0),
              m_screenTextureId(Draw::InvalidId),
              m_gpuQueries({}),
              m_gpuQueryPending({}),
              m_gpuQueryWrite(0)
        {
            for (std::uint32_t index = 0; index < TEXTURE_CLASS_COUNT; index++)
            {
                m_classes[index].size = TEXTURE_CLASS_SIZES[index];
            }
        }

        OpenGLBackend::~OpenGLBackend()
        {
            onShutdown();
        }

        void OpenGLBackend::onInit()
        {
            if (isStatus(BackendStatus::Running))
            {
                return;
            }

            Backend::onInit();

            buildContext();
            buildGlad();
            enableFeatures();
            updateResourcesBudget();
            buildFrames();
            buildGpuQueries();
            buildTextureData();
            buildTarget();
            m_rhi = std::make_unique<OpenGLRHIDevice>(this);

            RHI::BufferCreateInfo camera;
            camera.size           = sizeof(View);
            camera.usage          = RHI::BufferUsage::Uniform;
            camera.bHasHostAccess = true;
            m_cameraBuffer        = m_rhi->createBuffer(camera);

            RHI::BufferCreateInfo light;
            light.size           = sizeof(ShadowLight);
            light.usage          = RHI::BufferUsage::Storage;
            light.bHasHostAccess = true;
            m_lightBuffer        = m_rhi->createBuffer(light);

            RHI::BufferCreateInfo instances;
            instances.size           = getResourceBudget(Resource::SceneInstances);
            instances.usage          = RHI::BufferUsage::Storage;
            instances.bHasHostAccess = true;
            m_instanceBuffer         = m_rhi->createBuffer(instances);

            RHI::BufferCreateInfo particles;
            particles.size           = sizeof(DrawParticle) * MAX_PARTICLES;
            particles.usage          = RHI::BufferUsage::Storage;
            particles.bHasHostAccess = true;
            m_particleBuffer         = m_rhi->createBuffer(particles);

            RHI::BufferCreateInfo ui;
            ui.size                     = getResourceBudget(Resource::UIInstances);
            ui.usage                    = RHI::BufferUsage::Storage;
            ui.bHasHostAccess           = true;
            m_rhiFrame.instance2DBuffer = m_rhi->createBuffer(ui);

            RHI::SamplerCreateInfo sampler;
            sampler.minFilter = RHI::SamplerFilter::Linear;
            sampler.magFilter = RHI::SamplerFilter::Linear;
            sampler.address   = RHI::SamplerAddress::ClampToEdge;
            m_linearSampler   = m_rhi->createSampler(sampler);

            RHI::SamplerCreateInfo textureSampler;
            textureSampler.minFilter = RHI::SamplerFilter::Linear;
            textureSampler.magFilter = RHI::SamplerFilter::Linear;
            textureSampler.address   = RHI::SamplerAddress::Repeat;
            textureSampler.bHasMip   = true;
            m_textureSampler         = m_rhi->createSampler(textureSampler);

            RHI::BindGroupLayoutCreateInfo textureLayout;
            RHI::Binding                   binding;
            binding.binding     = TEXTURE_TABLE_BINDING;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            textureLayout.bindings.push_back(binding);
            for (std::uint32_t index = 0; index < TEXTURE_CLASS_COUNT; index++)
            {
                binding.binding = TEXTURE_CLASS_BINDING + index;
                binding.type    = RHI::BindingType::SampledImage;
                textureLayout.bindings.push_back(binding);
            }
            m_textureLayout = m_rhi->createBindGroupLayout(textureLayout);

            wrapTargetImages();
            buildTextureBindGroup();
            m_bloomPass.init(m_rhi.get());
            m_bloomPass.resize(m_targetWidth, m_targetHeight, m_rhi->sceneColorFormat());
            buildLayers();
        }

        void OpenGLBackend::onShutdown()
        {
            if (isStatus(BackendStatus::Shutdown))
            {
                return;
            }

            Backend::onShutdown();

            // Layers
            destroyLayers();
            m_bloomPass.destroy();
            destroyRhiResources();
            destroyFrames();

            destroyGpuQueries();
            destroyTextureData();
            destroyTarget();
            m_rhi.reset();
            destroyContext();
        }

        void OpenGLBackend::onLoad(const DrawTextureResource& inResources)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            Draw::Id maxId = Draw::InvalidId;
            for (const DrawTexture& texture : inResources.getDraws())
            {
                maxId = std::max(maxId, texture.id);
            }

            if (maxId > Draw::InvalidId && m_textures.size() < static_cast<std::size_t>(maxId) + 1)
            {
                m_textures.resize(static_cast<std::size_t>(maxId) + 1);
            }

            for (const DrawTexture& texture : inResources.getDraws())
            {
                if (texture.reference.equals(SCREEN_TARGET_ID))
                {
                    m_screenTextureId = texture.id;

                    continue;
                }

                uploadTexture(texture);
            }

            bindTextureTable();
            buildTextureBindGroup();
            Backend::onLoad(inResources);
        }

        void OpenGLBackend::onBeginRender()
        {
            resolveGpuQuery(m_gpuQueryWrite);
            beginGpuQuery();

            buildTarget();
            bindTarget();

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            glStencilMask(0xFF);
            glDisable(GL_SCISSOR_TEST);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            bindTextureTable();
        }

        void OpenGLBackend::onRender(const Frame& inFrame)
        {
            m_currentFrameIndex = (m_currentFrameIndex + 1) % frames.size();
            fillRhiFrame(inFrame);
            renderLayers(
                inFrame,
                &m_rhiFrame,
                [](const Layer* inLayer) { return !inLayer->getId().equals(UI_LAYER_ID); }
            );

            presentTarget(inFrame);
            bindTextureTable();

            renderLayers(
                inFrame,
                &m_rhiFrame,
                [](const Layer* inLayer) { return inLayer->getId().equals(UI_LAYER_ID); }
            );

            m_currentFrameIndex = (m_currentFrameIndex + 1) % frames.size();
        }

        void OpenGLBackend::onEndRender()
        {
            endGpuQuery();

            SDL_Window* window = static_cast<SDL_Window*>(getRenderer()->getWindow()->getInstance());

            if (!SDL_GL_SwapWindow(window))
            {
                throw std::runtime_error(std::string("Failed to swawp window frame buffer [") + SDL_GetError() + "]");
            }
        }

        Viewport OpenGLBackend::getGLViewport(Layer* inLayer) const
        {
            const Vec<2, std::uint32_t> resolution = getRenderer()->getResolution();

            Viewport result   = getLayerViewport(inLayer);
            result.position.y = resolution.y - (result.position.y + result.size.y);

            return result;
        }

        RHI::Viewport OpenGLBackend::getRHIViewport(Layer* inLayer) const
        {
            const Viewport viewport = getGLViewport(inLayer);

            RHI::Viewport result;
            result.size     = viewport.size;
            result.position = viewport.position;

            return result;
        }

        RHI::Scissor OpenGLBackend::getRHIScissor(Layer* inLayer) const
        {
            const Viewport viewport = getGLViewport(inLayer);

            RHI::Scissor result;
            result.x      = static_cast<std::int32_t>(viewport.position.x);
            result.y      = static_cast<std::int32_t>(viewport.position.y);
            result.width  = static_cast<std::uint32_t>(viewport.size.x);
            result.height = static_cast<std::uint32_t>(viewport.size.y);

            return result;
        }

        void OpenGLBackend::useViewport(Layer* inLayer) const
        {
            const Viewport viewport = getGLViewport(inLayer);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
        }

        void OpenGLBackend::useProgram(std::uint32_t inId) const
        {
            glUseProgram(inId);
        }

        void OpenGLBackend::destroyProgram(std::uint32_t inId) const
        {
            glDeleteProgram(inId);
        }

        std::uint32_t OpenGLBackend::initShader(const Shader::List& inShaders) const
        {
            std::vector<GLuint> modules;

            for (const Shader& shader : inShaders)
            {
                GLuint module = 0;

                switch (shader.type)
                {
                case ShaderType::Fragment:
                    module = glCreateShader(GL_FRAGMENT_SHADER);

                    break;

                case ShaderType::Vertex:
                    module = glCreateShader(GL_VERTEX_SHADER);

                    break;

                default:
                    break;
                }

                if (module == 0)
                {
                    continue;
                }

                const std::vector<char> code = Chicane::FileSystem::read(shader.source);
                std::string             source(code.begin(), code.end());
                const char*             src = source.c_str();

                GLint result = GL_FALSE;
                glShaderSource(module, 1, &src, nullptr);
                glCompileShader(module);
                glGetShaderiv(module, GL_COMPILE_STATUS, &result);

                if (!result)
                {
                    throw std::runtime_error("Failed to load shader module");
                }

                modules.push_back(module);
            }

            const std::uint32_t program = glCreateProgram();
            for (GLuint module : modules)
            {
                glAttachShader(program, module);
            }
            glLinkProgram(program);

            GLint result = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &result);
            if (!result)
            {
                throw std::runtime_error("Failed link shader program");
            }

            for (GLuint module : modules)
            {
                glDeleteShader(module);
            }

            return program;
        }

        std::uint32_t OpenGLBackend::initVertexArray(std::uint32_t inCount) const
        {
            std::uint32_t result = 0;

            glCreateVertexArrays(1, &result);

            return result;
        }

        void OpenGLBackend::bindVertexArray(std::uint32_t inId) const
        {
            glBindVertexArray(inId);
        }

        void OpenGLBackend::destroyVertexArray(std::uint32_t& inId) const
        {
            glDeleteVertexArrays(1, &inId);
        }

        void OpenGLBackend::enableDepth(const Depth& inSettings) const
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(inSettings.bCanWrite ? GL_TRUE : GL_FALSE);
            glDepthFunc(toGLDepthCompare(inSettings.compare));
        }

        void OpenGLBackend::disableDepth() const
        {
            glDisable(GL_DEPTH_TEST);
        }

        void OpenGLBackend::enableCulling(const Culling& inSettings) const
        {
            glEnable(GL_CULL_FACE);
            glFrontFace(toGLFrontFace(inSettings.frontFace));
            glCullFace(toGLCullingMode(inSettings.mode));
        }

        void OpenGLBackend::disableCulling() const
        {
            glDisable(GL_CULL_FACE);
        }

        void OpenGLBackend::enableBlending(const Blending& inSettings) const
        {
            glEnable(GL_BLEND);
            glBlendFunc(toGLFactor(inSettings.source), toGLFactor(inSettings.destination));
        }

        void OpenGLBackend::disableBlending() const
        {
            glDisable(GL_BLEND);
        }

        void OpenGLBackend::drawPolyArrays(const DrawPoly& inSettings, std::uint32_t inVertexArrayId) const
        {
            bindVertexArray(inVertexArrayId);

            glDrawArrays(toGLDrawTopology(inSettings.topology), inSettings.indexStart, inSettings.indexCount);
        }

        void OpenGLBackend::buildContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(getRenderer()->getWindow()->getInstance());

            g_context = SDL_GL_CreateContext(window);

            if (!g_context)
            {
                throw std::runtime_error(std::string("Failed to instantiate context [") + SDL_GetError() + "]");
            }

            if (!SDL_GL_MakeCurrent(window, g_context))
            {
                throw std::runtime_error(std::string("Failed to initialize context [") + SDL_GetError() + "]");
            }

            if (!SDL_GL_SetSwapInterval(0))
            {
                throw std::runtime_error(std::string("Failed to disable V-Sync [") + SDL_GetError() + "]");
            }
        }

        void OpenGLBackend::destroyContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(getRenderer()->getWindow()->getInstance());

            if (!SDL_GL_MakeCurrent(window, nullptr))
            {
                throw std::runtime_error(std::string("Failed to unref context [") + SDL_GetError() + "]");
            }

            if (!SDL_GL_DestroyContext(g_context))
            {
                throw std::runtime_error(std::string("Failed to destroy context [") + SDL_GetError() + "]");
            }
        }

        void OpenGLBackend::buildGlad()
        {
            if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
            {
                throw std::runtime_error("Failed to initialize Glad");
            }
        }

        void OpenGLBackend::buildGpuQueries()
        {
            destroyGpuQueries();

            const std::uint32_t count = std::max(3U, getRenderer()->getFrameInFlighCount());
            m_gpuQueries.resize(count);
            m_gpuQueryPending.assign(count, false);
            m_gpuQueryWrite = 0;

            for (std::array<std::uint32_t, 2>& pair : m_gpuQueries)
            {
                glGenQueries(2, pair.data());
            }
        }

        void OpenGLBackend::destroyGpuQueries()
        {
            for (std::array<std::uint32_t, 2>& pair : m_gpuQueries)
            {
                if (pair[0] != 0)
                {
                    glDeleteQueries(2, pair.data());
                    pair = {0, 0};
                }
            }

            m_gpuQueries.clear();
            m_gpuQueryPending.clear();
            m_gpuQueryWrite = 0;
        }

        void OpenGLBackend::beginGpuQuery()
        {
            if (m_gpuQueries.empty())
            {
                return;
            }

            glQueryCounter(m_gpuQueries[m_gpuQueryWrite][0], GL_TIMESTAMP);
        }

        void OpenGLBackend::endGpuQuery()
        {
            if (m_gpuQueries.empty())
            {
                return;
            }

            glQueryCounter(m_gpuQueries[m_gpuQueryWrite][1], GL_TIMESTAMP);
            m_gpuQueryPending[m_gpuQueryWrite] = true;
            m_gpuQueryWrite = (m_gpuQueryWrite + 1U) % static_cast<std::uint32_t>(m_gpuQueries.size());
        }

        void OpenGLBackend::resolveGpuQuery(std::uint32_t inSlot)
        {
            if (m_gpuQueries.empty() || inSlot >= m_gpuQueries.size() || !m_gpuQueryPending[inSlot])
            {
                return;
            }

            GLint64 start = 0;
            GLint64 end   = 0;
            glGetQueryObjecti64v(m_gpuQueries[inSlot][0], GL_QUERY_RESULT, &start);
            glGetQueryObjecti64v(m_gpuQueries[inSlot][1], GL_QUERY_RESULT, &end);

            setGpuDelta(static_cast<float>(static_cast<double>(end - start) / 1'000'000.0));
            m_gpuQueryPending[inSlot] = false;
        }

        void OpenGLBackend::enableFeatures()
        {
            if (IS_DEBUGGING)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(OpenGLDebugCallback, nullptr);
            }
        }

        void OpenGLBackend::updateResourcesBudget()
        {
            std::size_t VRAM = 512ULL * 1024 * 1024; // 512MB

            // NVIDIA
            if (GLAD_GL_NVX_gpu_memory_info)
            {
                GLint total = 0;
                glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);

                VRAM = static_cast<size_t>(total) * 1024;
            }

            // AMD
            if (GLAD_GL_ATI_meminfo)
            {
                std::array<GLint, 4> mem = {};
                glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, mem.data());

                VRAM = static_cast<size_t>(mem[0]) * 1024;
            }

            setVRAM(VRAM);
        }

        void OpenGLBackend::buildTextureData()
        {
            static_assert(sizeof(OpenGLTextureTableEntry) == 16, "GLSL uvec4 table entries must be 16 bytes");

            GLint maxLayers = 2048;
            glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);
            m_maxArrayLayers = static_cast<std::uint32_t>(std::max(8, maxLayers));

            createClassArray(m_classes[0], 8);
            fillWhiteLayer(m_classes[0], 0);
            m_classes[0].used = 1;

            const std::uint32_t slotCount = getResourceBudgetCount(Resource::Texture);
            glCreateBuffers(1, &m_textureTable);
            glNamedBufferStorage(
                m_textureTable,
                static_cast<GLsizeiptr>(sizeof(OpenGLTextureTableEntry) * slotCount),
                nullptr,
                GL_DYNAMIC_STORAGE_BIT
            );

            std::vector<OpenGLTextureTableEntry> entries(slotCount);
            glNamedBufferSubData(
                m_textureTable,
                0,
                static_cast<GLsizeiptr>(entries.size() * sizeof(OpenGLTextureTableEntry)),
                entries.data()
            );

            m_textures.clear();
        }

        void OpenGLBackend::destroyTextureData()
        {
            m_textures.clear();

            for (OpenGLTextureSizeClass& sizeClass : m_classes)
            {
                if (sizeClass.texture != 0)
                {
                    glDeleteTextures(1, &sizeClass.texture);
                    sizeClass.texture = 0;
                }

                sizeClass.allocated = 0;
                sizeClass.used      = 0;
                sizeClass.freeLayers.clear();
            }

            if (m_textureTable != 0)
            {
                glDeleteBuffers(1, &m_textureTable);
                m_textureTable = 0;
            }
        }

        void OpenGLBackend::bindTextureTable() const
        {
            if (m_textureTable == 0)
            {
                return;
            }

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TEXTURE_TABLE_BINDING, m_textureTable);

            const std::uint32_t fallback = m_classes[0].texture;
            for (std::uint32_t index = 0; index < TEXTURE_CLASS_COUNT; index++)
            {
                const std::uint32_t texture = m_classes[index].texture != 0 ? m_classes[index].texture : fallback;
                if (texture == 0)
                {
                    continue;
                }

                glBindTextureUnit(TEXTURE_CLASS_BINDING + index, texture);
            }
        }

        void OpenGLBackend::releaseTextureSlot(OpenGLTextureSlot& inSlot)
        {
            if (inSlot.classIndex < TEXTURE_CLASS_COUNT)
            {
                OpenGLTextureSizeClass& sizeClass = m_classes[inSlot.classIndex];
                if (inSlot.layer != 0 || inSlot.classIndex != 0)
                {
                    sizeClass.freeLayers.push_back(inSlot.layer);
                }
            }

            inSlot.classIndex     = ~0u;
            inSlot.layer          = 0;
            inSlot.sourceWidth    = 0;
            inSlot.sourceHeight   = 0;
            inSlot.residentMinMip = ~0u;
        }

        void OpenGLBackend::writeTextureSlot(
            Draw::Id inId, std::uint32_t inClass, std::uint32_t inLayer, std::uint32_t inWidth, std::uint32_t inHeight
        )
        {
            if (inId <= Draw::InvalidId || m_textureTable == 0)
            {
                return;
            }

            const std::uint32_t slotCount = getResourceBudgetCount(Resource::Texture);
            if (static_cast<std::uint32_t>(inId) >= slotCount)
            {
                return;
            }

            const OpenGLTextureTableEntry entry{inClass, inLayer, std::max(1u, inWidth), std::max(1u, inHeight)};
            glNamedBufferSubData(
                m_textureTable,
                static_cast<GLintptr>(static_cast<std::size_t>(inId) * sizeof(OpenGLTextureTableEntry)),
                sizeof(OpenGLTextureTableEntry),
                &entry
            );
        }

        std::uint32_t OpenGLBackend::classFromResident(std::uint32_t inWidth, std::uint32_t inHeight) const
        {
            const std::uint32_t dimension = std::max(inWidth, inHeight);
            for (std::uint32_t index = 0; index < TEXTURE_CLASS_COUNT; index++)
            {
                if (dimension <= TEXTURE_CLASS_SIZES[index])
                {
                    return index;
                }
            }

            return TEXTURE_CLASS_COUNT - 1;
        }

        void OpenGLBackend::createClassArray(OpenGLTextureSizeClass& inClass, std::uint32_t inLayers)
        {
            const std::uint32_t layers    = std::max(1u, std::min(inLayers, m_maxArrayLayers));
            const std::uint32_t mipLevels = Image::mipCount(inClass.size, inClass.size);

            std::uint32_t texture = 0;
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
            glTextureStorage3D(
                texture,
                static_cast<GLsizei>(mipLevels),
                GL_RGBA8,
                static_cast<GLsizei>(inClass.size),
                static_cast<GLsizei>(inClass.size),
                static_cast<GLsizei>(layers)
            );
            glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

            GLfloat anisotropy = 1.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
            if (anisotropy > 1.0f)
            {
                glTextureParameterf(texture, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
            }

            inClass.texture   = texture;
            inClass.allocated = layers;
        }

        void OpenGLBackend::fillWhiteLayer(const OpenGLTextureSizeClass& inClass, std::uint32_t inLayer) const
        {
            if (inClass.texture == 0)
            {
                return;
            }

            const std::uint32_t mipLevels = Image::mipCount(inClass.size, inClass.size);
            for (std::uint32_t level = 0; level < mipLevels; level++)
            {
                const std::uint32_t        width = Image::mipDimension(inClass.size, level);
                std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * width * 4, 255);
                glTextureSubImage3D(
                    inClass.texture,
                    static_cast<GLint>(level),
                    0,
                    0,
                    static_cast<GLint>(inLayer),
                    static_cast<GLsizei>(width),
                    static_cast<GLsizei>(width),
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    pixels.data()
                );
            }
        }

        void OpenGLBackend::growClass(std::uint32_t inClass, std::uint32_t inLayers)
        {
            OpenGLTextureSizeClass& sizeClass = m_classes[inClass];
            const std::uint32_t     layers    = std::min(std::max(inLayers, sizeClass.allocated + 1), m_maxArrayLayers);
            if (layers <= sizeClass.allocated && sizeClass.texture != 0)
            {
                return;
            }

            const std::uint32_t previous  = sizeClass.texture;
            const std::uint32_t oldCount  = sizeClass.allocated;
            const std::uint32_t mipLevels = Image::mipCount(sizeClass.size, sizeClass.size);
            createClassArray(sizeClass, layers);

            if (previous != 0 && oldCount > 0)
            {
                for (std::uint32_t level = 0; level < mipLevels; level++)
                {
                    const std::uint32_t width = Image::mipDimension(sizeClass.size, level);
                    glCopyImageSubData(
                        previous,
                        GL_TEXTURE_2D_ARRAY,
                        static_cast<GLint>(level),
                        0,
                        0,
                        0,
                        sizeClass.texture,
                        GL_TEXTURE_2D_ARRAY,
                        static_cast<GLint>(level),
                        0,
                        0,
                        0,
                        static_cast<GLsizei>(width),
                        static_cast<GLsizei>(width),
                        static_cast<GLsizei>(oldCount)
                    );
                }

                glDeleteTextures(1, &previous);
            }
        }

        std::uint32_t OpenGLBackend::allocateLayer(std::uint32_t inClass)
        {
            OpenGLTextureSizeClass& sizeClass = m_classes[inClass];
            if (!sizeClass.freeLayers.empty())
            {
                const std::uint32_t layer = sizeClass.freeLayers.back();
                sizeClass.freeLayers.pop_back();

                return layer;
            }

            if (sizeClass.texture == 0)
            {
                createClassArray(sizeClass, 4);
                sizeClass.used = 0;
            }

            if (sizeClass.used >= sizeClass.allocated)
            {
                const std::uint32_t next =
                    std::min(std::max(sizeClass.allocated * 2, sizeClass.allocated + 4), m_maxArrayLayers);
                if (next <= sizeClass.allocated)
                {
                    return ~0u;
                }

                growClass(inClass, next);
            }

            if (sizeClass.texture == 0 || sizeClass.used >= sizeClass.allocated)
            {
                return ~0u;
            }

            const std::uint32_t layer = sizeClass.used;
            sizeClass.used++;

            return layer;
        }

        void OpenGLBackend::uploadTexture(const DrawTexture& inTexture)
        {
            if (inTexture.id <= Draw::InvalidId)
            {
                return;
            }

            if (m_textures.size() <= static_cast<std::size_t>(inTexture.id))
            {
                m_textures.resize(static_cast<std::size_t>(inTexture.id) + 1);
            }

            const std::uint32_t residentWidth =
                std::max(1u, Image::mipDimension(inTexture.width, inTexture.residentMinMip));
            const std::uint32_t residentHeight =
                std::max(1u, Image::mipDimension(inTexture.height, inTexture.residentMinMip));
            const std::uint32_t classIndex = classFromResident(residentWidth, residentHeight);

            OpenGLTextureSlot& slot = m_textures[static_cast<std::size_t>(inTexture.id)];
            if (slot.classIndex == classIndex && slot.sourceWidth == inTexture.width &&
                slot.sourceHeight == inTexture.height && slot.residentMinMip == inTexture.residentMinMip)
            {
                return;
            }

            releaseTextureSlot(slot);

            const std::uint32_t     layer     = allocateLayer(classIndex);
            OpenGLTextureSizeClass& sizeClass = m_classes[classIndex];
            if (layer == ~0u || sizeClass.texture == 0)
            {
                writeTextureSlot(inTexture.id, 0, 0, residentWidth, residentHeight);

                return;
            }

            const std::uint32_t mipLevels = Image::mipCount(residentWidth, residentHeight);

            Image::Instance lastImage;
            for (std::uint32_t gpuLevel = 0; gpuLevel < mipLevels; gpuLevel++)
            {
                Image::Instance image;
                if (inTexture.mips)
                {
                    image = inTexture.mips->decode(inTexture.residentMinMip + gpuLevel);
                }
                else if (gpuLevel == 0)
                {
                    image = inTexture.image;
                }

                if (!image || !image->getPixels())
                {
                    image = lastImage;
                }

                const std::uint32_t       levelWidth  = std::max(1u, Image::mipDimension(residentWidth, gpuLevel));
                const std::uint32_t       levelHeight = std::max(1u, Image::mipDimension(residentHeight, gpuLevel));
                std::vector<Image::Pixel> staging(static_cast<std::size_t>(levelWidth) * levelHeight * 4, 255);
                if (image && image->getPixels())
                {
                    if (image->getWidth() == static_cast<int>(levelWidth) &&
                        image->getHeight() == static_cast<int>(levelHeight))
                    {
                        std::memcpy(staging.data(), image->getPixels(), staging.size());
                    }
                    else
                    {
                        image->blit(staging.data(), static_cast<int>(levelWidth), static_cast<int>(levelHeight));
                    }
                }

                glTextureSubImage3D(
                    sizeClass.texture,
                    static_cast<GLint>(gpuLevel),
                    0,
                    0,
                    static_cast<GLint>(layer),
                    static_cast<GLsizei>(levelWidth),
                    static_cast<GLsizei>(levelHeight),
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    staging.data()
                );

                lastImage = image;
            }

            slot.classIndex     = classIndex;
            slot.layer          = layer;
            slot.sourceWidth    = inTexture.width;
            slot.sourceHeight   = inTexture.height;
            slot.residentMinMip = inTexture.residentMinMip;
            writeTextureSlot(inTexture.id, classIndex, layer, residentWidth, residentHeight);
        }

        void OpenGLBackend::buildTarget()
        {
            const Vec<2, std::uint32_t> resolution = getRenderer()->getResolution();
            const std::uint32_t         width      = std::max(1u, resolution.x);
            const std::uint32_t         height     = std::max(1u, resolution.y);

            if (m_targetFramebuffer != 0 && m_targetWidth == width && m_targetHeight == height)
            {
                return;
            }

            destroyTarget();

            glCreateTextures(GL_TEXTURE_2D, 1, &m_targetColor);
            glTextureStorage2D(m_targetColor, 1, GL_RGBA16F, width, height);
            glTextureParameteri(m_targetColor, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_targetColor, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_targetColor, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_targetColor, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glCreateTextures(GL_TEXTURE_2D, 1, &m_targetDepth);
            glTextureStorage2D(m_targetDepth, 1, GL_DEPTH32F_STENCIL8, width, height);
            glTextureParameteri(m_targetDepth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_targetDepth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glCreateFramebuffers(1, &m_targetFramebuffer);
            glNamedFramebufferTexture(m_targetFramebuffer, GL_COLOR_ATTACHMENT0, m_targetColor, 0);
            glNamedFramebufferTexture(m_targetFramebuffer, GL_DEPTH_STENCIL_ATTACHMENT, m_targetDepth, 0);

            glCreateFramebuffers(1, &m_screenBlitFramebuffer);

            m_targetWidth  = width;
            m_targetHeight = height;

            if (m_rhi)
            {
                wrapTargetImages();
                m_bloomPass.resize(m_targetWidth, m_targetHeight, m_rhi->sceneColorFormat());
            }
        }

        void OpenGLBackend::destroyTarget()
        {
            if (m_rhi)
            {
                auto* device = static_cast<OpenGLRHIDevice*>(m_rhi.get());
                if (m_sceneColor.handle)
                {
                    device->destroyImage(m_sceneColor);
                    m_sceneColor = {};
                }
                if (m_sceneDepth.handle)
                {
                    device->destroyImage(m_sceneDepth);
                    m_sceneDepth = {};
                }
                if (m_presentColor.handle)
                {
                    device->destroyImage(m_presentColor);
                    m_presentColor = {};
                }
            }

            if (m_screenBlitFramebuffer != 0)
            {
                glDeleteFramebuffers(1, &m_screenBlitFramebuffer);
                m_screenBlitFramebuffer = 0;
            }

            if (m_targetFramebuffer != 0)
            {
                glDeleteFramebuffers(1, &m_targetFramebuffer);
                m_targetFramebuffer = 0;
            }

            if (m_targetDepth != 0)
            {
                glDeleteTextures(1, &m_targetDepth);
                m_targetDepth = 0;
            }

            if (m_targetColor != 0)
            {
                glDeleteTextures(1, &m_targetColor);
                m_targetColor = 0;
            }

            m_targetWidth  = 0;
            m_targetHeight = 0;
        }

        void OpenGLBackend::bindTarget() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_targetFramebuffer);
        }

        std::uint32_t OpenGLBackend::getTargetColor() const
        {
            return m_targetColor;
        }

        Draw::Id OpenGLBackend::getScreenTextureId() const
        {
            return m_screenTextureId;
        }

        bool OpenGLBackend::captureScreen(
            std::uint32_t& outWidth, std::uint32_t& outHeight, std::vector<unsigned char>& outRgba
        )
        {
            outWidth  = 0;
            outHeight = 0;
            outRgba.clear();

            const Vec<2, std::uint32_t> resolution =
                getRenderer() ? getRenderer()->getResolution() : Vec<2, std::uint32_t>(0, 0);
            if (resolution.x == 0 || resolution.y == 0)
            {
                return false;
            }

            glFinish();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glReadBuffer(GL_FRONT);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);

            const std::size_t bytes =
                static_cast<std::size_t>(resolution.x) * static_cast<std::size_t>(resolution.y) * 4;
            outRgba.resize(bytes);
            glReadPixels(
                0,
                0,
                static_cast<GLsizei>(resolution.x),
                static_cast<GLsizei>(resolution.y),
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                outRgba.data()
            );

            Image::flipY(outRgba.data(), static_cast<int>(resolution.x), static_cast<int>(resolution.y), 4);

            outWidth  = resolution.x;
            outHeight = resolution.y;

            return true;
        }

        void OpenGLBackend::presentTarget(const Frame& inFrame)
        {
            if (m_targetFramebuffer == 0)
            {
                return;
            }

            if (isScreenComposited(inFrame) || !m_rhiFrame.commands)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glDrawBuffer(GL_BACK);
                glDisable(GL_SCISSOR_TEST);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                return;
            }

            m_bloomPass.apply(
                m_rhiFrame.commands,
                m_rhiFrame.sceneColor,
                m_rhiFrame.presentColor,
                m_rhiFrame.width,
                m_rhiFrame.height,
                inFrame.hasFeature(RendererFeature::HDR),
                m_rhiFrame.frameIndex,
                inFrame.hasFeature(RendererFeature::Bloom)
            );
        }

        void OpenGLBackend::wrapTargetImages()
        {
            auto* device = static_cast<OpenGLRHIDevice*>(m_rhi.get());
            if (!device)
            {
                return;
            }

            if (m_sceneColor.handle)
            {
                device->destroyImage(m_sceneColor);
                m_sceneColor = {};
            }
            if (m_sceneDepth.handle)
            {
                device->destroyImage(m_sceneDepth);
                m_sceneDepth = {};
            }
            if (m_presentColor.handle)
            {
                device->destroyImage(m_presentColor);
                m_presentColor = {};
            }

            m_sceneColor = device->wrapTexture(
                m_targetColor,
                RHI::ImageKind::Color2D,
                RHI::ImageFormat::RGBA16F,
                m_targetWidth,
                m_targetHeight,
                m_targetFramebuffer
            );
            m_sceneDepth = device->wrapTexture(
                m_targetDepth,
                RHI::ImageKind::Depth2D,
                RHI::ImageFormat::Depth32F,
                m_targetWidth,
                m_targetHeight,
                m_targetFramebuffer
            );
            m_presentColor = device->wrapTexture(
                0,
                RHI::ImageKind::Color2D,
                device->presentColorFormat(),
                m_targetWidth,
                m_targetHeight,
                0
            );
        }

        void OpenGLBackend::buildTextureBindGroup()
        {
            auto* device = static_cast<OpenGLRHIDevice*>(m_rhi.get());
            if (!device || m_textureTable == 0)
            {
                return;
            }

            if (m_textureTableBuffer.handle)
            {
                device->destroyBuffer(m_textureTableBuffer);
                m_textureTableBuffer = {};
            }
            for (RHI::Image& image : m_classImages)
            {
                if (image.handle)
                {
                    device->destroyImage(image);
                    image = {};
                }
            }
            if (m_textureGroup.handle)
            {
                device->destroyBindGroup(m_textureGroup);
                m_textureGroup = {};
            }

            m_textureTableBuffer = device->wrapBuffer(
                m_textureTable,
                sizeof(OpenGLTextureTableEntry) * getResourceBudgetCount(Resource::Texture),
                RHI::BufferUsage::Storage
            );

            std::vector<RHI::BindResource> resources;
            resources.push_back(
                {TEXTURE_TABLE_BINDING, RHI::BindingType::StorageBuffer, m_textureTableBuffer, {}, {}, 0}
            );
            for (std::uint32_t index = 0; index < TEXTURE_CLASS_COUNT; index++)
            {
                const std::uint32_t texture =
                    m_classes[index].texture != 0 ? m_classes[index].texture : m_classes[0].texture;
                m_classImages[index] = device->wrapTexture(
                    texture,
                    RHI::ImageKind::Color2D,
                    RHI::ImageFormat::RGBA8,
                    m_classes[index].size,
                    m_classes[index].size
                );
                resources.push_back(
                    {TEXTURE_CLASS_BINDING + index,
                     RHI::BindingType::SampledImage,
                     {},
                     m_classImages[index],
                     m_textureSampler,
                     0}
                );
            }
            m_textureGroup = device->createBindGroup(m_textureLayout, resources);
        }

        void OpenGLBackend::fillRhiFrame(const Frame& inFrame)
        {
            auto* device                = static_cast<OpenGLRHIDevice*>(m_rhi.get());
            m_rhiFrame.commands         = device->commandList();
            m_rhiFrame.frameIndex       = m_currentFrameIndex;
            m_rhiFrame.width            = m_targetWidth;
            m_rhiFrame.height           = m_targetHeight;
            m_rhiFrame.cameraBuffer     = m_cameraBuffer;
            m_rhiFrame.lightBuffer      = m_lightBuffer;
            m_rhiFrame.instance3DBuffer = m_instanceBuffer;
            m_rhiFrame.particleBuffer   = m_particleBuffer;
            m_rhiFrame.textureTable     = m_textureGroup;
            m_rhiFrame.sceneColor       = m_sceneColor;
            m_rhiFrame.sceneDepth       = m_sceneDepth;
            m_rhiFrame.presentColor     = m_presentColor;
            m_rhiFrame.linearSampler    = m_linearSampler;

            View camera = inFrame.getCamera();
            camera.depthZeroToOne();
            m_rhi->updateBuffer(m_cameraBuffer, &camera, sizeof(View));

            ShadowLight light =
                Shadow::build(inFrame.getCamera(), inFrame.getLights(), inFrame.hasFeature(RendererFeature::Light));
            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; cascade++)
            {
                Mat4 depth                 = Mat4::One;
                depth[2][2]                = 0.5f;
                depth[3][2]                = 0.5f;
                light.projections[cascade] = depth * light.projections[cascade];
            }
            m_rhi->updateBuffer(m_lightBuffer, &light, sizeof(ShadowLight));

            const DrawPoly3DInstance::List& instances = inFrame.getInstances3D();
            if (!instances.empty())
            {
                m_rhi->updateBuffer(m_instanceBuffer, instances.data(), sizeof(DrawPoly3DInstance) * instances.size());
            }

            const DrawParticle::List& particles = inFrame.getParticles();
            const std::uint32_t       count     = std::min(static_cast<std::uint32_t>(particles.size()), MAX_PARTICLES);
            if (count > 0)
            {
                m_rhi->updateBuffer(m_particleBuffer, particles.data(), sizeof(DrawParticle) * count);
            }

            if (!inFrame.getInstances2D().empty())
            {
                m_rhi->updateBuffer(
                    m_rhiFrame.instance2DBuffer,
                    inFrame.getInstances2D().data(),
                    sizeof(DrawPoly2DInstance) * inFrame.getInstances2D().size()
                );
            }
        }

        void OpenGLBackend::destroyRhiResources()
        {
            if (!m_rhi)
            {
                return;
            }

            auto killBuffer = [&](RHI::Buffer& inBuffer)
            {
                if (inBuffer.handle)
                {
                    m_rhi->destroyBuffer(inBuffer);
                    inBuffer = {};
                }
            };
            auto killImage = [&](RHI::Image& inImage)
            {
                if (inImage.handle)
                {
                    m_rhi->destroyImage(inImage);
                    inImage = {};
                }
            };

            killBuffer(m_cameraBuffer);
            killBuffer(m_lightBuffer);
            killBuffer(m_instanceBuffer);
            killBuffer(m_particleBuffer);
            killBuffer(m_textureTableBuffer);
            if (m_rhiFrame.instance2DBuffer.handle)
            {
                m_rhi->destroyBuffer(m_rhiFrame.instance2DBuffer);
                m_rhiFrame.instance2DBuffer = {};
            }
            for (RHI::Image& image : m_classImages)
            {
                killImage(image);
            }
            killImage(m_sceneColor);
            killImage(m_sceneDepth);
            killImage(m_presentColor);
            if (m_textureGroup.handle)
            {
                m_rhi->destroyBindGroup(m_textureGroup);
                m_textureGroup = {};
            }
            if (m_textureLayout.handle)
            {
                m_rhi->destroyBindGroupLayout(m_textureLayout);
                m_textureLayout = {};
            }
            if (m_linearSampler.handle)
            {
                m_rhi->destroySampler(m_linearSampler);
                m_linearSampler = {};
            }
            if (m_textureSampler.handle)
            {
                m_rhi->destroySampler(m_textureSampler);
                m_textureSampler = {};
            }
        }

        void OpenGLBackend::buildFrames()
        {
            frames.resize(getRenderer()->getFrameInFlighCount());
            m_currentFrameIndex = 0U;
        }

        void OpenGLBackend::destroyFrames()
        {
            frames.clear();
            m_currentFrameIndex = 0U;
        }

        void OpenGLBackend::buildLayers()
        {
            ListPush<Layer*> settings;

            settings.strategy = ListPushStrategy::Front;
            addLayer<LScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<LUI>(settings);
        }

        std::uint16_t OpenGLBackend::toGLDepthCompare(DepthCompare inValue) const
        {
            switch (inValue)
            {
            case DepthCompare::Less:
                return GL_LESS;

            case DepthCompare::Equal:
                return GL_EQUAL;

            case DepthCompare::LessOrEqual:
                return GL_LEQUAL;

            case DepthCompare::Greater:
                return GL_GREATER;

            case DepthCompare::NotEqual:
                return GL_NOTEQUAL;

            case DepthCompare::GreaterOrEqual:
                return GL_GEQUAL;

            case DepthCompare::Always:
                return GL_ALWAYS;
            }

            return GL_NEVER;
        }

        std::uint16_t OpenGLBackend::toGLFrontFace(CullingFrontFace inValue) const
        {
            switch (inValue)
            {
            case CullingFrontFace::CounterClockwise:
                return GL_CCW;
            }

            return GL_CW;
        }

        std::uint16_t OpenGLBackend::toGLCullingMode(CullingMode inValue) const
        {
            switch (inValue)
            {
            case CullingMode::Front:
                return GL_FRONT;

            case CullingMode::Back:
                return GL_BACK;

            case CullingMode::FrontAndBack:
                return GL_FRONT_AND_BACK;
            }

            return GL_NONE;
        }

        std::uint16_t OpenGLBackend::toGLFactor(BlendingFactor inValue) const
        {
            switch (inValue)
            {
            case BlendingFactor::One:
                return GL_ONE;

            case BlendingFactor::SrcColor:
                return GL_SRC_COLOR;

            case BlendingFactor::OneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;

            case BlendingFactor::DstColor:
                return GL_DST_COLOR;

            case BlendingFactor::OneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;

            case BlendingFactor::SrcAlpha:
                return GL_SRC_ALPHA;

            case BlendingFactor::OneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;

            case BlendingFactor::DstAlpha:
                return GL_DST_ALPHA;

            case BlendingFactor::OneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;

            case BlendingFactor::ConstantColor:
                return GL_CONSTANT_COLOR;

            case BlendingFactor::OneMinusConstantColor:
                return GL_ONE_MINUS_CONSTANT_COLOR;

            case BlendingFactor::ConstantAlpha:
                return GL_CONSTANT_ALPHA;

            case BlendingFactor::OneMinusConstantAlpha:
                return GL_ONE_MINUS_CONSTANT_ALPHA;

            case BlendingFactor::SrcAlphaSaturate:
                return GL_SRC_ALPHA_SATURATE;

            case BlendingFactor::Src1Color:
                return GL_SRC1_COLOR;

            case BlendingFactor::OneMinusSrc1Color:
                return GL_ONE_MINUS_SRC1_COLOR;

            case BlendingFactor::Src1Alpha:
                return GL_SRC1_ALPHA;

            case BlendingFactor::OneMinusSrc1Alpha:
                return GL_ONE_MINUS_SRC1_ALPHA;
            }

            return GL_ZERO;
        }

        std::uint32_t OpenGLBackend::toGLDrawTopology(DrawPolyTopology inValue) const
        {
            switch (inValue)
            {
            case DrawPolyTopology::PointList:
                return GL_POINTS;

            case DrawPolyTopology::LineList:
                return GL_LINES;

            case DrawPolyTopology::LineStrip:
                return GL_LINE_STRIP;

            case DrawPolyTopology::TriangleStrip:
                return GL_TRIANGLE_STRIP;

            case DrawPolyTopology::TriangleFan:
                return GL_TRIANGLE_FAN;

            case DrawPolyTopology::LineListWithAdjacency:
                return GL_LINES_ADJACENCY;

            case DrawPolyTopology::LineStripWithAdjacency:
                return GL_LINE_STRIP_ADJACENCY;

            case DrawPolyTopology::TriangleListWithAdjacency:
                return GL_TRIANGLES_ADJACENCY;

            case DrawPolyTopology::TriangleStripWithAdjacency:
                return GL_TRIANGLE_STRIP_ADJACENCY;

            case DrawPolyTopology::PatchList:
                return GL_PATCHES;
            }

            return GL_TRIANGLES;
        }
    }
}