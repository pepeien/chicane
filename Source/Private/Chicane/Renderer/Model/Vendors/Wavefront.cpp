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
            void parse(
                const std::vector<unsigned char>& inData,
                ParseResult& outResult
            )
            {
                std::string data = std::string(inData.begin(), inData.end());
                data.push_back('\n');

                fastObjMesh* parsedData = fast_obj_read_data(
                    &data.front(),
                    &data.back()
                );

                std::uint32_t indexPerFace = *parsedData->face_vertices;

                std::vector<std::uint32_t> layout = { 0, 1, 2 };

                if (indexPerFace > 3)
                {
                    layout = { 0, 1, 2, 2, 3, 0 };
                }

                for (std::uint32_t i = 0; i < parsedData->index_count; i += indexPerFace)
                {
                    for (std::uint32_t index : layout)
                    {
                        fastObjIndex indices = parsedData->indices[i + index];

                        std::string dataSet = std::to_string(indices.p) + "/" + std::to_string(indices.t) + "/" + std::to_string(indices.n);

                        if (outResult.indexesMap.find(dataSet) != outResult.indexesMap.end())
                        {
                            uint32_t index = outResult.indexesMap.at(dataSet);

                            outResult.indexes.push_back(index);

                            continue;
                        }

                        Vertex::Instance vertex;
                        vertex.color = Vec<float>::Three(1.0f, 0.0f, 0.0f);

                        vertex.position   = Vec<float>::Three(0.0f);

                        vertex.position.x = parsedData->positions[indices.p * 3 + 0];
                        vertex.position.y = parsedData->positions[indices.p * 3 + 1];
                        vertex.position.z = parsedData->positions[indices.p * 3 + 2];

                        vertex.normal   = Vec<float>::Three(0.0f);
                        vertex.normal.x = parsedData->normals[indices.n * 3 + 0];
                        vertex.normal.y = parsedData->normals[indices.n * 3 + 1];
                        vertex.normal.z = parsedData->normals[indices.n * 3 + 2];

                        vertex.texturePosition   = Vec<float>::Two(0.0f);
                        vertex.texturePosition.x = parsedData->texcoords[indices.t * 2 + 0];
                        vertex.texturePosition.y = parsedData->texcoords[indices.t * 2 + 1];

                        uint32_t currentIndex = static_cast<uint32_t>(outResult.vertices.size());

                        outResult.indexesMap.insert(std::make_pair(dataSet, currentIndex));
                        outResult.indexes.push_back(currentIndex);
                        outResult.vertices.push_back(vertex);
                    }
                }

                fast_obj_destroy(parsedData);
            }
        }
    }
}