#pragma once

#include "Core.hpp"
#include "Box/Asset/Model/Data.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Parse
            {
                struct CHICANE Result
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