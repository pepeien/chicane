#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ViewComponent
        {
            const std::string TAG_ID = "View";
    
            void validate(const pugi::xml_node& inNode);
            void compile(
                pugi::xml_node& outNode,
                const Vec2& inResolution,
                const Vec2& inPosition
            );
        }
    }
}