#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Helper.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Vertex.hpp"

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
                std::vector<Vec<float>::Three>& outGeometryVertices,
                const std::vector<std::string>& inDataSet
            );
            void extractTextureVertices(
                std::vector<Vec<float>::Two>& outTextureVertices,
                const std::vector<std::string>& inDataSet
            );
            void extractNormalVertices(
                std::vector<Vec<float>::Three>& outNormalVertices,
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