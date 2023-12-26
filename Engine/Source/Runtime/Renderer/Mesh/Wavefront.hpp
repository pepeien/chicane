#pragma once

#include "Base.hpp"
#include "Instance.hpp"
#include "Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Mesh
    {
        namespace Wavefront
        {
            enum class Property
            {
                // N/A
                Undefined,
                // v
                GeometryVertices,
                // vt
                TextureVertices,
                // vn
                VertexNormals,
                // f
                Faces
            };

            Property indetifyProperty(const std::string& inIdentifier);

            void extractGeometryVertices(
                std::vector<glm::vec3>& outGeometryVertices,
                const std::vector<std::string>& inSplittedDataset
            );
            void extractTextureVertices(
                std::vector<glm::vec2>& outTextureVertices,
                const std::vector<std::string>& inSplittedDataset
            );
            void extractNormalVertices(
                std::vector<glm::vec3>& outNormalVertices,
                const std::vector<std::string>& inSplittedDataset
            );
            void combineVertices(
                std::vector<Vertex::Instance>& outVertices,
                std::vector<uint32_t>& outIndexes,
                const std::vector<glm::vec3>& inGeometryVertices,
                const std::vector<glm::vec2>& inTextureVertices,
                const std::vector<glm::vec3>& inNormalVertices,
                const std::vector<std::string>& inSplittedDataset
            );

            ParseResult parse(const std::string& inFilepath);
        }
    }
}