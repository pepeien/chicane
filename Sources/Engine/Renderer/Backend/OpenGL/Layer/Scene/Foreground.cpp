#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Foreground.hpp"

#include <glad/gl.h>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneForeground::OpenGLLSceneForeground()
            : Layer(SCENE_FOREGROUND_LAYER_ID)
        {}

        bool OpenGLLSceneForeground::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasForegroundDraws();
        }

        void OpenGLLSceneForeground::onRender(const Frame& inFrame, void* inData)
        {
            OpenGLBackend*    backend = getBackend<OpenGLBackend>();
            OpenGLLScene*     scene   = backend->getLayer<OpenGLLScene>(SCENE_LAYER_ID);
            OpenGLLSceneMesh* mesh    = backend->getLayer<OpenGLLSceneMesh>(SCENE_MESH_LAYER_ID);

            if (!scene || !mesh)
            {
                return;
            }

            const std::uint32_t shaderProgram = mesh->getShaderProgram();
            if (shaderProgram == 0)
            {
                return;
            }

            glBindVertexArray(scene->getModelVertexArray());
            glUseProgram(shaderProgram);

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            if (OpenGLLSceneSky* sky = backend->getLayer<OpenGLLSceneSky>(SCENE_SKY_LAYER_ID))
            {
                glBindTextureUnit(3, sky->getCubemap());
            }

            Viewport viewport = backend->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            for (const DrawPoly& draw : inFrame.getForegroundDraws())
            {
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(Vertex::Index) * draw.indexStart),
                    draw.instanceCount,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
        }

        void OpenGLLSceneForeground::onEndRender()
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    }
}
