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
                const std::vector<glm::vec3>& inGeometryVertices,
                const std::vector<glm::vec2>& inTextureVertices,
                const std::vector<glm::vec3>& inNormalVertices,
                const std::vector<std::string>& inSplittedDataset
            )
            {
                for (int i = 1; i < inSplittedDataset.size(); i++)
                {
                    std::vector<std::string> rawFaces = Helper::splitString(
                        inSplittedDataset[i],
                        "/"
                    );

                    Vertex::Instance value;
                    value.color           = glm::vec3(1.0f, 0.0f, 0.0f);
                    value.position        = glm::vec3(1.0f);
                    value.texturePosition = glm::vec2(1.0f);
                    value.normal          = glm::vec3(1.0f);

                    if (rawFaces[0].compare("") != 0)
                    {
                        int index = std::atoi(rawFaces[0].c_str()) - 1;

                        value.position = inGeometryVertices[index];

                        outIndexes.push_back(index);
                    }

                    if (rawFaces[1].compare("") != 0)
                    {
                        int index = std::atoi(rawFaces[1].c_str()) - 1;

                        value.texturePosition = inTextureVertices[index];
                    }

                    if (rawFaces[2].compare("") != 0)
                    {
                        int index = std::atoi(rawFaces[2].c_str()) - 1;

                        value.normal = inNormalVertices[index];
                    }

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