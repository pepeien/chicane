#include "Chicane/Box/Model/Gltf.hpp"

#include "../Gltf/Parser.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelGltf
        {
            ModelParsed::Map parse(const ModelRaw& inData)
            {
                if (inData.empty())
                {
                    return {};
                }

                const Document   document(inData);
                const tg3_model& model = document.get();
                if (model.meshes_count == 0)
                {
                    return {};
                }

                const std::vector<std::int32_t> parents = document.parents();
                std::vector<glm::mat4>          locals(model.nodes_count, glm::mat4(1.0f));
                std::vector<glm::mat4>          worlds(model.nodes_count, glm::mat4(1.0f));
                std::vector<char>               computed(model.nodes_count, 0);

                for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                {
                    locals[i] = localGltfMatrix(model.nodes[i]);
                }

                bool progressed = true;
                while (progressed)
                {
                    progressed = false;

                    for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                    {
                        if (computed[i])
                        {
                            continue;
                        }

                        const std::int32_t parent = parents[i];
                        if (parent < 0)
                        {
                            worlds[i]   = locals[i];
                            computed[i] = 1;
                            progressed  = true;

                            continue;
                        }

                        if (!computed[static_cast<std::size_t>(parent)])
                        {
                            continue;
                        }

                        worlds[i]   = worlds[static_cast<std::size_t>(parent)] * locals[i];
                        computed[i] = 1;
                        progressed  = true;
                    }
                }

                std::unordered_map<std::int32_t, std::vector<std::int32_t>> meshNodes;
                for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                {
                    if (model.nodes[i].mesh < 0)
                    {
                        continue;
                    }

                    meshNodes[model.nodes[i].mesh].push_back(static_cast<std::int32_t>(i));
                }

                ModelParsed::Map           result;
                std::unordered_set<String> usedNames;

                auto parsePrimitive =
                    [&](const String& inName, const String& inBone, const tg3_primitive& inPrimitive, const glm::mat4& inWorld)
                {
                    const std::int32_t positionAccessor = document.findAttribute(inPrimitive, "POSITION");
                    if (positionAccessor < 0)
                    {
                        return;
                    }

                    std::vector<float> positions;
                    std::int32_t       positionComponents = 0;
                    if (!document.readFloats(positionAccessor, positions, positionComponents) ||
                        positionComponents < 3)
                    {
                        return;
                    }

                    const std::size_t vertexCount = positions.size() / static_cast<std::size_t>(positionComponents);
                    if (vertexCount == 0)
                    {
                        return;
                    }

                    std::vector<float> normals;
                    std::int32_t       normalComponents = 0;
                    const bool         hasNormals =
                        document.findAttribute(inPrimitive, "NORMAL") >= 0 &&
                        document.readFloats(document.findAttribute(inPrimitive, "NORMAL"), normals, normalComponents) &&
                        normalComponents >= 3 &&
                        (normals.size() / static_cast<std::size_t>(normalComponents)) >= vertexCount;

                    std::vector<float> uvs;
                    std::int32_t       uvComponents = 0;
                    const bool         hasUvs =
                        document.findAttribute(inPrimitive, "TEXCOORD_0") >= 0 &&
                        document.readFloats(document.findAttribute(inPrimitive, "TEXCOORD_0"), uvs, uvComponents) &&
                        uvComponents >= 2 &&
                        (uvs.size() / static_cast<std::size_t>(uvComponents)) >= vertexCount;

                    std::vector<std::uint32_t> sourceIndices;
                    if (inPrimitive.indices >= 0)
                    {
                        document.readIndices(inPrimitive.indices, sourceIndices);
                    }
                    else
                    {
                        sourceIndices.resize(vertexCount);
                        for (std::size_t i = 0; i < vertexCount; ++i)
                        {
                            sourceIndices[i] = static_cast<std::uint32_t>(i);
                        }
                    }

                    std::vector<std::uint32_t> indices;
                    appendTriangles(indices, sourceIndices, inPrimitive.mode);
                    if (indices.size() < 3)
                    {
                        return;
                    }

                    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(inWorld)));

                    ModelParsed parsed;
                    parsed.indices   = indices;
                    parsed.material  = inPrimitive.material;
                    parsed.bone      = inBone;
                    parsed.transform = transformFromMatrix(BASIS4 * inWorld * BASIS4_INVERSE);
                    parsed.vertices.reserve(vertexCount);

                    for (std::size_t i = 0; i < vertexCount; ++i)
                    {
                        const std::size_t offset = i * static_cast<std::size_t>(positionComponents);
                        const glm::vec4   mapped = inWorld * glm::vec4(
                                                       positions[offset],
                                                       positions[offset + 1],
                                                       positions[offset + 2],
                                                       1.0f
                                                   );

                        glm::vec3 normal(0.0f, 0.0f, 1.0f);
                        if (hasNormals)
                        {
                            const std::size_t normalOffset = i * static_cast<std::size_t>(normalComponents);
                            normal                         = glm::normalize(
                                normalMatrix *
                                glm::vec3(normals[normalOffset], normals[normalOffset + 1], normals[normalOffset + 2])
                            );
                        }

                        float u = 0.0f;
                        float v = 0.0f;
                        if (hasUvs)
                        {
                            const std::size_t uvOffset = i * static_cast<std::size_t>(uvComponents);
                            u                          = uvs[uvOffset];
                            v                          = uvs[uvOffset + 1];
                        }

                        Vertex vertex;
                        vertex.color    = Vec4(1.0f);
                        vertex.position = convertVector(mapped.x, mapped.y, mapped.z);
                        vertex.normal   = convertVector(normal.x, normal.y, normal.z);
                        vertex.uv       = Vec2(u, v);
                        parsed.vertices.push_back(vertex);
                    }

                    result.emplace(inName, std::move(parsed));
                };

                for (std::uint32_t meshIndex = 0; meshIndex < model.meshes_count; ++meshIndex)
                {
                    const tg3_mesh&                  mesh      = model.meshes[meshIndex];
                    const std::vector<std::int32_t>& nodes     = meshNodes[static_cast<std::int32_t>(meshIndex)];
                    const std::vector<std::int32_t>  instances = nodes.empty() ? std::vector<std::int32_t>{-1} : nodes;

                    for (const std::int32_t node : instances)
                    {
                        const glm::mat4 world = node >= 0 ? worlds[static_cast<std::size_t>(node)] : glm::mat4(1.0f);
                        for (std::uint32_t primitiveIndex = 0; primitiveIndex < mesh.primitives_count; ++primitiveIndex)
                        {
                            String name = toString(mesh.name);
                            if (name.isEmpty())
                            {
                                name = String::sprint("Mesh_%u", meshIndex);
                            }
                            if (mesh.primitives_count > 1)
                            {
                                name = String::sprint("%s_%u", name.toChar(), primitiveIndex);
                            }
                            if (instances.size() > 1 && node >= 0)
                            {
                                name = String::sprint("%s_%s", name.toChar(), document.nodeName(node).toChar());
                            }

                            const String bone = node >= 0 ? document.nodeName(node) : String();
                            parsePrimitive(
                                uniqueName(name, usedNames),
                                bone,
                                mesh.primitives[primitiveIndex],
                                world
                            );
                        }
                    }
                }

                return result;
            }
        }
    }
}
