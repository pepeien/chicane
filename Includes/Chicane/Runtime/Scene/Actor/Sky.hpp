#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Box/Sky.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Sky")
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        static constexpr inline const char* TAG_ID = "ASky";

    public:
        CH_CONSTRUCTOR()
        ASky();

    protected:
        void onLoad() override;
        void onPropertyEdited(const String& inName) override;

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);

        CLight* getEnvironmentLight() const;

        float getIntensity() const;
        void setIntensity(float inValue);

        float getExposure() const;
        void setExposure(float inValue);

    public:
        CH_FIELD()
        FileSystem::Path sky;

        CH_FIELD()
        float intensity;

        CH_FIELD()
        float exposure;

    protected:
        const Box::Sky* m_asset;
        CLight*         m_environment;
    };
}
