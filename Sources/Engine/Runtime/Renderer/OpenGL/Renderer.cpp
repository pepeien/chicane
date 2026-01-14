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
            buildDebugMessenger();
            buildLayers();

            buildCameraData();
            buildLightData();
            buildMeshData();
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

            CLight* light = nullptr;
            for (CLight* component : m_lights)
            {
                if (!component->isActive())
                {
                    continue;
                }

                light = component;

                break;
            }

            std::vector<Box::MeshParsed> meshes = {};
            meshes.reserve(m_meshes.size());

            std::vector<CMesh*> components = m_meshes;
            std::sort(components.begin(), components.end(), [](CMesh* inA, CMesh* inB) {
                return inA->getModel().compare(inB->getModel()) > 0;
            });

            for (const CMesh* mesh : components)
            {
                Box::MeshParsed data = {};
                data.modelMatrix     = mesh->getTransform().getMatrix();
                data.textureIndex    = Box::getTextureManager()->getIndex(mesh->getTexture());

                meshes.push_back(data);
            }

            for (RendererLayer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(RendererView), &camera->getData());

                glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(RendererView), &light->getData());

                glBindBuffer(GL_UNIFORM_BUFFER, m_meshBuffer);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Box::MeshParsed) * 10, meshes.data());

                layer->render(nullptr);
            }

            SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_window->getInstance()));
        }

        void Renderer::onResizing()
        {
            glViewport(0, 0, m_size.x, m_size.y);
        }

        RendererInternals Renderer::getInternals()
        {
            RendererInternals internals;
            internals.meshBuffer   = m_meshBuffer;
            internals.cameraBuffer = m_cameraBuffer;
            internals.lightBuffer  = m_lightBuffer;

            return internals;
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

        void Renderer::buildCameraData()
        {
            glGenBuffers(1, &m_cameraBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(RendererView), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        }

        void Renderer::destroyLightData()
        {
            glDeleteBuffers(1, &m_lightBuffer);
        }

        void Renderer::buildMeshData()
        {
            glGenBuffers(1, &m_meshBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_meshBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Box::MeshParsed) * 10, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_meshBuffer);
        }

        void Renderer::destroyMeshData()
        {
            glDeleteBuffers(1, &m_meshBuffer);
        }
    }
}