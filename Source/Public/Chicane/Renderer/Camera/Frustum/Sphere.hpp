#pragma once

#include "Chicane/Renderer/Camera/Frustum.hpp"

namespace Chicane
{
    namespace Camera
    {
        struct SphereFrustum : public Frustum
        {
        public:
            bool contains(const Transformable* inSubject) const override;
            void use(const CameraComponent* inCamera) override;

        protected:
            bool isWithinPlane(
                const Plane& inPlane,
                const Vec<3, float> inCenter,
                float inRadius
            ) const override;

        public:
            float radius = 1.0f;
        };
    }
}