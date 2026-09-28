#include "Editor/UI/Component/Outliner/Node.reflected.hpp"

namespace Editor
{
    OutlinerNode::OutlinerNode()
        : item(nullptr),
          label(Chicane::String::sEmpty()),
          icon("Person"),
          indent("0em"),
          expandState("leaf"),
          bHasChildren(false),
          bIsLeaf(true),
          bIsEditing(false),
          bShowLabel(true)
    {}
}
