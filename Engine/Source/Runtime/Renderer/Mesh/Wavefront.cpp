#include "Wavefront.hpp"

namespace Chicane
{
    namespace Mesh
    {
        namespace Wavefront
        {
            Property indetifyProperty(const std::string& inIdentifier)
            {
                if (inIdentifier.compare("v") == 0)
                {
                    return Property::GeometryVertices;
                }

                if (inIdentifier.compare("vt") == 0)
                {
                    return Property::TextureVertices;
                }

                if (inIdentifier.compare("vn") == 0)
                {
                    return Property::VertexNormals;
                }

                if (inIdentifier.compare("f") == 0)
                {
                    return Property::Faces;
                }

                return Property::Undefined;
            }

            void extractGeometryVertices(
                std::vector<glm::vec3>& outGeometryVertices,
                const std::vector<std::string>& inSplittedDataset
            )
            {
                glm::vec3 value;
                value.x = std::atof(inSplittedDataset[1].c_str());
                value.y = std::atof(inSplittedDataset[2].c_str());
                value.z = std::atof(inSplittedDataset[3].c_str());

                outGeometryVertices.push_back(value);
            }

            void extractTextureVertices(
                std::vector<glm::vec2>& outTextureVertices,
                const std::vector<std::string>& inSplittedDataset
            )
            {
                glm::vec2 value;
                value.x = std::atof(inSplittedDataset[1].c_str());
                value.y = std::atof(inSplittedDataset[2].c_str());

                outTextureVertices.push_back(value);
            }

            void extractNormalVertices(
                std::vector<glm::vec3>& outNormalVertices,
                const std::vector<std::string>& inSplittedDataset
            )
            {
                glm::vec3 value;
                value.x = std::atof(inSplittedDataset[1].c_str());
                value.y = std::atof(inSplittedDataset[2].c_str());
                value.z = std::atof(inSplittedDataset[3].c_str());

                outNormalVertices.push_back(value);
            }

            void combineVertices(
                std::vector<Vertex::Instance>& outVertices,
                std::vector<uint32_t>& outIndexes,
                std::unordered_map<std::string, uint32_t>& outIndexesMap,
                const std::vector<glm::vec3>& inGeometryVertices,
                const std::vector<glm::vec2>& inTextureVertices,
                const std::vector<glm::vec3>& inNormalVertices,
                const std::vector<std::string>& inSplittedDataset
            )
            {
                for (int i = 1; i < inSplittedDataset.size(); i++)
                {
                    std::string rawFace = inSplittedDataset[i];

                    if (outIndexesMap.find(rawFace) != outIndexesMap.end())
                    {
                        uint32_t index = outIndexesMap[rawFace];

                        outVertices.push_back(outVertices[index]);
                        outIndexes.push_back(index);

                        continue;
                    }

                    outIndexesMap.insert(std::make_pair(rawFace, outVertices.size()));

                    std::vector<std::string> faceData = Helper::splitString(
                        rawFace,
                        "/"
                    );

                    Vertex::Instance value;
                    value.color           = glm::vec3(1.0f, 0.0f, 0.0f);
                    value.position        = glm::vec3(1.0f);
                    value.texturePosition = glm::vec2(1.0f);
                    value.normal          = glm::vec3(1.0f);

                    if (faceData[0].compare("") != 0)
                    {
                        int index = std::atoi(faceData[0].c_str()) - 1;

                        value.position = inGeometryVertices[index];
                    }

                    if (faceData[1].compare("") != 0)
                    {
                        int index = std::atoi(faceData[1].c_str()) - 1;

                        value.texturePosition = inTextureVertices[index];
                    }

                    if (faceData[2].compare("") != 0)
                    {
                        int index = std::atoi(faceData[2].c_str()) - 1;

                        value.normal = inNormalVertices[index];
                    }

                    outIndexes.push_back(outVertices.size());
                    outVertices.push_back(value);
                }
            }

            ParseResult parse(const std::string& inFilepath)
            {    
                std::vector<std::string> rawData = Helper::splitString(
                    FileSystem::readFile(
                        ENGINE_MODELS_DIR + inFilepath
                    ),
                    "\n"
                );
                
                ParseResult result;

                std::vector<glm::vec3> geometricVertices;
                std::vector<glm::vec2> textureVertices;
                std::vector<glm::vec3> normalVertices;
                std::unordered_map<std::string, uint32_t> indexesMap;

                for (std::string& dataset : rawData)
                {
                    std::vector<std::string> splittedDataset = Helper::splitString(
                        dataset,
                        " "
                    );

                    switch (indetifyProperty(splittedDataset[0]))
                    {
                    case Property::GeometryVertices:
                        extractGeometryVertices(
                            geometricVertices,
                            splittedDataset
                        );

                        continue;
                    
                    case Property::TextureVertices:
                        extractTextureVertices(
                            textureVertices,
                            splittedDataset
                        );

                        continue;
                    
                    case Property::VertexNormals:
                        extractNormalVertices(
                            normalVertices,
                            splittedDataset
                        );

                        continue;

                    case Property::Faces:
                        combineVertices(
                            result.vertices,
                            result.indexes,
                            indexesMap,
                            geometricVertices,
                            textureVertices,
                            normalVertices,
                            splittedDataset
                        );

                        continue;

                    default:
                        continue;
                    }
                }

                return result;
            }
        }
    }
}