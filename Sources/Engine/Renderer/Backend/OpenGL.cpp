#include "Chicane/Renderer/Backend/OpenGL.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Debug.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/UI.hpp"

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
            destroyFrames();

            // OpenGL
            destroyGpuQueries();
            destroyTextureData();
            destroyTarget();
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
            Backend::onLoad(inResources);
        }

        void OpenGLBackend::onBeginRender()
        {
            resolveGpuQuery(m_gpuQueryWrite);
            beginGpuQuery();

            buildTarget();
            bindTarget();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            bindTextureTable();
        }

        void OpenGLBackend::onRender(const Frame& inFrame)
        {
            OpenGLFrame& nextFrame = frames.at(m_currentFrameIndex);

            renderLayers(
                inFrame,
                &nextFrame,
                [](const Layer* inLayer) { return !inLayer->getId().equals(UI_LAYER_ID); }
            );

            presentTarget(!isScreenComposited(inFrame));
            bindTextureTable();

            renderLayers(
                inFrame,
                &nextFrame,
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

                GLint result = GL_FALSE;
                glShaderBinary(1, &module, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), code.size());
                glSpecializeShader(module, "main", 0, nullptr, nullptr);
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
                GLint mem[4];
                glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, mem);

                VRAM = static_cast<size_t>(mem[0]) * 1024;
            }

            setVRAM(VRAM);
        }

        void OpenGLBackend::buildTextureData()
        {
            static_assert(sizeof(TextureTableEntry) == 8, "GLSL uvec2 table entries must be 8 bytes");

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
                static_cast<GLsizeiptr>(sizeof(TextureTableEntry) * slotCount),
                nullptr,
                GL_DYNAMIC_STORAGE_BIT
            );

            std::vector<TextureTableEntry> entries(slotCount);
            glNamedBufferSubData(
                m_textureTable,
                0,
                static_cast<GLsizeiptr>(entries.size() * sizeof(TextureTableEntry)),
                entries.data()
            );

            m_textures.clear();
        }

        void OpenGLBackend::destroyTextureData()
        {
            m_textures.clear();

            for (SizeClass& sizeClass : m_classes)
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

        void OpenGLBackend::releaseTextureSlot(TextureSlot& inSlot)
        {
            if (inSlot.classIndex < TEXTURE_CLASS_COUNT)
            {
                SizeClass& sizeClass = m_classes[inSlot.classIndex];
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

        void OpenGLBackend::writeTextureSlot(Draw::Id inId, std::uint32_t inClass, std::uint32_t inLayer)
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

            const TextureTableEntry entry{inClass, inLayer};
            glNamedBufferSubData(
                m_textureTable,
                static_cast<GLintptr>(static_cast<std::size_t>(inId) * sizeof(TextureTableEntry)),
                sizeof(TextureTableEntry),
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

        void OpenGLBackend::createClassArray(SizeClass& inClass, std::uint32_t inLayers)
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

        void OpenGLBackend::fillWhiteLayer(const SizeClass& inClass, std::uint32_t inLayer) const
        {
            if (inClass.texture == 0)
            {
                return;
            }

            const std::uint32_t mipLevels = Image::mipCount(inClass.size, inClass.size);
            for (std::uint32_t level = 0; level < mipLevels; level++)
            {
                const std::uint32_t width = Image::mipDimension(inClass.size, level);
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
            SizeClass& sizeClass = m_classes[inClass];
            const std::uint32_t layers = std::min(std::max(inLayers, sizeClass.allocated + 1), m_maxArrayLayers);
            if (layers <= sizeClass.allocated && sizeClass.texture != 0)
            {
                return;
            }

            const std::uint32_t previous = sizeClass.texture;
            const std::uint32_t oldCount = sizeClass.allocated;
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
            SizeClass& sizeClass = m_classes[inClass];
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
                const std::uint32_t next = std::min(
                    std::max(sizeClass.allocated * 2, sizeClass.allocated + 4),
                    m_maxArrayLayers
                );
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

            TextureSlot& slot = m_textures[static_cast<std::size_t>(inTexture.id)];
            if (slot.classIndex == classIndex && slot.sourceWidth == inTexture.width &&
                slot.sourceHeight == inTexture.height && slot.residentMinMip == inTexture.residentMinMip)
            {
                return;
            }

            releaseTextureSlot(slot);

            const std::uint32_t layer     = allocateLayer(classIndex);
            SizeClass&          sizeClass = m_classes[classIndex];
            if (layer == ~0u || sizeClass.texture == 0)
            {
                writeTextureSlot(inTexture.id, 0, 0);

                return;
            }

            const std::uint32_t mipLevels = Image::mipCount(sizeClass.size, sizeClass.size);

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

                const std::uint32_t levelWidth = Image::mipDimension(sizeClass.size, gpuLevel);
                std::vector<Image::Pixel> staging(static_cast<std::size_t>(levelWidth) * levelWidth * 4, 255);
                if (image && image->getPixels())
                {
                    if (image->getWidth() == static_cast<int>(levelWidth) &&
                        image->getHeight() == static_cast<int>(levelWidth))
                    {
                        std::memcpy(staging.data(), image->getPixels(), staging.size());
                    }
                    else
                    {
                        image->blit(staging.data(), static_cast<int>(levelWidth), static_cast<int>(levelWidth));
                    }
                }

                glTextureSubImage3D(
                    sizeClass.texture,
                    static_cast<GLint>(gpuLevel),
                    0,
                    0,
                    static_cast<GLint>(layer),
                    static_cast<GLsizei>(levelWidth),
                    static_cast<GLsizei>(levelWidth),
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
            writeTextureSlot(inTexture.id, classIndex, layer);
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
            glTextureStorage2D(m_targetColor, 1, GL_RGBA8, width, height);
            glTextureParameteri(m_targetColor, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_targetColor, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_targetColor, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_targetColor, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glCreateRenderbuffers(1, &m_targetDepth);
            glNamedRenderbufferStorage(m_targetDepth, GL_DEPTH24_STENCIL8, width, height);

            glCreateFramebuffers(1, &m_targetFramebuffer);
            glNamedFramebufferTexture(m_targetFramebuffer, GL_COLOR_ATTACHMENT0, m_targetColor, 0);
            glNamedFramebufferRenderbuffer(
                m_targetFramebuffer,
                GL_DEPTH_STENCIL_ATTACHMENT,
                GL_RENDERBUFFER,
                m_targetDepth
            );

            glCreateFramebuffers(1, &m_screenBlitFramebuffer);

            m_targetWidth  = width;
            m_targetHeight = height;
        }

        void OpenGLBackend::destroyTarget()
        {
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
                glDeleteRenderbuffers(1, &m_targetDepth);
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

        void OpenGLBackend::presentTarget(bool inShouldPresentToWindow) const
        {
            if (m_targetFramebuffer == 0)
            {
                return;
            }

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glDrawBuffer(GL_BACK);

            if (inShouldPresentToWindow)
            {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, m_targetFramebuffer);
                glReadBuffer(GL_COLOR_ATTACHMENT0);
                glBlitFramebuffer(
                    0,
                    0,
                    static_cast<GLint>(m_targetWidth),
                    static_cast<GLint>(m_targetHeight),
                    0,
                    0,
                    static_cast<GLint>(m_targetWidth),
                    static_cast<GLint>(m_targetHeight),
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST
                );
            }
            else
            {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void OpenGLBackend::captureScreenTarget()
        {
            return;
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
            addLayer<OpenGLLScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<OpenGLLUI>(settings);
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