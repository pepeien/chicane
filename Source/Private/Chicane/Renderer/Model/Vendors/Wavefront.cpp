#include "Chicane/Renderer/Model/Vendors/Wavefront.hpp"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Wavefront
        {
            void parseDataset(
                ParseResult& outResult,
                fastObjMesh* inMesh,
                const fastObjIndex& inIndices
            )
            {
                std::string dataSet = "";
                dataSet += std::to_string(inIndices.p) + "/";
                dataSet += std::to_string(inIndices.t) + "/";
                dataSet += std::to_string(inIndices.n);

                if (outResult.indexesMap.find(dataSet) != outResult.indexesMap.end())
                {
                    uint32_t index = outResult.indexesMap.at(dataSet);

                    outResult.indexes.push_back(index);

                    return;
                }

                Vertex::Instance vertex;
                vertex.color = Vec<float>::Three(1.0f, 0.0f, 0.0f);

                vertex.position   = Vec<float>::Three(0.0f);

                vertex.position.x = inMesh->positions[inIndices.p * 3 + 0];
                vertex.position.y = inMesh->positions[inIndices.p * 3 + 1];
                vertex.position.z = inMesh->positions[inIndices.p * 3 + 2];

                vertex.normal   = Vec<float>::Three(0.0f);
                vertex.normal.x = inMesh->normals[inIndices.n * 3 + 0];
                vertex.normal.y = inMesh->normals[inIndices.n * 3 + 1];
                vertex.normal.z = inMesh->normals[inIndices.n * 3 + 2];

                vertex.texturePosition   = Vec<float>::Two(0.0f);
                vertex.texturePosition.x = inMesh->texcoords[inIndices.t * 2 + 0];
                vertex.texturePosition.y = inMesh->texcoords[inIndices.t * 2 + 1];

                uint32_t currentIndex = static_cast<uint32_t>(outResult.vertices.size());

                outResult.indexesMap.insert(std::make_pair(dataSet, currentIndex));
                outResult.indexes.push_back(currentIndex);
                outResult.vertices.push_back(vertex);
            }

            void parse(
                ParseResult& outResult,
                const std::vector<unsigned char>& inData
            )
            {
                std::string data = std::string(inData.begin(), inData.end());
                data.push_back('\n');

                fastObjMesh* mesh = fast_obj_read_data(
                    &data.front(),
                    &data.back()
                );

                std::uint32_t indexPerFace = *mesh->face_vertices;

                std::vector<std::uint32_t> vertexLayout = { 0, 1, 2 };

                if (indexPerFace > 3) // is quad
                {
                    vertexLayout = { 0, 1, 2, 2, 3, 0 };
                }

                for (std::uint32_t i = 0; i < mesh->index_count; i += indexPerFace)
                {
                    for (std::uint32_t index : vertexLayout)
                    {
                        parseDataset(
                            outResult,
                            mesh,
                            mesh->indices[i + index]
                        );
                    }
                }

                fast_obj_destroy(mesh);
            }
        }
    }
}