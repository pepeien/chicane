#pragma once

#include "Chicane/Box/Sky.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        ASky();

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);

    protected:
        const Box::Sky* m_asset;
    };
}