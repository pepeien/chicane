#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Camera/Plane.hpp"

namespace Chicane
{
    class CameraComponent;
    class Transformable;

    namespace Camera
    {
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