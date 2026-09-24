#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelGltf
        {
            struct CHICANE_BOX Primitive
            {
            public:
                String             name = String::empty();

                std::int32_t       material = -1;

                std::vector<float> positions          = {};
                std::int32_t       positionComponents = 0;

                bool               bHasNormals      = false;
                std::vector<float> normals          = {};
                std::int32_t       normalComponents = 0;

                bool               bHasUvs      = false;
                std::vector<float> uvs          = {};
                std::int32_t       uvComponents = 0;

                bool               bHasTangents      = false;
                std::vector<float> tangents          = {};
                std::int32_t       tangentComponents = 0;

                Vertex::Indices    indices = {};
            };
        }
    }
}
