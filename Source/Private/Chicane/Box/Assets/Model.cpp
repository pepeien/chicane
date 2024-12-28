#include "Chicane/Box/Assets/Model.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static constexpr auto VENDOR_ATTRIBUTE_NAME = "vendor";

        const std::unordered_map<std::string, Chicane::Model::Vendor> VENDOR_MAP {
            { "OBJ", Chicane::Model::Vendor::Wavefront }
        };

        Model::Model(const std::string& inFilepath)
            : Asset(inFilepath)
        {
            fetchVendor();
            fetchData();
        }

        Chicane::Model::Vendor Model::getVendor() const
        {
            return m_vendor;
        }

        void Model::setVendor(Chicane::Model::Vendor inVendor)
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

            std::string vendorID = vendor->first;

            pugi::xml_node root = getXMLRoot();

            if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
            {
                root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());

                return;
            }

            root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());
        }

        const std::vector<unsigned char>& Model::getData() const
        {
            return m_data;
        }

        void Model::setData(const std::vector<unsigned char>& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXMLRoot().text().set(base64::encode(inData));
        }

        void Model::setData(const std::string& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                return;
            }

            setData(base64::read_file(inFilepath));
        }

        void Model::fetchVendor()
        {
            if (getFilepath().empty() || isXMLEmpty())
            {
                return;
            }

            std::string vendor = XML::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                getXMLRoot()
            ).as_string();

            if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
            {
                m_vendor = Chicane::Model::Vendor::Undefined;

                return;
            }

            m_vendor = VENDOR_MAP.at(vendor);
        }

        void Model::fetchData()
        {
            if (getFilepath().empty() || isXMLEmpty())
            {
                return;
            }

            m_data = base64::decode(getXMLRoot().text().as_string());
        }
    }
}