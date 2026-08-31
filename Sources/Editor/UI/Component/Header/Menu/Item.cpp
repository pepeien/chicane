#include "Editor/UI/Component/Header/Menu/Item.reflected.hpp"

#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    HeaderMenuItem::HeaderMenuItem()
        : label(Chicane::String::empty()),
          shortcut(Chicane::String::empty()),
          action(Chicane::String::empty()),
          children({})
    {}
}
