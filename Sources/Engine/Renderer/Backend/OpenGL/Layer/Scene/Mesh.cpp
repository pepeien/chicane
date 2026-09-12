#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"

#include <algorithm>
#include <string>

#include <glad/gl.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Shader.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneMesh::OpenGLLSceneMesh()
            : Layer(SCENE_MESH_LAYER_ID)
        {}

        void OpenGLLSceneMesh::onInit()
        {
            buildShader();
        }

        void OpenGLLSceneMesh::onDestruction()
        {
            destroyShader();
        }

        bool OpenGLLSceneMesh::onBeginRender(const Frame& inFrame)
        {
            if (!getBackend()->hasFeature(RendererFeature::Fill) || !inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill))
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneMesh::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glClear(GL_DEPTH_BUFFER_BIT);

            OpenGLFrame& frame = *((OpenGLFrame*)inData);

            glBindTextureUnit(3, frame.getObject(SCENE_SKY_LAYER_ID));

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            auto drawMeshes = [&inFrame]()
            {
                for (const DrawPoly& draw : inFrame.getSceneDraws())
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
            };

            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
            glProgramUniform1i(m_shaderProgram, 10, 0);
            drawMeshes();

            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
            glProgramUniform1i(m_shaderProgram, 10, 1);
            drawMeshes();

            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
            glProgramUniform1i(m_shaderProgram, 10, -1);
        }

        void OpenGLLSceneMesh::onEndRender()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);
        }

        void OpenGLLSceneMesh::buildShader()
        {
            Shader::List shaders;
            shaders.push_back({"Assets/Engine/Shaders/OpenGL/Scene/Mesh.overt", ShaderType::Vertex});
            shaders.push_back({"Assets/Engine/Shaders/OpenGL/Scene/Mesh.ofrag", ShaderType::Fragment});

            m_shaderProgram = getBackend<OpenGLBackend>()->initShader(shaders);

            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.addObject(m_id, m_shaderProgram);
            }
        }

        void OpenGLLSceneMesh::destroyShader()
        {
            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.removeObject(m_id);
            }
            glDeleteProgram(m_shaderProgram);
        }
    }
}