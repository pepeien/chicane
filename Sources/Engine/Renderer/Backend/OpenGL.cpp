#include "Chicane/Renderer/Backend/OpenGL.hpp"

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Debug.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct MeshDrawData
        {
        public:
            Mat4 model = Mat4::One;
        };

        static SDL_GLContext g_context;

        OpenGLBackend::OpenGLBackend(const Instance* inRenderer)
            : Backend(inRenderer)
        {}

        OpenGLBackend::~OpenGLBackend()
        {
            destroyCameraData();
            destroyLightData();
            destroyMeshData();

            destroyContext();
        }

        void OpenGLBackend::onInit()
        {
            buildContext();
            buildGlew();
            enableFeatures();
            buildCameraData();
            buildLightData();
            buildMeshData();
            buildLayers();
        }

        void OpenGLBackend::onSetup(const Frame& inFrame)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), &inFrame.getCamera());

            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), &inFrame.getLights().at(0));

            std::vector<MeshDrawData> meshes;
            for (const Draw3D& draw : inFrame.getDraws3D())
            {
                MeshDrawData mesh;
                mesh.model = draw.model;

                meshes.push_back(mesh);
            }

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(MeshDrawData) * meshes.size(),
                meshes.data()
            );

            const Viewport& viewport = m_renderer->getViewport();
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClearDepth(1.0);
            glClearStencil(0);
            glDepthRange(0.0f, 1.0f);
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
                throw std::runtime_error(
                    std::string("Failed to disabled V-Sync [") + SDL_GetError() + "]"
                );
            }
        }

        void OpenGLBackend::destroyContext()
        {
            SDL_Window* window = static_cast<SDL_Window*>(m_renderer->getWindow()->getInstance());

            if (!SDL_GL_MakeCurrent(window, nullptr))
            {
                throw std::runtime_error(
                    std::string("Failed to unref context [") + SDL_GetError() + "]"
                );
            }

            if (!SDL_GL_DestroyContext(g_context))
            {
                throw std::runtime_error(
                    std::string("Failed to destroy context [") + SDL_GetError() + "]"
                );
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
            glEnable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            if (IS_DEBUGGING)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_CALLBACK_FUNCTION_ARB);
                glDebugMessageCallback(OpenGLDebugCallback, nullptr);
            }
        }

        void OpenGLBackend::buildCameraData()
        {
            glGenBuffers(1, &m_cameraBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(View), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer);
        }

        void OpenGLBackend::destroyCameraData()
        {
            glDeleteBuffers(1, &m_cameraBuffer);
        }

        void OpenGLBackend::buildLightData()
        {
            glGenBuffers(1, &m_lightBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(View), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        }

        void OpenGLBackend::destroyLightData()
        {
            glDeleteBuffers(1, &m_lightBuffer);
        }

        void OpenGLBackend::buildMeshData()
        {
            glGenBuffers(1, &m_meshBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                sizeof(MeshDrawData) * 1000,
                NULL,
                GL_DYNAMIC_DRAW
            );
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_meshBuffer);
        }

        void OpenGLBackend::destroyMeshData()
        {
            glDeleteBuffers(1, &m_meshBuffer);
        }

        void OpenGLBackend::buildLayers()
        {
            addLayer<OpenGLLScene>();
        }
    }
}