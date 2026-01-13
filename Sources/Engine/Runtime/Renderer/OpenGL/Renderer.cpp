#include "Chicane/Runtime/Renderer/OpenGL/Renderer.hpp"

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Core/Log.hpp"
#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Renderer/OpenGL/Debug.hpp"
#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        static SDL_GLContext g_context;

        Renderer::Renderer()
            : Super()
        {
            Application::getWindow()->watchSize([this](Vec<2, int> inSize) { setSize(inSize); });
        }

        Renderer::~Renderer()
        {
            destroyContext();
        }

        void Renderer::onInit()
        {
            buildContext();
            buildGlew();
            buildDebugMessenger();
            buildLayers();
        }

        void Renderer::onRender()
        {
            CCamera* camera = nullptr;
            for (CCamera* component : m_cameras)
            {
                if (!component->isActive())
                {
                    continue;
                }

                camera = component;

                break;
            }

            for (RendererLayer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                layer->render(camera);
            }

            SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_window->getInstance()));
        }

        void Renderer::onResizing()
        {
            glViewport(0, 0, m_size.x, m_size.y);
        }

        void Renderer::buildContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>((m_window->getInstance()));

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

        void Renderer::destroyContext()
        {
            if (!SDL_GL_MakeCurrent(static_cast<SDL_Window*>((m_window->getInstance())), nullptr))
            {
                throw std::runtime_error(std::string("Failed to unref context [") + SDL_GetError() + "]");
            }

            if (!SDL_GL_DestroyContext(g_context))
            {
                throw std::runtime_error(std::string("Failed to destroy context [") + SDL_GetError() + "]");
            }
        }

        void Renderer::buildGlew()
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

            glGetError();
        }

        void Renderer::buildDebugMessenger()
        {
            glEnable(GL_DEBUG_OUTPUT);

            glDebugMessageCallback(Debug::debugCallback, nullptr);
        }

        void Renderer::buildLayers()
        {
            pushLayer<LScene>();
        }
    }
}