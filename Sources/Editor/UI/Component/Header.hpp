#pragma once

#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Editor/UI/Component/Header/Menu.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class Header : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Header(const pugi::xml_node& inNode);

        ~Header() override;

    public:
        bool isFocusable() const override;

        bool onEvent(const Chicane::WindowEvent& inEvent) override;

    protected:
        void onTick(float inDeltaTime) override;
        void onBlur() override;

    public:
        CH_FUNCTION()
        void onMinimize();

        CH_FUNCTION()
        void onMaximize();

        CH_FUNCTION()
        void onClose();

    private:
        bool isControlHit(const Chicane::Vec2& inLocation) const;
        void closeMenus();
        void bindMoveHitTest();
        void unbindMoveHitTest();

        void initSettingsMenu();

    public:
        CH_FIELD()
        Chicane::String maximizeState;

        CH_FIELD()
        HeaderMenuItem::List menus;

    private:
        void* m_moveWindow;
    };
}
