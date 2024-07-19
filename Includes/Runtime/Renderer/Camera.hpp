#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Renderer/Buffer.hpp"

namespace Chicane
{
    struct VectorUBO
    {
        glm::vec3 forward = { 1.0f, 0.0f, 0.0f };
        glm::vec3 right   = { 0.0f, -1.0f, 0.0f };
        glm::vec3 up      = { 0.0f, 0.0f, 1.0f };
    };

    struct MatrixUBO
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
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
        // State
        void panTo(const glm::vec2& inTarget);
        void rotateTo(const glm::vec2& inTarget);
        void zoomTo(float inTarget);

        glm::vec3 getPosition();
        
        void setViewportResolution(uint32_t inWidth, uint32_t inHeight);

        glm::quat getOrientation();
        glm::vec3 getForward();
        glm::vec3 getRight();
        glm::vec3 getUp();

        VectorUBO getVectorUBO();
        MatrixUBO getMatrixUBO();

    private:
        // State
        glm::vec2 getPanSpeed();
        float getRotationSpeed();
        float getZoomSpeed();

        // Settings
        void setAspectRatio(float inAspectRatio);

        // Render
        glm::mat4 getView();
        glm::mat4 getProjection();

        void updateUBOs();

    private:
        // State
        glm::vec3 m_position;
        glm::vec3 m_focalPoint;

        float m_yaw;
        float m_pitch;

        // Settings
        std::uint32_t m_viewportWidth;
        std::uint32_t m_viewportHeight;

        float m_fov;
        float m_aspectRatio;
        float m_nearClip;
        float m_farClip;
        float m_moveDistance;

        // Render
        MatrixUBO m_matrixUBO;
        VectorUBO m_vectorUBO;
    };
}