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

        OpenGLBackend::OpenGLBackend(const Instance* inRenderer)
            : Backend(inRenderer)
        {}

        OpenGLBackend::~OpenGLBackend()
        {
            destroyContext();
        }

        void OpenGLBackend::onInit()
        {
            buildContext();
            buildGlew();
            enableFeatures();
            buildLayers();
        }

        void OpenGLBackend::onSetup(const Frame& inFrame, const DrawResource::Map& inResources)
        {
            const Viewport& viewport = m_renderer->getViewport();
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            glClearDepth(1.0);
            glClearStencil(0);
            glDepthRange(0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        void OpenGLBackend::onCleanup()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_renderer->getWindow()->getInstance());

            if (!SDL_GL_SwapWindow(window))
            {
                throw std::runtime_error(
                    std::string("Failed to swawp window frame buffer [") + SDL_GetError() + "]"
                );
            }
        }

        void OpenGLBackend::buildContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_renderer->getWindow()->getInstance());

            g_context = SDL_GL_CreateContext(window);

            if (!g_context)
            {
                throw std::runtime_error(
                    std::string("Failed to instantiate context [") + SDL_GetError() + "]"
                );
            }

            if (!SDL_GL_MakeCurrent(window, g_context))
            {
                throw std::runtime_error(
                    std::string("Failed to initialize context [") + SDL_GetError() + "]"
                );
            }

            if (!SDL_GL_SetSwapInterval(0))
            {
                throw std::runtime_error(std::string("Failed to disabled V-Sync [") + SDL_GetError() + "]");
            }
        }

        void OpenGLBackend::destroyContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_renderer->getWindow()->getInstance());

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
                glEnable(GL_DEBUG_CALLBACK_FUNCTION_ARB);
                glDebugMessageCallback(OpenGLDebugCallback, nullptr);
            }
        }

        void OpenGLBackend::buildLayers()
        {
            addLayer<OpenGLLScene>();
            addLayer<OpenGLLGrid>();
        }
    }
}