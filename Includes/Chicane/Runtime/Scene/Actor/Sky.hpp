#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Box/Sky.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Actor | Sky")
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        // Tag
        static constexpr inline const char* TAG_ID = "ASky";

        // Attributes
        static constexpr inline const char* SKY_ATTRIBUTE_NAME       = "sky";
        static constexpr inline const char* INTENSITY_ATTRIBUTE_NAME = "intensity";
        static constexpr inline const char* EXPOSURE_ATTRIBUTE_NAME  = "exposure";

    public:
        CH_CONSTRUCTOR()
        ASky();

    protected:
        void onLoad() override;
        void onPropertyEdited(const String& inName) override;

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);

        float getIntensity() const;
        void setIntensity(float inValue);

        float getExposure() const;
        void setExposure(float inValue);

        bool isVisible() const;
        void setVisible(bool inValue);

    protected:
        void placeEnvironmentLightAtSun();

    public:
        CH_FIELD()
        FileSystem::Path sky;

        CH_FIELD()
        float intensity;

        CH_FIELD()
        float exposure;

        CH_FIELD()
        bool bIsVisible;

    protected:
        const Box::Sky* m_asset;
        CLight*         m_sun;
    };
}
