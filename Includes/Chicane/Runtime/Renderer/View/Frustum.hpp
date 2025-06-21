#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer/View/Plane.hpp"

namespace Chicane
{
    class CView;
    class Transformable;

    struct CHICANE_RUNTIME RendererViewFrustum
    {
    public:
    	virtual bool contains(const Transformable* inSubject) const;
        virtual void update(const CView* inView);

    protected:
        RendererViewPlane m_top    = {};
        RendererViewPlane m_bottom = {};
        RendererViewPlane m_left   = {};
        RendererViewPlane m_right  = {};
        RendererViewPlane m_near   = {};
        RendererViewPlane m_far    = {};
    };
}