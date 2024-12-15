#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace XML
    {
        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );
    }
}