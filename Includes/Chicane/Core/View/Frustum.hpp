#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Transformable.hpp"
#include "Chicane/Core/View/Plane.hpp"
#include "Chicane/Core/View/Settings.hpp"

namespace Chicane
{
    struct CHICANE_CORE ViewFrustum
    {
    public:
        virtual bool contains(const Transformable* inSubject) const;
        virtual void update(const Transformable* inView, const ViewSettings& inSettings);

    protected:
        ViewPlane m_top    = {};
        ViewPlane m_bottom = {};
        ViewPlane m_left   = {};
        ViewPlane m_right  = {};
        ViewPlane m_near   = {};
        ViewPlane m_far    = {};
    };
}