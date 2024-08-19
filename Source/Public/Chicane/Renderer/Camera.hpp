#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    class Camera
    {
    public:
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

    public:
        Camera();
        virtual ~Camera() = default;

    public:
        virtual void onEvent(const SDL_Event& inEvent) { return; };

    public:
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<std::uint32_t>::Two& inViewportResolution);

        // Positioning
        const Vec<float>::Three& getTranslation();
        void addTranslation(const Vec<float>::Three& inPosition);
        void addTranslation(float inX, float inY, float inZ);
        void setTranslation(const Vec<float>::Three& inPosition);

        const Vec<float>::Three& getRotation();
        void addRotation(const Vec<float>::Three& inRotation);
        void addRotation(float inRoll, float inYaw, float inPitch);
        void setRotation(const Vec<float>::Three& inRotation);

        // State
        const Vec<float>::Three& getForward();
        const Vec<float>::Three& getRight();
        const Vec<float>::Three& getUp();

        // Render
        const UBO& getUBO();

    protected:
        // Updates
        void onTransformUpdate();

        // Settings
        void setAspectRatio(float inAspectRatio);

    protected:
        // State
        Transform m_transform;

        Vec<float>::Three m_forward;
        Vec<float>::Three m_up;
        Vec<float>::Three m_right;

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