#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    struct UBO
    {
        Mat<float>::Four view;
        Mat<float>::Four projection;
        Mat<float>::Four viewProjection;

        Vec<float>::Four forward;
        Vec<float>::Four right;
        Vec<float>::Four up;
    };

    struct UBOBundle
    {
        size_t allocationSize;
        void* writeLocation;
        Buffer::Instance buffer;
        UBO instance;
    };

    class Camera
    {
    public:
        Camera();

    public:
        virtual void onEvent(const SDL_Event& inEvent) { return; };

    public:
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<std::uint32_t>::Two& inViewportResolution);

        // Transform
        Vec<float>::Three getPosition();
        void setPosition(float inX, float inY, float inZ);
        void setPosition(const Vec<float>::Three& inPosition);
        void updatePosition();

        void setRotation(float inPitch, float inRoll, float inYaw);
        void setRotation(const Vec<float>::Three& inRotation);

        // Movement
        void pan(float inX, float inY);
        void pan(const Vec<float>::Two& inDelta);
    
        void zoom(float inDelta);

        // Rotation Movement
        void addPitch(float inPitch);
        void addRoll(float inRoll);
        void addYaw(float inYaw);

        // State
        Quat<float>::Default getOrientation();
        Vec<float>::Three getForward();
        Vec<float>::Three getRight();
        Vec<float>::Three getUp();

        // Render
        UBO getUBO();
        void updateUBO();

    protected:
        // State
        Vec<float>::Two getPanSpeed();
        float getRotationSpeed();
        float getZoomSpeed();

        // Settings
        void setAspectRatio(float inAspectRatio);

        // Render
        Mat<float>::Four getView();
        Mat<float>::Four getProjection();

    protected:
        // State
        Transform m_transform;
        Vec<float>::Three m_focalPoint;

        // Settings
        std::uint32_t m_viewportWidth;
        std::uint32_t m_viewportHeight;

        float m_fov;
        float m_aspectRatio;
        float m_nearClip;
        float m_farClip;
        float m_moveDistance;

        // Render
        UBO m_UBO;
    };
}