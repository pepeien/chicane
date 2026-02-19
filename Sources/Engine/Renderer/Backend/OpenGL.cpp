#include "Chicane/Renderer/Backend/OpenGL.hpp"

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Debug.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Grid.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"

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
            buildGlew();
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

        Viewport OpenGLBackend::getGLViewport(Layer* inLayer)
        {
            const Vec<2, std::uint32_t> resolution = getRenderer()->getResolution();

            Viewport result   = getLayerViewport(inLayer);
            result.position.y = resolution.y - (result.position.y + result.size.y);

            return result;
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
                throw std::runtime_error(std::string("Failed to disabled V-Sync [") + SDL_GetError() + "]");
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

        void OpenGLBackend::buildGlew()
        {
            glewExperimental = true;

            GLenum result = glewInit();
            if (result != GLEW_OK)
            {
                throw std::runtime_error(
                    std::string("Failed to initialize OpenGL [") +
                    reinterpret_cast<const char*>(glewGetErrorString(result)) + "]"
                );
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
            if (GLEW_NVX_gpu_memory_info)
            {
                GLint total = 0;
                glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);

                VRAM = static_cast<size_t>(total) * 1024;
            }

            // AMD
            if (GLEW_ATI_meminfo)
            {
                GLint mem[4] = {};
                glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, mem);

                VRAM = static_cast<size_t>(mem[0]) * 1024;
            }

            setResourceBudget(VRAM);
        }

        void OpenGLBackend::buildTextureData()
        {
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texturesBuffer);
            glTextureStorage3D(m_texturesBuffer, 1, GL_RGBA8, 512, 512, Backend::TEXTURE_COUNT);

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
            addLayer<OpenGLLGrid>(settings);
        }
    }
}