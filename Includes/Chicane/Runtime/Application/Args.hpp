#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/String.hpp"
#include "Chicane/Runtime.hpp"

namespace Chicane
{
    using ApplicationArgs = std::unordered_map<String, std::vector<String>>;
}
