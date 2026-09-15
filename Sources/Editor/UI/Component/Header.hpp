#pragma once

#include <mutex>
#include <vector>

#include <Chicane/Core/Math/Bounds/2D.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Editor/UI/Component/Header/Menu.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Header))
    class Header : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* THEME_ATTRIBUTE                 = "theme";
        static constexpr inline const char* VIEWPORT_TAB_STATE_ATTRIBUTE    = "viewportTabState";
        static constexpr inline const char* ASSETS_TAB_STATE_ATTRIBUTE      = "assetsTabState";
        static constexpr inline const char* ON_WORKSPACE_VIEWPORT_ATTRIBUTE = "onWorkspaceViewport";
        static constexpr inline const char* ON_WORKSPACE_ASSETS_ATTRIBUTE   = "onWorkspaceAssets";

    public:
        CH_CONSTRUCTOR()
        Header(const pugi::xml_node& inNode);

        ~Header() override;

    public:
        bool isFocusable() const override;

        bool onEvent(const Chicane::WindowEvent& inEvent) override;
        void tick(float inDeltaTime) override;

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

        CH_FUNCTION()
        void onWorkspaceViewport();

        CH_FUNCTION()
        void onWorkspaceAssets();

    private:
        bool isControl(const Chicane::Grid::Component* inComponent) const;
        bool isControlHit(const Chicane::Vec2& inLocation) const;
        void closeMenus();
        void publishMoveHitSnapshot();
        bool isMoveRegion(int inX, int inY) const;
        void bindMoveHitTest();
        void unbindMoveHitTest();

        void initFileMenu();
        void initSettingsMenu();
        void syncMenuChecks();

    public:
        CH_FIELD()
        Chicane::String maximizeState;

        CH_FIELD()
        HeaderMenuItem::List menus;

        CH_FIELD()
        Chicane::String theme;

        CH_FIELD()
        Chicane::String viewportTabState;

        CH_FIELD()
        Chicane::String assetsTabState;

    private:
        void*                          m_moveWindow;
        mutable std::mutex             m_moveHitMutex;
        Chicane::Bounds2D              m_moveBounds;
        std::vector<Chicane::Bounds2D> m_moveControls;
    };
}
