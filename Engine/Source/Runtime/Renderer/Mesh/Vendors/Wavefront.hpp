#pragma once

#include "Base.hpp"
#include "Renderer/Mesh/Instance.hpp"
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
                const std::vector<std::string>& inDataSet
            );
            void extractTextureVertices(
                std::vector<glm::vec2>& outTextureVertices,
                const std::vector<std::string>& inDataSet
            );
            void extractNormalVertices(
                std::vector<glm::vec3>& outNormalVertices,
                const std::vector<std::string>& inDataSet
            );
            void extractTriangleVertex(
                ParseResult& outResult,
                const ParseBundle& inBundle,
                const std::string& inDataSet
            );
            void extractTriangleVertices(
                ParseResult& outResult,
                const ParseBundle& inBundle,
                const std::vector<int>& inVertexLayout,
                const std::vector<std::string>& inDataSetList,
                uint32_t inOffset
            );

            ParseResult parse(const std::string& inFilepath);
        }
    }
}