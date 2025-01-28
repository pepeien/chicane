#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Model/Data.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Parse
            {
                struct Result
                {
                public:
                    std::vector<Vertex>                            vertices;
                    std::vector<std::uint32_t>                     indexes;
                    std::unordered_map<std::string, std::uint32_t> indexesMap;
                };
            }
        }
    }
}