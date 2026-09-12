#include "Chicane/Box/Model.hpp"

#include <algorithm>
#include <cmath>
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
            {ModelVendor::Undefined, "N/A" },
            {ModelVendor::Wavefront, "OBJ" },
            {ModelVendor::Gltf,      "GLTF"},
        };

        ModelVendor Model::parseVendor(const String& inValue)
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

        const String& Model::getVendorExtension(ModelVendor inValue)
        {
            const auto& found = EXTENSIONS.find(inValue);

            if (found == EXTENSIONS.end())
            {
                return EXTENSIONS.at(ModelVendor::Undefined);
            }

            return found->second;
        }

        const Model* Model::getDefault()
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
            setVendor(parseVendor(inValue));
        }

        void Model::setVendor(ModelVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, getVendorExtension(m_vendor));
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

                return ModelParsed::empty();
            }

            const Model* fallback = getDefault();

            if (!fallback || fallback == this)
            {
                return ModelParsed::empty();
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

            m_vendor = parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
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
                AssetPreview::createFromGeometry(getFilepath(), vertices, indices);
            if (!preview || !preview->image)
            {
                return;
            }

            AssetPreview::write(getXML(), AssetType::Model, *preview->image);
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
            std::vector<Vec3>& outTangents,
            std::vector<Vec3>& outBitangents,
            const Vertex::List& inVertices,
            Vertex::Index inFirst,
            Vertex::Index inSecond,
            Vertex::Index inThird
        )
        {
            if (
                inFirst >= inVertices.size() || inSecond >= inVertices.size() || inThird >= inVertices.size()
            )
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

            const float inverse = 1.0f / determinant;
            const Vec3  tangent   = (edge0 * uv1.y - edge1 * uv0.y) * inverse;
            const Vec3  bitangent = (edge1 * uv0.x - edge0 * uv1.x) * inverse;

            outTangents[inFirst] += tangent;
            outTangents[inSecond] += tangent;
            outTangents[inThird] += tangent;
            outBitangents[inFirst] += bitangent;
            outBitangents[inSecond] += bitangent;
            outBitangents[inThird] += bitangent;
        }

        static void generateTangents(ModelParsed& outModel)
        {
            if (outModel.vertices.empty() || hasVertexTangents(outModel.vertices))
            {
                return;
            }

            std::vector<Vec3> tangents(outModel.vertices.size(), Vec3::Zero());
            std::vector<Vec3> bitangents(outModel.vertices.size(), Vec3::Zero());

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

            for (std::size_t i = 0; i < outModel.vertices.size(); ++i)
            {
                const Vec3 normal = outModel.vertices[i].normal.normalize();
                Vec3       tangent = tangents[i];
                tangent            = tangent - normal * normal.dot(tangent);
                if (tangent.dot(tangent) < 1.0e-10f)
                {
                    const Vec3 axis = std::fabs(normal.z) < 0.9f ? Vec3::Up() : Vec3::Right();
                    tangent         = normal.cross(axis);
                }

                tangent = tangent.normalize();
                const float handedness =
                    normal.cross(tangent).dot(bitangents[i]) < 0.0f ? -1.0f : 1.0f;
                outModel.vertices[i].tangent = Vec4(tangent, handedness);
            }
        }

        ModelParsed::Map Model::normalizeData(const ModelParsed::Map& inValue) const
        {
            ModelParsed::Map result;

            for (const auto& [name, model] : inValue)
            {
                result[name] = model;
                generateTangents(result[name]);
            }

            return result;
        }
    }
}