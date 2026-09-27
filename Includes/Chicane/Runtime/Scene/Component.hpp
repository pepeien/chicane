#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME Component : public Object
    {
    public:
        static constexpr inline const char* TAG_ID = "Component";

    public:
        CH_CONSTRUCTOR()
        Component();

    protected:
        inline virtual void onActivation() { return; }
        inline virtual void onDeactivation() { return; }

    public:
        CH_FUNCTION()
        bool isActive() const;

        CH_FUNCTION()
        void activate();

        CH_FUNCTION()
        void deactivate();

    protected:
        bool m_bIsActive;
    };
}