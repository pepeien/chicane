#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class OutlinerTab : public Chicane::Grid::Component
    {
    public:
        // Attributes
        static constexpr inline const char* LABEL_ATTRIBUTE_NAME = "label";

        // Values
        static constexpr inline const char* VISILITY_STATUS_HIDDEN  = "hidden";
        static constexpr inline const char* VISILITY_STATUS_VISIBLE = "visible";

    public:
        CH_CONSTRUCTOR()
        OutlinerTab(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onDropdownClick();

    private:
        void refreshLabel();

    public:
        CH_FIELD()
        Chicane::String contentVisibility;

        CH_FIELD()
        Chicane::String label;
    };
}
