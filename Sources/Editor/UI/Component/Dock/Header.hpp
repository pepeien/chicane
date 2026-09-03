#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class DockHeader : public Chicane::Grid::Container
    {
    public:
        // Properties
        static constexpr inline const char* LABEL_ATTRIBUTE_NAME = "label";

        // Values
        static constexpr inline const char* PIN_STATE_UNPINNED_VALUE = "unpinned";
        static constexpr inline const char* PIN_STATE_PINNED_VALUE   = "pinned";

    public:
        CH_CONSTRUCTOR()
        DockHeader(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        bool isPinned() const;

        CH_FUNCTION()
        void onPin();

        CH_FUNCTION()
        void onClose();

    private:
        void refreshLabel();

    public:
        CH_FIELD()
        Chicane::String label;
        CH_FIELD()
        Chicane::String pinState;
    };
}
