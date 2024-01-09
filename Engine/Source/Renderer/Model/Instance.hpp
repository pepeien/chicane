#pragma once

#include "Base.hpp"

#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Model
    {
        struct AllocationInfo
        {
            uint32_t vertexCount;
            uint32_t firstVertex;
            uint32_t firstInstance;
            uint32_t indexCount;
            uint32_t firstIndex;
        };

        struct Instance
        {
            std::vector<Vertex::Instance> vertexInstances;
            std::vector<uint32_t> vertexIndices;
        };

        struct ParseResult
        {
            std::vector<Vertex::Instance> vertices;
            std::vector<uint32_t> indexes;
            std::unordered_map<std::string, uint32_t> indexesMap;
        };

        struct ParseBundle
        {
            std::vector<glm::vec3> geometryVertices;
            std::vector<glm::vec2> textureVertices;
            std::vector<glm::vec3> normalVertices;
        };
    }
}