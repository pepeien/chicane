#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"

namespace Chicane
{
    struct MatrixUBO
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    struct VectorUBO
    {
        glm::vec4 forward = { 1.0f, 0.0f, 0.0f, 0.0f };
        glm::vec4 right   = { 0.0f, -1.0f, 0.0f, 0.0f };
        glm::vec4 up      = { 0.0f, 0.0f, 1.0f, 0.0f };
    };

    struct UBOBundle
    {
        size_t allocationSize;
        void* writeLocation;
        Buffer::Instance buffer;
    };

    struct MatrixUBOBundle : public UBOBundle
    {
        MatrixUBO instance;
    };

    struct VectorUBOBundle : public UBOBundle
    {
        VectorUBO instance;
    };

    class Camera
    {
    public:
        Camera();

    public:
        void setPosition(const glm::vec3& inPosition);
        glm::vec3 getPosition();

        void addYaw(float inYaw);
        void addPitch(float inPitch);
        
        void setResolution(uint32_t inWidth, uint32_t inHeight);

        VectorUBO getVectorUBO();
        MatrixUBO getMatrixUBO();

    private:
        glm::mat4 generateView();
        glm::mat4 generateProjection();

        void updateUBOs();

    private:
        MatrixUBO m_matrixUBO;
        VectorUBO m_vectorUBO;

        glm::vec3 m_position;
        glm::vec3 m_aim;
        glm::vec3 m_up;

        uint32_t m_width;
        uint32_t m_height;
    };
}