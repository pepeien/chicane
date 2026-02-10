#include "Chicane/Box/Model/Wavefront.hpp"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj_chicane.h>

namespace Chicane
{
    namespace Box
    {
        namespace ModelWavefront
        {
            void parseDataset(
                ModelParsed&                               outResult,
                std::unordered_map<String, std::uint32_t>& outMap,
                const fastObjMesh*                         inMesh,
                std::uint32_t                              inIndex
            )
            {
                const fastObjIndex& index = inMesh->indices[inIndex];

                String dataSet = "";
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

            ModelParsed::Map parse(const ModelRaw& inData)
            {
                String data = String(inData.begin(), inData.end());
                data.append('\n');

                fastObjMesh* mesh = fast_obj_read_memory(&data.front(), &data.back());

                if (!mesh)
                {
                    return {};
                }

                ModelParsed::Map result = {};

                std::vector<std::uint32_t> faceIndexOffsets(mesh->face_count);
                {
                    std::uint32_t offset = 0;
                    for (std::uint32_t f = 0; f < mesh->face_count; ++f)
                    {
                        faceIndexOffsets[f] = offset;
                        offset += mesh->face_vertices[f];
                    }
                }

                for (std::uint32_t o = 0; o < mesh->object_count; ++o)
                {
                    const fastObjGroup& obj = mesh->objects[o];

                    ModelParsed                               model = {};
                    std::unordered_map<String, std::uint32_t> map   = {};

                    for (std::uint32_t f = 0; f < obj.face_count; ++f)
                    {
                        std::uint32_t faceIndex  = obj.face_offset + f;
                        std::uint32_t faceVertex = mesh->face_vertices[faceIndex];
                        std::uint32_t indexStart = faceIndexOffsets[faceIndex];

                        for (std::uint32_t i = 1; i + 1 < faceVertex; ++i)
                        {
                            parseDataset(model, map, mesh, indexStart + 0);
                            parseDataset(model, map, mesh, indexStart + i);
                            parseDataset(model, map, mesh, indexStart + i + 1);
                        }
                    }

                    result.emplace(obj.name ? obj.name : "unnamed", std::move(model));
                }

                fast_obj_destroy(mesh);

                return result;
            }
        }
    }
}