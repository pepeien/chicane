#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Toolbar))
    class Toolbar : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* ORIENTATION_ATTRIBUTE        = "orientation";
        static constexpr inline const char* TRANSLATE_STATE_ATTRIBUTE    = "translateState";
        static constexpr inline const char* ROTATE_STATE_ATTRIBUTE       = "rotateState";
        static constexpr inline const char* SCALE_STATE_ATTRIBUTE        = "scaleState";
        static constexpr inline const char* ON_TRACK_SAVE_ATTRIBUTE      = "onTrackSave";
        static constexpr inline const char* ON_GIZMO_TRANSLATE_ATTRIBUTE = "onGizmoTranslate";
        static constexpr inline const char* ON_GIZMO_ROTATE_ATTRIBUTE    = "onGizmoRotate";
        static constexpr inline const char* ON_GIZMO_SCALE_ATTRIBUTE     = "onGizmoScale";

    public:
        CH_CONSTRUCTOR()
        Toolbar(const Chicane::XmlNode& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onTrackSave();

        CH_FUNCTION()
        void onGizmoTranslate();

        CH_FUNCTION()
        void onGizmoRotate();

        CH_FUNCTION()
        void onGizmoScale();

        CH_FUNCTION()
        void onSettingsToggle();

        CH_FUNCTION()
        void closeSettings();

    public:
        CH_FIELD()
        Chicane::String orientation;

        CH_FIELD()
        Chicane::String translateState;

        CH_FIELD()
        Chicane::String rotateState;

        CH_FIELD()
        Chicane::String scaleState;

        CH_FIELD()
        bool isSettingsOpen;

        CH_FIELD()
        bool showSettingsHint;

        CH_FIELD()
        Chicane::String settingsState;
    };
}
