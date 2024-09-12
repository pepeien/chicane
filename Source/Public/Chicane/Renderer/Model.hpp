#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        struct AllocationInfo
        {
            std::uint32_t vertexCount   = 0;
            std::uint32_t firstVertex   = 0;
            std::uint32_t firstInstance = 0;
            std::uint32_t indexCount    = 0;
            std::uint32_t firstIndex    = 0;
        };

        struct Bundle
        {
            std::vector<Mat<4, float>> transforms;
            size_t                     allocationSize;
            void*                      writeLocation;
            Buffer::Instance           buffer;
        };

        struct ParseBundle
        {
            std::vector<Vec<3, float>> geometryVertices;
            std::vector<Vec<2, float>> textureVertices;
            std::vector<Vec<3, float>> normalVertices;
        };

        struct ParseResult
        {
            std::vector<Vertex::Instance>             vertices;
            std::vector<uint32_t>                     indexes;
            std::unordered_map<std::string, uint32_t> indexesMap;
        };

        struct Instance
        {
            std::vector<Vertex::Instance> vertexInstances;
            std::vector<uint32_t>         vertexIndices;
            Vec<3, float>                 extent;
        };
    }
}