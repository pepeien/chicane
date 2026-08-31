#pragma once

#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Editor/UI/Component/Header/Menu/Item.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class HeaderMenu : public Chicane::Grid::Container
    {
    public:
        // Tag
        static constexpr inline const char* TAG_ID = "HeaderMenu";

    public:
        CH_CONSTRUCTOR()
        HeaderMenu(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;
        void onHover() override;

    public:
        CH_FUNCTION()
        void onActivate();

        CH_FUNCTION()
        void toggle();

        CH_FUNCTION()
        void open();

        CH_FUNCTION()
        void close();

        CH_FUNCTION()
        bool hasSubmenu() const;

        CH_FUNCTION()
        bool showChevron() const;

        CH_FUNCTION()
        bool hasShortcut() const;

    private:
        bool isNested() const;
        bool hasOpenSibling() const;

        const HeaderMenuItem* getItem() const;
        HeaderMenu* findParentMenu() const;

        void closeSiblings();
        void closeRoot();

        void ensureSubmenu();
        void refreshItem();
        void bindSubmenu();

    public:
        CH_FIELD()
        bool bIsOpen;

        CH_FIELD()
        bool bShowChevron;

        CH_FIELD()
        bool bHasShortcut;

        CH_FIELD()
        Chicane::String label;

        CH_FIELD()
        Chicane::String shortcut;

    private:
        bool m_bHasSubmenuList;
    };
}
