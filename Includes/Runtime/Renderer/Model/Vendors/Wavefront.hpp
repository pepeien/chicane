#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Core/Helper.hpp"
#include "Runtime/Renderer/Model.hpp"
#include "Runtime/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Wavefront
        {
            enum class Property : uint8_t
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

            ParseResult parse(const std::vector<unsigned char>& inData);
        }
    }
}