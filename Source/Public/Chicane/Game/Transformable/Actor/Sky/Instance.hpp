#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    class ASky : public Actor
    {
    public:
        ASky();
        virtual ~ASky() = default;

    public:
        const Box::Sky::Instance* getSky() const;
        void setSky(const Box::Sky::Instance* inSky);

    protected:
        const Box::Sky::Instance* m_asset;
    };
}