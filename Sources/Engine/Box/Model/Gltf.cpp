#include "Chicane/Box/Model/Gltf.hpp"

#include "Chicane/Box/Model/Gltf/Primitive.hpp"

#include "../Gltf/Parser.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Worker/Pool.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelGltf
        {
            static bool readPrimitive(
                const Document& inDocument, const tg3_primitive& inPrimitive, Primitive& outPrimitive
            )
            {
                const std::int32_t positionAccessor = inDocument.findAttribute(inPrimitive, "POSITION");
                if (positionAccessor < 0)
                {
                    return false;
                }

                if (!inDocument.readFloats(positionAccessor, outPrimitive.positions, outPrimitive.positionComponents) ||
                    outPrimitive.positionComponents < 3)
                {
                    return false;
                }

                const std::size_t vertexCount =
                    outPrimitive.positions.size() / static_cast<std::size_t>(outPrimitive.positionComponents);
                if (vertexCount == 0)
                {
                    return false;
                }

                outPrimitive.bHasNormals = inDocument.findAttribute(inPrimitive, "NORMAL") >= 0 &&
                                           inDocument.readFloats(
                                               inDocument.findAttribute(inPrimitive, "NORMAL"),
                                               outPrimitive.normals,
                                               outPrimitive.normalComponents
                                           ) &&
                                           outPrimitive.normalComponents >= 3 &&
                                           (outPrimitive.normals.size() /
                                            static_cast<std::size_t>(outPrimitive.normalComponents)) >= vertexCount;

                outPrimitive.bHasUvs =
                    inDocument.findAttribute(inPrimitive, "TEXCOORD_0") >= 0 &&
                    inDocument.readFloats(
                        inDocument.findAttribute(inPrimitive, "TEXCOORD_0"),
                        outPrimitive.uvs,
                        outPrimitive.uvComponents
                    ) &&
                    outPrimitive.uvComponents >= 2 &&
                    (outPrimitive.uvs.size() / static_cast<std::size_t>(outPrimitive.uvComponents)) >= vertexCount;

                outPrimitive.bHasTangents = inDocument.findAttribute(inPrimitive, "TANGENT") >= 0 &&
                                            inDocument.readFloats(
                                                inDocument.findAttribute(inPrimitive, "TANGENT"),
                                                outPrimitive.tangents,
                                                outPrimitive.tangentComponents
                                            ) &&
                                            outPrimitive.tangentComponents >= 4 &&
                                            (outPrimitive.tangents.size() /
                                             static_cast<std::size_t>(outPrimitive.tangentComponents)) >= vertexCount;

                std::vector<std::uint32_t> sourceIndices;
                if (inPrimitive.indices >= 0)
                {
                    inDocument.readIndices(inPrimitive.indices, sourceIndices);
                }
                else
                {
                    sourceIndices.resize(vertexCount);
                    for (std::size_t index = 0; index < vertexCount; index++)
                    {
                        sourceIndices[index] = static_cast<std::uint32_t>(index);
                    }
                }

                std::vector<std::uint32_t> indices;
                appendTriangles(indices, sourceIndices, inPrimitive.mode);
                if (indices.size() < 3)
                {
                    return false;
                }

                outPrimitive.indices  = indices;
                outPrimitive.material = inPrimitive.material;

                return true;
            }

            static void fillPrimitive(const Primitive& inPrimitive, ModelParsed& outParsed)
            {
                const std::size_t vertexCount =
                    inPrimitive.positions.size() / static_cast<std::size_t>(inPrimitive.positionComponents);

                outParsed.indices  = inPrimitive.indices;
                outParsed.material = inPrimitive.material;
                outParsed.vertices.clear();
                outParsed.vertices.reserve(vertexCount);

                for (std::size_t index = 0; index < vertexCount; index++)
                {
                    const std::size_t offset = index * static_cast<std::size_t>(inPrimitive.positionComponents);

                    glm::vec3 normal(0.0f, 0.0f, 1.0f);
                    if (inPrimitive.bHasNormals)
                    {
                        const std::size_t normalOffset = index * static_cast<std::size_t>(inPrimitive.normalComponents);
                        normal                         = glm::normalize(
                            glm::vec3(
                                inPrimitive.normals[normalOffset],
                                inPrimitive.normals[normalOffset + 1],
                                inPrimitive.normals[normalOffset + 2]
                            )
                        );
                    }

                    float u = 0.0f;
                    float v = 0.0f;
                    if (inPrimitive.bHasUvs)
                    {
                        const std::size_t uvOffset = index * static_cast<std::size_t>(inPrimitive.uvComponents);
                        u                          = inPrimitive.uvs[uvOffset];
                        v                          = inPrimitive.uvs[uvOffset + 1];
                    }

                    Vertex vertex;
                    vertex.color    = Vec4(1.0f);
                    vertex.position = convertVector(
                        inPrimitive.positions[offset],
                        inPrimitive.positions[offset + 1],
                        inPrimitive.positions[offset + 2]
                    );
                    vertex.normal = convertVector(normal.x, normal.y, normal.z);
                    vertex.uv     = Vec2(u, v);
                    if (inPrimitive.bHasTangents)
                    {
                        const std::size_t tangentOffset =
                            index * static_cast<std::size_t>(inPrimitive.tangentComponents);
                        const Vec3 converted = convertVector(
                            inPrimitive.tangents[tangentOffset],
                            inPrimitive.tangents[tangentOffset + 1],
                            inPrimitive.tangents[tangentOffset + 2]
                        );
                        vertex.tangent = Vec4(converted, inPrimitive.tangents[tangentOffset + 3]);
                    }

                    outParsed.vertices.push_back(vertex);
                }
            }

            Scene parseScene(const ModelRaw& inData)
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

                for (std::uint32_t index = 0; index < model.nodes_count; index++)
                {
                    locals[index] = localGltfMatrix(model.nodes[index]);
                }

                bool bProgressed = true;
                while (bProgressed)
                {
                    bProgressed = false;

                    for (std::uint32_t index = 0; index < model.nodes_count; index++)
                    {
                        if (computed[index])
                        {
                            continue;
                        }

                        const std::int32_t parent = parents[index];
                        if (parent < 0)
                        {
                            worlds[index]   = locals[index];
                            computed[index] = 1;
                            bProgressed     = true;

                            continue;
                        }

                        if (!computed[static_cast<std::size_t>(parent)])
                        {
                            continue;
                        }

                        worlds[index]   = worlds[static_cast<std::size_t>(parent)] * locals[index];
                        computed[index] = 1;
                        bProgressed     = true;
                    }
                }

                std::unordered_map<std::int32_t, std::vector<std::int32_t>> meshNodes;
                for (std::uint32_t index = 0; index < model.nodes_count; index++)
                {
                    if (model.nodes[index].mesh < 0)
                    {
                        continue;
                    }

                    meshNodes[model.nodes[index].mesh].push_back(static_cast<std::int32_t>(index));
                }

                std::vector<Primitive>     primitives;
                std::unordered_set<String> usedNames;
                Scene                      scene;

                for (std::uint32_t meshIndex = 0; meshIndex < model.meshes_count; meshIndex++)
                {
                    const tg3_mesh&                  mesh  = model.meshes[meshIndex];
                    const std::vector<std::int32_t>& nodes = meshNodes[static_cast<std::int32_t>(meshIndex)];
                    std::vector<String>              primitiveNames;

                    for (std::uint32_t primitiveIndex = 0; primitiveIndex < mesh.primitives_count; primitiveIndex++)
                    {
                        Primitive primitive;
                        if (!readPrimitive(document, mesh.primitives[primitiveIndex], primitive))
                        {
                            continue;
                        }

                        String name = toString(mesh.name);
                        if (name.isEmpty())
                        {
                            name = String::sprint("Mesh_%u", meshIndex);
                        }
                        if (mesh.primitives_count > 1)
                        {
                            name = String::sprint("%s_%u", name.toChar(), primitiveIndex);
                        }

                        primitive.name = uniqueName(name, usedNames);
                        primitiveNames.push_back(primitive.name);
                        primitives.push_back(std::move(primitive));
                    }

                    if (primitiveNames.empty())
                    {
                        continue;
                    }

                    const std::vector<std::int32_t> instances = nodes.empty() ? std::vector<std::int32_t>{-1} : nodes;
                    for (const std::int32_t node : instances)
                    {
                        const glm::mat4 world = node >= 0 ? worlds[static_cast<std::size_t>(node)] : glm::mat4(1.0f);
                        const Transform transform = transformFromMatrix(BASIS4 * world * BASIS4_INVERSE);
                        const String    bone      = node >= 0 ? document.nodeName(node) : String();

                        for (const String& name : primitiveNames)
                        {
                            Instance instance;
                            instance.id        = name;
                            instance.bone      = bone;
                            instance.transform = transform;
                            scene.instances.push_back(std::move(instance));
                        }
                    }
                }

                std::vector<ModelParsed> parsed(primitives.size());
                WorkerPool::parallel(
                    primitives.size(),
                    [&primitives, &parsed](std::size_t inIndex) { fillPrimitive(primitives[inIndex], parsed[inIndex]); }
                );

                for (std::size_t index = 0; index < primitives.size(); index++)
                {
                    scene.geometry.emplace(primitives[index].name, std::move(parsed[index]));
                }

                return scene;
            }

            ModelParsed::Map parse(const ModelRaw& inData)
            {
                return parseScene(inData).geometry;
            }
        }
    }
}
