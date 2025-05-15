#include "Chicane/Box/Asset/Texture/Instance.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            static constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

            static const std::unordered_map<std::string, Vendor> VENDOR_MAP {
                { "PNG", Vendor::Png }
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

                pugi::xml_node root = getXML();

                if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
                {
                    root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());

                    return;
                }

                root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());
            }

            const Image::RawData& Instance::getData() const
            {
                return m_data;
            }

            void Instance::setData(const Image::RawData& inData)
            {
                if (inData.empty())
                {
                    return;
                }

                m_data = inData;

                getXML().text().set(Base64::encode(inData));
            }

            void Instance::setData(const std::string& inFilepath)
            {
                if (!FileSystem::exists(inFilepath))
                {
                    return;
                }

                setData(Base64::readFile(inFilepath));
            }

            void Instance::fetchVendor()
            {
                if (getFilepath().empty() || isEmpty())
                {
                    return;
                }

                std::string vendor = XML::getAttribute(
                    VENDOR_ATTRIBUTE_NAME,
                    getXML()
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
                if (getFilepath().empty() || isEmpty())
                {
                    return;
                }

                m_data = Base64::decode(getXML().text().as_string());
            }
        }
    }
}