#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/View/Plane.hpp"

namespace Chicane
{
    class CView;
    class Transformable;

    namespace View
    {
        struct Frustum
        {
        public:
        	virtual bool contains(const Transformable* inSubject) const { return false; }
            virtual void use(const CView* inView) { return; }

        protected:
        	virtual bool isWithinPlane(const Plane& inPlane, const Vec<3, float>& inCenter) const { return false; }

        public:
            Vec<3, float> center   = {};

        protected:
            Plane         m_top    = {};
            Plane         m_bottom = {};
            Plane         m_left   = {};
            Plane         m_right  = {};
            Plane         m_near   = {};
            Plane         m_far    = {};
        };
    }
}