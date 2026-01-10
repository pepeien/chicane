#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/View/Plane.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME RendererViewFrustum
    {
    public:
        virtual bool contains(const class Transformable* inSubject) const;
        virtual void update(const class CView* inView);

    protected:
        RendererViewPlane m_top    = {};
        RendererViewPlane m_bottom = {};
        RendererViewPlane m_left   = {};
        RendererViewPlane m_right  = {};
        RendererViewPlane m_near   = {};
        RendererViewPlane m_far    = {};
    };
}