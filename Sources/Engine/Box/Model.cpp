#include "Chicane/Box/Model.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Model/Gltf.hpp"
#include "Chicane/Box/Model/Wavefront.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<ModelVendor, String> EXTENSIONS = {
            {ModelVendor::Undefined, "N/A"   },
            {ModelVendor::Wavefront, "OBJ"   },
            {ModelVendor::Gltf,      "GLTF"  },
            {ModelVendor::Cooked,    "COOKED"},
        };

        ModelVendor Model::sParseVendor(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return ModelVendor::Undefined;
            }

            const String& value = inValue.trim().toUpper();
            if (value.contains("GLB"))
            {
                return ModelVendor::Gltf;
            }

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!value.contains(extension))
                {
                    continue;
                }

                return type;
            }

            return ModelVendor::Undefined;
        }

        const String& Model::sGetVendorExtension(ModelVendor inValue)
        {
            const auto& found = EXTENSIONS.find(inValue);

            if (found == EXTENSIONS.end())
            {
                return EXTENSIONS.at(ModelVendor::Undefined);
            }

            return found->second;
        }

        const Model* Model::sGetDefault()
        {
            return Box::load<Model>(DEFAULT_SOURCE);
        }

        Model::Model(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(ModelVendor::Undefined),
              m_data({})
        {
            fetchVendorFromXML();
            fetchDataFromXML();
        }

        ModelVendor Model::getVendor() const
        {
            return m_vendor;
        }

        void Model::setVendor(const String& inValue)
        {
            setVendor(sParseVendor(inValue));
        }

        void Model::setVendor(ModelVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, sGetVendorExtension(m_vendor));
        }

        const ModelParsed::Map& Model::getData() const
        {
            return m_data;
        }

        void Model::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Model source file was not found");
            }

            setVendor(inFilepath.extension().toString());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Model::setData(const ModelRaw& inData)
        {
            if (!setPayload(Base64::encode(inData)))
            {
                throw std::runtime_error("Failed to save the model [" + m_header.filepath.toString() + "] data");
            }

            m_data = parseData(inData);
            bakePreview();
        }

        static void appendU32(ModelRaw& outRaw, std::uint32_t inValue)
        {
            const unsigned char bytes[4] = {
                static_cast<unsigned char>(inValue),
                static_cast<unsigned char>(inValue >> 8),
                static_cast<unsigned char>(inValue >> 16),
                static_cast<unsigned char>(inValue >> 24),
            };
            outRaw.insert(outRaw.end(), bytes, bytes + 4);
        }

        static void appendF32(ModelRaw& outRaw, float inValue)
        {
            std::uint32_t bits = 0;
            std::memcpy(&bits, &inValue, sizeof(bits));
            appendU32(outRaw, bits);
        }

        static bool readU32(const ModelRaw& inRaw, std::size_t& ioCursor, std::uint32_t& outValue)
        {
            if (ioCursor + 4 > inRaw.size())
            {
                return false;
            }

            outValue = static_cast<std::uint32_t>(inRaw[ioCursor]) |
                       (static_cast<std::uint32_t>(inRaw[ioCursor + 1]) << 8) |
                       (static_cast<std::uint32_t>(inRaw[ioCursor + 2]) << 16) |
                       (static_cast<std::uint32_t>(inRaw[ioCursor + 3]) << 24);
            ioCursor += 4;

            return true;
        }

        static bool readF32(const ModelRaw& inRaw, std::size_t& ioCursor, float& outValue)
        {
            std::uint32_t bits = 0;
            if (!readU32(inRaw, ioCursor, bits))
            {
                return false;
            }

            std::memcpy(&outValue, &bits, sizeof(outValue));

            return true;
        }

        static ModelRaw encodeCooked(const ModelParsed::Map& inData)
        {
            ModelRaw raw;
            raw.push_back('B');
            raw.push_back('M');
            raw.push_back('D');
            raw.push_back('1');
            appendU32(raw, static_cast<std::uint32_t>(inData.size()));

            for (const auto& [name, model] : inData)
            {
                const std::string bytes = name.toStandard();
                appendU32(raw, static_cast<std::uint32_t>(bytes.size()));
                raw.insert(raw.end(), bytes.begin(), bytes.end());
                appendU32(raw, static_cast<std::uint32_t>(model.material));
                appendU32(raw, static_cast<std::uint32_t>(model.vertices.size()));

                for (const Vertex& vertex : model.vertices)
                {
                    appendF32(raw, vertex.position.x);
                    appendF32(raw, vertex.position.y);
                    appendF32(raw, vertex.position.z);
                    appendF32(raw, vertex.color.x);
                    appendF32(raw, vertex.color.y);
                    appendF32(raw, vertex.color.z);
                    appendF32(raw, vertex.color.w);
                    appendF32(raw, vertex.uv.x);
                    appendF32(raw, vertex.uv.y);
                    appendF32(raw, vertex.normal.x);
                    appendF32(raw, vertex.normal.y);
                    appendF32(raw, vertex.normal.z);
                    appendF32(raw, vertex.tangent.x);
                    appendF32(raw, vertex.tangent.y);
                    appendF32(raw, vertex.tangent.z);
                    appendF32(raw, vertex.tangent.w);
                }

                appendU32(raw, static_cast<std::uint32_t>(model.indices.size()));
                for (const Vertex::Index index : model.indices)
                {
                    appendU32(raw, index);
                }
            }

            return raw;
        }

        static ModelParsed::Map decodeCooked(const ModelRaw& inRaw)
        {
            if (inRaw.size() < 8 || inRaw[0] != 'B' || inRaw[1] != 'M' || inRaw[2] != 'D' || inRaw[3] != '1')
            {
                throw std::runtime_error("Cooked model data is invalid");
            }

            std::size_t   cursor = 4;
            std::uint32_t count  = 0;
            if (!readU32(inRaw, cursor, count))
            {
                throw std::runtime_error("Cooked model data is invalid");
            }

            ModelParsed::Map result;
            for (std::uint32_t group = 0; group < count; group++)
            {
                std::uint32_t nameSize = 0;
                if (!readU32(inRaw, cursor, nameSize) || cursor + nameSize > inRaw.size())
                {
                    throw std::runtime_error("Cooked model data is invalid");
                }

                const String name(std::string(reinterpret_cast<const char*>(inRaw.data() + cursor), nameSize));
                cursor += nameSize;

                std::uint32_t material    = 0;
                std::uint32_t vertexCount = 0;
                if (!readU32(inRaw, cursor, material) || !readU32(inRaw, cursor, vertexCount))
                {
                    throw std::runtime_error("Cooked model data is invalid");
                }

                ModelParsed parsed;
                parsed.material = static_cast<std::int32_t>(material);
                parsed.vertices.resize(vertexCount);
                for (Vertex& vertex : parsed.vertices)
                {
                    if (!readF32(inRaw, cursor, vertex.position.x) || !readF32(inRaw, cursor, vertex.position.y) ||
                        !readF32(inRaw, cursor, vertex.position.z) || !readF32(inRaw, cursor, vertex.color.x) ||
                        !readF32(inRaw, cursor, vertex.color.y) || !readF32(inRaw, cursor, vertex.color.z) ||
                        !readF32(inRaw, cursor, vertex.color.w) || !readF32(inRaw, cursor, vertex.uv.x) ||
                        !readF32(inRaw, cursor, vertex.uv.y) || !readF32(inRaw, cursor, vertex.normal.x) ||
                        !readF32(inRaw, cursor, vertex.normal.y) || !readF32(inRaw, cursor, vertex.normal.z) ||
                        !readF32(inRaw, cursor, vertex.tangent.x) || !readF32(inRaw, cursor, vertex.tangent.y) ||
                        !readF32(inRaw, cursor, vertex.tangent.z) || !readF32(inRaw, cursor, vertex.tangent.w))
                    {
                        throw std::runtime_error("Cooked model data is invalid");
                    }
                }

                std::uint32_t indexCount = 0;
                if (!readU32(inRaw, cursor, indexCount))
                {
                    throw std::runtime_error("Cooked model data is invalid");
                }

                parsed.indices.resize(indexCount);
                for (Vertex::Index& index : parsed.indices)
                {
                    std::uint32_t value = 0;
                    if (!readU32(inRaw, cursor, value))
                    {
                        throw std::runtime_error("Cooked model data is invalid");
                    }

                    index = value;
                }

                result.emplace(name, std::move(parsed));
            }

            return result;
        }

        void Model::setCooked(const ModelParsed::Map& inData)
        {
            setVendor(ModelVendor::Cooked);
            m_data = normalizeData(inData);

            if (!setPayload(Base64::encode(encodeCooked(m_data))))
            {
                throw std::runtime_error("Failed to save the model [" + m_header.filepath.toString() + "] data");
            }

            bakePreview();
        }

        const ModelParsed& Model::getModel(const String& inId) const
        {
            const auto& found = m_data.find(inId);

            if (found != m_data.end() && !found->second.vertices.empty())
            {
                return found->second;
            }

            if (getFilepath() == DEFAULT_SOURCE)
            {
                const auto& body = m_data.find(DEFAULT_REFERENCE);

                if (body != m_data.end() && !body->second.vertices.empty())
                {
                    return body->second;
                }

                if (!m_data.empty())
                {
                    return m_data.begin()->second;
                }

                return ModelParsed::sEmpty();
            }

            const Model* fallback = sGetDefault();

            if (!fallback || fallback == this)
            {
                return ModelParsed::sEmpty();
            }

            return fallback->getModel(DEFAULT_REFERENCE);
        }

        String Model::getUniqueId(const String& inId) const
        {
            if (getId().isEmpty())
            {
                return inId;
            }

            if (inId.isEmpty())
            {
                return getId();
            }

            String result = getId();
            result.append("_");
            result.append(inId);

            return result;
        }

        void Model::fetchVendorFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_vendor = sParseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME));
        }

        void Model::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_data = parseData(Base64::decodeToUnsigned(getPayload()));
        }

        void Model::bakePreview()
        {
            if (getFilepath().isEmpty() || m_data.empty())
            {
                return;
            }

            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
            for (const auto& entry : m_data)
            {
                const ModelParsed& data = entry.second;
                if (data.vertices.empty())
                {
                    continue;
                }

                const Vertex::Index base = static_cast<Vertex::Index>(vertices.size());
                vertices.insert(vertices.end(), data.vertices.begin(), data.vertices.end());

                if (data.indices.empty())
                {
                    for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(data.vertices.size()); i++)
                    {
                        indices.push_back(base + i);
                    }

                    continue;
                }

                for (const Vertex::Index index : data.indices)
                {
                    indices.push_back(base + index);
                }
            }

            const std::unique_ptr<AssetPreview> preview =
                AssetPreview::sCreateFromGeometry(getFilepath(), vertices, indices);
            if (!preview || !preview->image)
            {
                return;
            }

            AssetPreview::sWrite(getXML(), AssetType::Model, *preview->image);
        }

        ModelParsed::Map Model::parseData(const ModelRaw& inValue) const
        {
            ModelParsed::Map result;

            switch (m_vendor)
            {
            case ModelVendor::Wavefront:
                result = ModelWavefront::parse(inValue);

                break;

            case ModelVendor::Gltf:
                result = ModelGltf::parse(inValue);

                break;

            case ModelVendor::Cooked:
                result = decodeCooked(inValue);

                break;

            default:
                throw std::runtime_error("Failed to parse Model due to invalid vendor");
            }

            return normalizeData(result);
        }

        static bool hasVertexTangents(const Vertex::List& inVertices)
        {
            for (const Vertex& vertex : inVertices)
            {
                if (std::fabs(vertex.tangent.w) > 0.5f)
                {
                    return true;
                }
            }

            return false;
        }

        static void accumulateTriangleTangent(
            std::vector<Vec3>&  outTangents,
            std::vector<Vec3>&  outBitangents,
            const Vertex::List& inVertices,
            Vertex::Index       inFirst,
            Vertex::Index       inSecond,
            Vertex::Index       inThird
        )
        {
            if (inFirst >= inVertices.size() || inSecond >= inVertices.size() || inThird >= inVertices.size())
            {
                return;
            }

            const Vertex& first  = inVertices[inFirst];
            const Vertex& second = inVertices[inSecond];
            const Vertex& third  = inVertices[inThird];

            const Vec3 edge0 = second.position - first.position;
            const Vec3 edge1 = third.position - first.position;
            const Vec2 uv0   = second.uv - first.uv;
            const Vec2 uv1   = third.uv - first.uv;

            const float determinant = uv0.x * uv1.y - uv1.x * uv0.y;
            if (std::fabs(determinant) < 1.0e-8f)
            {
                return;
            }

            const float inverse   = 1.0f / determinant;
            const Vec3  tangent   = (edge0 * uv1.y - edge1 * uv0.y) * inverse;
            const Vec3  bitangent = (edge1 * uv0.x - edge0 * uv1.x) * inverse;

            outTangents[inFirst] += tangent;
            outTangents[inSecond] += tangent;
            outTangents[inThird] += tangent;
            outBitangents[inFirst] += bitangent;
            outBitangents[inSecond] += bitangent;
            outBitangents[inThird] += bitangent;
        }

        void Model::sGenerateTangents(ModelParsed& outModel)
        {
            if (outModel.vertices.empty() || hasVertexTangents(outModel.vertices))
            {
                return;
            }

            std::vector<Vec3> tangents(outModel.vertices.size(), Vec3::sZero());
            std::vector<Vec3> bitangents(outModel.vertices.size(), Vec3::sZero());

            if (outModel.indices.size() >= 3)
            {
                for (std::size_t i = 0; i + 2 < outModel.indices.size(); i += 3)
                {
                    accumulateTriangleTangent(
                        tangents,
                        bitangents,
                        outModel.vertices,
                        outModel.indices[i],
                        outModel.indices[i + 1],
                        outModel.indices[i + 2]
                    );
                }
            }
            else
            {
                for (Vertex::Index i = 0; i + 2 < static_cast<Vertex::Index>(outModel.vertices.size()); i += 3)
                {
                    accumulateTriangleTangent(tangents, bitangents, outModel.vertices, i, i + 1, i + 2);
                }
            }

            for (std::size_t i = 0; i < outModel.vertices.size(); i++)
            {
                const Vec3 normal  = outModel.vertices[i].normal.normalize();
                Vec3       tangent = tangents[i];
                tangent            = tangent - normal * normal.dot(tangent);
                if (tangent.dot(tangent) < 1.0e-10f)
                {
                    const Vec3 axis = std::fabs(normal.z) < 0.9f ? Vec3::sUp() : Vec3::sRight();
                    tangent         = normal.cross(axis);
                }

                tangent                      = tangent.normalize();
                const float handedness       = normal.cross(tangent).dot(bitangents[i]) < 0.0f ? -1.0f : 1.0f;
                outModel.vertices[i].tangent = Vec4(tangent, handedness);
            }
        }

        ModelParsed::Map Model::normalizeData(const ModelParsed::Map& inValue) const
        {
            ModelParsed::Map result;

            for (const auto& [name, model] : inValue)
            {
                result[name] = model;
                Model::sGenerateTangents(result[name]);
            }

            return result;
        }
    }
}