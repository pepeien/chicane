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
            : Backend<Frame>()
        {}

        OpenGLBackend::~OpenGLBackend()
        {
            // Layers
            deleteLayers();

            // OpenGL
            destroyTextureData();
            destroyContext();
        }

        void OpenGLBackend::onInit()
        {
            buildContext();
            buildGlew();
            enableFeatures();
            buildTextureData();
            buildLayers();

            Backend::onInit();
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

        void OpenGLBackend::onSetup()
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

        void OpenGLBackend::onCleanup()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_window->getInstance());

            if (!SDL_GL_SwapWindow(window))
            {
                throw std::runtime_error(std::string("Failed to swawp window frame buffer [") + SDL_GetError() + "]");
            }
        }

        void OpenGLBackend::buildContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_window->getInstance());

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
            SDL_Window* window = static_cast<SDL_Window*>(m_window->getInstance());

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

        void OpenGLBackend::buildTextureData()
        {
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texturesBuffer);
            glTextureStorage3D(m_texturesBuffer, 1, GL_RGBA8, 512, 512, 64);

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
            ListPush<Layer<Frame>*> settings;

            settings.strategy = ListPushStrategy::Front;
            addLayer<OpenGLLScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<OpenGLLGrid>(settings);
        }
    }
}