#include "Chicane/Renderer/Backend/OpenGL.hpp"

#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Debug.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/UI.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static SDL_GLContext g_context;

        OpenGLBackend::OpenGLBackend()
            : Backend()
        {}

        void OpenGLBackend::onInit()
        {
            buildContext();
            buildGlad();
            enableFeatures();
            updateResourcesBudget();
            buildTextureData();
            buildLayers();
        }

        void OpenGLBackend::onShutdown()
        {
            // Layers
            destroyLayers();

            // OpenGL
            destroyTextureData();
            destroyContext();
        }

        void OpenGLBackend::onLoad(const DrawTextureResource& inResources)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            GLubyte zero[4] = {0, 0, 0, 0};
            glClearTexImage(m_texturesBuffer, 0, GL_RGBA, GL_UNSIGNED_BYTE, zero);

            for (const DrawTexture& texture : inResources.getDraws())
            {
                glTextureSubImage3D(
                    m_texturesBuffer,
                    0,
                    0,
                    0,
                    texture.id,
                    512,
                    512,
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    texture.image.getPixels()
                );
            }

            Backend::onLoad(inResources);
        }

        void OpenGLBackend::onBeginRender()
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

            glBindTextureUnit(0, m_texturesBuffer);
        }

        void OpenGLBackend::onRender(const Frame& inFrame)
        {
            renderLayers(inFrame);
        }

        void OpenGLBackend::onEndRender()
        {
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
                GLint mem[4] = {};
                glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, mem);

                VRAM = static_cast<size_t>(mem[0]) * 1024;
            }

            setVRAM(VRAM);
        }

        void OpenGLBackend::buildTextureData()
        {
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texturesBuffer);
            glTextureStorage3D(m_texturesBuffer, 1, GL_RGBA8, 512, 512, TEXTURE_COUNT);

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_R, GL_REPEAT);
        }

        void OpenGLBackend::destroyTextureData()
        {
            glDeleteTextures(1, &m_texturesBuffer);
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