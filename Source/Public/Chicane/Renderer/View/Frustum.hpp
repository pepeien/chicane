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
        	virtual bool contains(const Transformable* inSubject) const;
            virtual void update(const CView* inView);

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