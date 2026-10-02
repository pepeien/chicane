#pragma once

#include <array>
#include <atomic>
#include <cstdint>
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
        Header(const Chicane::XmlNode& inNode);

        ~Header() override;

    public:
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
        struct MoveHitSnapshot
        {
        public:
            Chicane::Bounds2D              bounds;
            std::vector<Chicane::Bounds2D> controls;
            std::vector<Chicane::Bounds2D> overlays;
        };

        void*                          m_moveWindow = nullptr;
        std::array<MoveHitSnapshot, 2> m_moveHits;
        std::atomic<std::uint8_t>      m_moveHitIndex = 0;
    };
}
