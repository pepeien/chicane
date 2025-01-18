#pragma once

#include "Chicane/Renderer/View/Frustum.hpp"

namespace Chicane
{
    namespace View
    {
        struct SphereFrustum : public Frustum
        {
        public:
            bool contains(const Transformable* inSubject) const override;
            void use(const ViewComponent* inView) override;

        protected:
            bool isWithinPlane(
                const Plane& inPlane,
                const Vec<3, float> inPoint
            ) const override;

        public:
            float radius = 1.0f;
        };
    }
}