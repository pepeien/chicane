#include "Editor/UI/Component/Explorer/TreeEntry.reflected.hpp"

namespace Editor
{
    ExplorerTreeEntry::ExplorerTreeEntry()
        : name(Chicane::String::empty()),
          path(Chicane::String::empty()),
          indent("0em"),
          expandState("leaf"),
          selectedState("idle")
    {}
}
