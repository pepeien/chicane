#include "Chicane/Box/Model.hpp"

#include <algorithm>
#include <unordered_map>

#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Model/Wavefront.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<ModelVendor, String> EXTENSIONS = {
            {ModelVendor::Undefined, "N/A"},
            {ModelVendor::Wavefront, "OBJ"},
        };

        ModelVendor Model::parseVendor(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return ModelVendor::Undefined;
            }

            const String& value = inValue.trim().toUpper();

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

            default:
                throw std::runtime_error("Failed to parse Model due to invalid vendor");
            }

            return normalizeData(result);
        }

        ModelParsed::Map Model::normalizeData(const ModelParsed::Map& inValue) const
        {
            ModelParsed::Map result;

            for (const auto& [name, model] : inValue)
            {
                String id = getId();
                id.append("_");
                id.append(name);

                result[id] = model;
            }

            return result;
        }
    }
}