#include "Chicane/Box/Model.hpp"

#include <algorithm>
#include <unordered_map>

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

            setVendor(inFilepath.extension().string());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Model::setData(const ModelRaw& inData)
        {
            if (!getXML().text().set(Base64::encode(inData).toChar()))
            {
                throw std::runtime_error("Failed to save the model [" + m_header.filepath.string() + "] data");
            }

            m_data = parseData(inData);
        }

        const ModelParsed& Model::getModel(const String& inId) const
        {
            const auto& found = m_data.find(inId);

            if (found == m_data.end())
            {
                return ModelParsed::empty();
            }

            return found->second;
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

            m_data = parseData(Base64::decodeToUnsigned(getXML().text().as_string()));
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