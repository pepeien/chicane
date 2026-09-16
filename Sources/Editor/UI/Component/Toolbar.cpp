#include "Editor/UI/Component/Toolbar.reflected.hpp"

#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Toolbar::Toolbar(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          orientation(Chicane::String::empty()),
          translateState(Chicane::String::empty()),
          rotateState(Chicane::String::empty()),
          scaleState(Chicane::String::empty())
    {
        load("Assets/Editor/UI/Components/Toolbar.grid", "Assets/Editor/UI/Components/Toolbar.decal");

        Prop::bind(this, ORIENTATION_ATTRIBUTE, orientation);
        Prop::bind(this, TRANSLATE_STATE_ATTRIBUTE, translateState);
        Prop::bind(this, ROTATE_STATE_ATTRIBUTE, rotateState);
        Prop::bind(this, SCALE_STATE_ATTRIBUTE, scaleState);
    }

    void Toolbar::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, ORIENTATION_ATTRIBUTE, orientation);
        Prop::copy(this, TRANSLATE_STATE_ATTRIBUTE, translateState);
        Prop::copy(this, ROTATE_STATE_ATTRIBUTE, rotateState);
        Prop::copy(this, SCALE_STATE_ATTRIBUTE, scaleState);
    }

    void Toolbar::onTrackSave()
    {
        Prop::invoke(this, ON_TRACK_SAVE_ATTRIBUTE);
    }

    void Toolbar::onGizmoTranslate()
    {
        Prop::invoke(this, ON_GIZMO_TRANSLATE_ATTRIBUTE);
    }

    void Toolbar::onGizmoRotate()
    {
        Prop::invoke(this, ON_GIZMO_ROTATE_ATTRIBUTE);
    }

    void Toolbar::onGizmoScale()
    {
        Prop::invoke(this, ON_GIZMO_SCALE_ATTRIBUTE);
    }
}
