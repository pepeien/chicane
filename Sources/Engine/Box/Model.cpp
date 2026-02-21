#include "Chicane/Box/Model.hpp"

#include <algorithm>
#include <unordered_map>

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
            if (EXTENSIONS.find(inValue) == EXTENSIONS.end())
            {
                return EXTENSIONS.at(ModelVendor::Undefined);
            }

            return EXTENSIONS.at(inValue);
        }

        Model::Model(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(ModelVendor::Undefined),
              m_data({})
        {
            fetchVendor();
            fetchData();
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

        const ModelRaw& Model::getData() const
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
            m_data = inData;

            if (!getXML().text().set(Base64::encode(m_data).toChar()))
            {
                throw std::runtime_error("Failed to save the model [" + m_header.filepath.string() + "] data");
            }
        }

        void Model::fetchVendor()
        {
            if (isEmpty())
            {
                return;
            }

            m_vendor = parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Model::fetchData()
        {
            if (isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}