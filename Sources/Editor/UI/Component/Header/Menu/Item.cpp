#include "Editor/UI/Component/Header/Menu/Item.reflected.hpp"

#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    HeaderMenuItem::HeaderMenuItem()
        : label(Chicane::String::sEmpty()),
          shortcut(Chicane::String::sEmpty()),
          action(Chicane::String::sEmpty()),
          isChecked(false),
          children({})
    {}
}
