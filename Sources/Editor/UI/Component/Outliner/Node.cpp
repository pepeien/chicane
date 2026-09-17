#include "Editor/UI/Component/Outliner/Node.reflected.hpp"

namespace Editor
{
    OutlinerNode::OutlinerNode()
        : item(nullptr),
          label(Chicane::String::empty()),
          icon("Person"),
          indent("0em"),
          expandState("leaf"),
          selectedState("idle"),
          bHasChildren(false),
          bIsLeaf(true),
          bIsEditing(false),
          bShowLabel(true)
    {}
}
