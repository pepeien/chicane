#include "Chicane/Box/Asset/Model/Wavefront.hpp"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Wavefront
            {
                void parseDataset(
                    Extracted& outResult,
                    std::unordered_map<std::string, std::uint32_t>& outMap,
                    const fastObjMesh* inMesh,
                    std::uint32_t inIndex
                )
                {
                    const fastObjIndex& index = inMesh->indices[inIndex];

                    std::string dataSet = "";
                    dataSet += std::to_string(index.p) + "/";
                    dataSet += std::to_string(index.t) + "/";
                    dataSet += std::to_string(index.n);

                    if (outMap.find(dataSet) != outMap.end())
                    {
                        std::uint32_t index = outMap.at(dataSet);

                        outResult.indices.push_back(index);

                        return;
                    }

                    Vertex vertex = {};

                    vertex.color = Vec4(1.0f);

                    vertex.position.x = inMesh->positions[index.p * 3 + 0];
                    vertex.position.y = inMesh->positions[index.p * 3 + 1];
                    vertex.position.z = inMesh->positions[index.p * 3 + 2];

                    vertex.normal.x = inMesh->normals[index.n * 3 + 0];
                    vertex.normal.y = inMesh->normals[index.n * 3 + 1];
                    vertex.normal.z = inMesh->normals[index.n * 3 + 2];

                    vertex.uv.x = inMesh->texcoords[index.t * 2 + 0];
                    vertex.uv.y = inMesh->texcoords[index.t * 2 + 1];

                    std::uint32_t currentIndex = static_cast<std::uint32_t>(outResult.vertices.size());

                    outResult.indices.push_back(currentIndex);
                    outResult.vertices.push_back(vertex);

                    outMap.insert(std::make_pair(dataSet, currentIndex));
                }

                Extracted parse(const std::vector<unsigned char>& inData)
                {
                    Extracted result = {};

                    std::string data = std::string(inData.begin(), inData.end());
                    data.push_back('\n');

                    fastObjMesh* mesh = fast_obj_read_memory(&data.front(), &data.back());

                    std::uint32_t indexPerFace = *mesh->face_vertices;

                    std::vector<std::uint32_t> vertexLayout = { 0, 1, 2 };

                    if (indexPerFace > 3) // is quad
                    {
                        vertexLayout = { 0, 1, 2, 2, 3, 0 };
                    }

                    std::unordered_map<std::string, std::uint32_t> map = {};

                    for (std::uint32_t i = 0; i < mesh->index_count; i += indexPerFace)
                    {
                        for (std::uint32_t index : vertexLayout)
                        {
                            parseDataset(
                                result ,
                                map,
                                mesh,
                                i + index
                            );
                        }
                    }

                    fast_obj_destroy(mesh);

                    return result;
                }
            }
        }
    }
}