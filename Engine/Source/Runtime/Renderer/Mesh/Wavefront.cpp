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

            void extractTriangleVertex(
                ParseResult& outResult,
                const ParseBundle& outBundle,
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
                vertex.position        = glm::vec3(1.0f);
                vertex.texturePosition = glm::vec2(1.0f);
                vertex.normal          = glm::vec3(1.0f);

                std::vector<std::string> inSplittedDataset = Helper::splitString(
                    inDataSet,
                    "/"
                );

                if (inSplittedDataset[0].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[0].c_str()) - 1;

                    vertex.position = outBundle.geometryVertices[index];
                }

                if (inSplittedDataset[1].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[1].c_str()) - 1;

                    vertex.texturePosition = outBundle.textureVertices[index];
                }

                if (inSplittedDataset[2].compare("") != 0)
                {
                    int index = std::atoi(inSplittedDataset[2].c_str()) - 1;

                    vertex.normal = outBundle.normalVertices[index];
                }

                uint32_t vertexCount = outResult.vertices.size();
                
                outResult.indexesMap.insert(std::make_pair(inDataSet, vertexCount));
                outResult.indexes.push_back(vertexCount);
                outResult.vertices.push_back(vertex);
            }

            void extractTriangleVertices(
                ParseResult& outParseResult,
                const ParseBundle& inParseBundle,
                const std::vector<int>& inVertexLayout,
                const std::vector<std::string>& inDataSet
            )
            {
                int count  = inDataSet.size();
                int offset = 1;

                for (int vertexIndex : inVertexLayout)
                {
                    extractTriangleVertex(
                        outParseResult,
                        inParseBundle,
                        inDataSet[offset + vertexIndex]
                    );
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
                
                ParseResult parseResult;
                ParseBundle parseBundle;

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
                            parseBundle.geometryVertices,
                            splittedDataset
                        );

                        break;
                    
                    case Property::TextureVertices:
                        extractTextureVertices(
                            parseBundle.textureVertices,
                            splittedDataset
                        );

                        break;
                    
                    case Property::VertexNormals:
                        extractNormalVertices(
                            parseBundle.normalVertices,
                            splittedDataset
                        );

                        break;

                    case Property::Faces:
                        /**
                         * Quad detection:
                         * 
                         * Calculate face count minus 1 due to the identifier at the start
                         * 
                         * e.g. f 1,1,1 1,0,1 0,0,1
                        */
                        if ((splittedDataset.size() - 1) > 3)
                        {
                            extractTriangleVertices(
                                parseResult,
                                parseBundle,
                                { 0, 1, 2, 2, 3, 0 },
                                splittedDataset
                            );

                            break;
                        }

                        extractTriangleVertices(
                            parseResult,
                            parseBundle,
                            { 0, 1, 2 },
                            splittedDataset
                        );

                        break;

                    default:
                        break;
                    }
                }

                return parseResult;
            }
        }
    }
}