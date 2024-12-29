#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Parse
        {
            struct Result
            {
                std::vector<Vertex::Instance>                  vertices;
                std::vector<std::uint32_t>                     indexes;
                std::unordered_map<std::string, std::uint32_t> indexesMap;
            };
        }
    }
}