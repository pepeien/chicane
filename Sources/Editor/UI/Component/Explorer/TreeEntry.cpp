#include "Editor/UI/Component/Explorer/TreeEntry.reflected.hpp"

namespace Editor
{
    ExplorerTreeEntry::ExplorerTreeEntry()
        : name(Chicane::String::sEmpty()),
          path(Chicane::String::sEmpty()),
          indent("0em"),
          expandState("leaf"),
          selectedState("idle")
    {}
}
