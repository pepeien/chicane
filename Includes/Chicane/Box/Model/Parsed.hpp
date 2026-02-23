#pragma once

#include <unordered_map>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ModelParsed
        {
        public:
            using Map = std::unordered_map<String, ModelParsed>;

        public:
            static const ModelParsed& empty()
            {
                static ModelParsed result;

                return result;
            }

        public:
            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
        };
    }
}