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
            uint32_t vertexCount   = 0;
            uint32_t firstVertex   = 0;
            uint32_t firstInstance = 0;
            uint32_t indexCount    = 0;
            uint16_t instanceCount = 1;
            uint32_t firstIndex    = 0;
        };

        struct Bundle
        {
            std::vector<Mat<float>::Four> transforms;
            size_t allocationSize;
            void* writeLocation;
            Buffer::Instance buffer;
        };

        struct ParseBundle
        {
            std::vector<Vec<float>::Three> geometryVertices;
            std::vector<Vec<float>::Two> textureVertices;
            std::vector<Vec<float>::Three> normalVertices;
        };

        struct ParseResult
        {
            std::vector<Vertex::Instance> vertices;
            std::vector<uint32_t> indexes;
            std::unordered_map<std::string, uint32_t> indexesMap;
        };
        
        struct Instance
        {
            std::vector<Vertex::Instance> vertexInstances;
            std::vector<uint32_t> vertexIndices;
        };
    }
}