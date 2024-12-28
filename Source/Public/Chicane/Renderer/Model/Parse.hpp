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
            struct Bundle
            {
                std::vector<Vec<3, float>> geometryVertices;
                std::vector<Vec<2, float>> textureVertices;
                std::vector<Vec<3, float>> normalVertices;
            };

            struct Result
            {
                std::vector<Vertex::Instance>             vertices;
                std::vector<uint32_t>                     indexes;
                std::unordered_map<std::string, uint32_t> indexesMap;
            };
        }
    }
}