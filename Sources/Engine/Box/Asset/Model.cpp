#include "Chicane/Box/Asset/Model.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<String, ModelVendor> VENDOR_MAP {
            { "OBJ", ModelVendor::Wavefront }
        };

        Model::Model(const FileSystem::Path& inFilepath)
            : Asset(inFilepath)
        {
            fetchVendor();
            fetchData();
        }

        ModelVendor Model::getVendor() const
        {
            return m_vendor;
        }

        void Model::setVendor(ModelVendor inVendor)
        {
            m_vendor = inVendor;

            auto vendor = std::find_if(
                VENDOR_MAP.begin(),
                VENDOR_MAP.end(),
                [inVendor](const auto& inPair) { return inPair.second == inVendor; }
            );

            if (vendor == VENDOR_MAP.end())
            {
                return;
            }

            String vendorID = vendor->first;

            pugi::xml_node root = getXML();

            if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
            {
                root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.toChar());

                return;
            }

            root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.toChar());
        }

        const ModelRaw& Model::getData() const
        {
            return m_data;
        }

        void Model::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                return;
            }

            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Model::setData(const ModelRaw& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXML().text().set(Base64::encode(inData));
        }

        void Model::fetchVendor()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            String vendor = Xml::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                getXML()
            ).as_string();

            if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
            {
                m_vendor = ModelVendor::Undefined;

                return;
            }

            m_vendor = VENDOR_MAP.at(vendor);
        }

        void Model::fetchData()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}