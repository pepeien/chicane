#include "Wavefront.hpp"

namespace Chicane
{
    namespace Model
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
                const std::vector<std::string>& inDataSet
            )
            {
                glm::vec3 value;
                value.x = std::stof(inDataSet[1].c_str());
                value.y = std::stof(inDataSet[2].c_str());
                value.z = std::stof(inDataSet[3].c_str());

                outGeometryVertices.push_back(value);
            }

            void extractTextureVertices(
                std::vector<glm::vec2>& outTextureVertices,
                const std::vector<std::string>& inDataSet
            )
            {
                glm::vec2 value;
                value.x = std::stof(inDataSet[1].c_str());
                value.y = std::stof(inDataSet[2].c_str());

                outTextureVertices.push_back(value);
            }

            void extractNormalVertices(
                std::vector<glm::vec3>& outNormalVertices,
                const std::vector<std::string>& inDataSet
            )
            {
                glm::vec3 value;
                value.x = std::stof(inDataSet[1].c_str());
                value.y = std::stof(inDataSet[2].c_str());
                value.z = std::stof(inDataSet[3].c_str());

                outNormalVertices.push_back(value);
            }

            void extractTriangleVertex(
                ParseResult& outResult,
                const ParseBundle& inBundle,
                const std::string& inDataSet
            )
            {
                if (outResult.indexesMap.find(inDataSet) != outResult.indexesMap.end())
                {
                    uint32_t index = outResult.indexesMap[inDataSet];

                    outResult.indexes.push_back(index);

                    return;
                }

                Vertex::Instance vertex;
                vertex.color           = glm::vec3(1.0f, 0.0f, 0.0f);
                vertex.position        = glm::vec3(0.0f);
                vertex.texturePosition = glm::vec2(0.0f);
                vertex.normal          = glm::vec3(0.0f);

                std::vector<std::string> inSplittedDataset = Helper::splitString(
                    inDataSet,
                    "/"
                );

                if (inSplittedDataset[0].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[0].c_str()) - 1;

                    vertex.position = inBundle.geometryVertices[index];
                }

                if (inSplittedDataset[1].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[1].c_str()) - 1;

                    vertex.texturePosition = inBundle.textureVertices[index];
                }

                if (inSplittedDataset[2].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[2].c_str()) - 1;

                    vertex.normal = inBundle.normalVertices[index];
                }

                uint32_t currentIndex = outResult.vertices.size();
                
                outResult.indexesMap.insert(std::make_pair(inDataSet, currentIndex));
                outResult.indexes.push_back(currentIndex);
                outResult.vertices.push_back(vertex);
            }

            void extractTriangleVertices(
                ParseResult& outResult,
                const ParseBundle& inBundle,
                const std::vector<int>& inVertexLayout,
                const std::vector<std::string>& inDataSetList,
                uint32_t inOffset = 1
            )
            {
                for (int vertexIndex : inVertexLayout)
                {
                    extractTriangleVertex(
                        outResult,
                        inBundle,
                        inDataSetList[inOffset + vertexIndex]
                    );
                }
            }

            ParseResult parse(const std::vector<unsigned char>& inData)
            {    
                std::vector<std::string> dataSets = Helper::splitString(
                    std::string(inData.begin(), inData.end()),
                    "\n"
                );

                ParseResult result;
                ParseBundle bundle;

                for (const std::string& dataSet : dataSets)
                {
                    std::vector<std::string> splittedDataSet = Helper::splitString(
                        dataSet,
                        " "
                    );

                    switch (indetifyProperty(splittedDataSet[0]))
                    {
                    case Property::GeometryVertices:
                        extractGeometryVertices(
                            bundle.geometryVertices,
                            splittedDataSet
                        );

                        break;

                    case Property::TextureVertices:
                        extractTextureVertices(
                            bundle.textureVertices,
                            splittedDataSet
                        );

                        break;

                    case Property::VertexNormals:
                        extractNormalVertices(
                            bundle.normalVertices,
                            splittedDataSet
                        );

                        break;

                    case Property::Faces:
                        // Quad detection
                        if ((splittedDataSet.size() - 1) > 3)
                        {
                            extractTriangleVertices(
                                result,
                                bundle,
                                { 0, 1, 2, 2, 3, 0 },
                                splittedDataSet
                            );

                            break;
                        }

                        extractTriangleVertices(
                            result,
                            bundle,
                            { 0, 1, 2 },
                            splittedDataSet
                        );

                        break;

                    default:
                        break;
                    }
                }

                return result;
            }
        }
    }
}