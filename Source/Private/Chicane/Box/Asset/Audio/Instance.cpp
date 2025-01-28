#include "Chicane/Box/Asset/Audio/Instance.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            static constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

            static const std::unordered_map<std::string, Vendor> VENDOR_MAP {
                { "WAVE", Vendor::Wave }
            };

            Instance::Instance(const std::string& inFilepath)
                : Asset::Instance(inFilepath),
                m_vendor(Vendor::Undefined)
            {
                fetchVendor();
                fetchData();
            }

            Vendor Instance::getVendor() const
            {
                return m_vendor;
            }

            void Instance::setVendor(Vendor inVendor)
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

            const RawData& Instance::getData() const
            {
                return m_data;
            }

            void Instance::setData(const RawData& inData)
            {
                if (inData.empty())
                {
                    return;
                }

                m_data = inData;

                getXMLRoot().text().set(base64::encode(inData));
            }

            void Instance::setData(const std::string& inFilepath)
            {
                if (!FileSystem::exists(inFilepath))
                {
                    return;
                }

                setData(base64::read_file(inFilepath));
            }

            void Instance::fetchVendor()
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
                    m_vendor = Vendor::Undefined;

                    return;
                }

                m_vendor = VENDOR_MAP.at(vendor);
            }

            void Instance::fetchData()
            {
                if (getFilepath().empty() || isXMLEmpty())
                {
                    return;
                }

                m_data = base64::decode(getXMLRoot().text().as_string());
            }
        }
    }
}