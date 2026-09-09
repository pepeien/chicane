#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Box/Sky.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        ASky();

    protected:
        void onLoad() override;

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);

        CLight* getEnvironmentLight() const;

        float getEnvironmentIntensity() const;
        void setEnvironmentIntensity(float inIntensity);

    protected:
        const Box::Sky* m_asset;
        CLight*         m_environment;
        float           m_environmentIntensity;
    };
}
