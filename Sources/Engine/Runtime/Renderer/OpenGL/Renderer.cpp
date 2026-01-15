#include "Chicane/Runtime/Renderer/OpenGL/Renderer.hpp"

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Box/Asset/Mesh/Parsed.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"

#include "Chicane/Core/Log.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Renderer/OpenGL/Debug.hpp"
#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

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
            destroyCameraData();
            destroyLightData();
            destroyMeshData();

            destroyContext();
        }

        void Renderer::onInit()
        {
            buildContext();
            buildGlew();

            glEnable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            buildDebugMessenger();
            buildLayers();
            buildCameraData();
            buildLightData();
            buildMeshData();
        }

        void Renderer::onRender()
        {
            rebuildMeshData();

            std::vector<Box::MeshParsed> meshes;
            for (CMesh* mesh : m_meshes)
            {
                Box::MeshParsed data;
                data.modelMatrix  = mesh->getTransform().getMatrix();
                data.textureIndex = Box::getTextureManager()->getIndex(mesh->getTexture());

                meshes.push_back(data);
            }

            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(RendererView), &m_camera->getData());

            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(RendererView), &m_light->getData());

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Box::MeshParsed) * meshes.size(), meshes.data());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearDepth(1.0);
            glClearStencil(0);
            glDepthRange(0.0f, 1.0f);

            for (RendererLayer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->render(nullptr);
            }

            SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_window->getInstance()));
        }

        void Renderer::onResizing()
        {
            glViewport(m_position.x, m_position.y, m_size.x, m_size.y);
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
        }

        void Renderer::buildDebugMessenger()
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_CALLBACK_FUNCTION_ARB);

            glDebugMessageCallback(Debug::debugCallback, nullptr);
        }

        void Renderer::buildLayers()
        {
            pushLayer<LScene>();
        }

        void Renderer::buildCameraData()
        {
            glGenBuffers(1, &m_cameraBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(RendererView), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer);
        }

        void Renderer::destroyCameraData()
        {
            glDeleteBuffers(1, &m_cameraBuffer);
        }

        void Renderer::buildLightData()
        {
            glGenBuffers(1, &m_lightBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(RendererView), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        }

        void Renderer::destroyLightData()
        {
            glDeleteBuffers(1, &m_lightBuffer);
        }

        void Renderer::buildMeshData()
        {
            glGenBuffers(1, &m_meshBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Box::MeshParsed) * m_meshes.size(), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_meshBuffer);
        }

        void Renderer::destroyMeshData()
        {
            glDeleteBuffers(1, &m_meshBuffer);
        }

        void Renderer::rebuildMeshData()
        {
            destroyMeshData();
            buildMeshData();
        }
    }
}