#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    class CameraComponent;
    class Transformable;

    namespace Camera
    {
        struct Plane
        {
        public:
            float getDistance(const Vec<3, float>& inPoint) const;

            void update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint);

        public:
            float         distance = 0.0f;
            Vec<3, float> normal   = {};
        };

        struct Frustum
        {
        public:
        	virtual bool contains(const Transformable* inSubject) const { return false; }
            virtual void use(const CameraComponent* inCamera) { return; }

        protected:
        	virtual bool isWithinPlane(
                const Plane& inPlane,
                const Vec<3, float> inCenter
            ) const { return false; }

        public:
            Vec<3, float> center = {};

        protected:
            Plane m_top    = {};
            Plane m_bottom = {};
            Plane m_left   = {};
            Plane m_right  = {};
            Plane m_near   = {};
            Plane m_far    = {};
        };
    }
}