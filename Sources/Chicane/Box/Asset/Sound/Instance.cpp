#include "Chicane/Box/Asset/Sound/Instance.hpp"

#include "Chicane/Box/Asset/Sound.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sound
        {
            static const std::unordered_map<std::string, Audio::Vendor> VENDOR_MAP {
                { "WAVE", Audio::Vendor::Wave }
            };

            Instance::Instance(const FileSystem::Path& inFilepath)
                : Asset::Instance(inFilepath),
                m_vendor(Audio::Vendor::Undefined)
            {
                fetchVendor();
                fetchData();
            }

            Audio::Vendor Instance::getVendor() const
            {
                return m_vendor;
            }

            void Instance::setVendor(Audio::Vendor inVendor)
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

            const Audio::Raw& Instance::getData() const
            {
                return m_data;
            }

            void Instance::setData(const FileSystem::Path& inFilepath)
            {
                if (!FileSystem::exists(inFilepath))
                {
                    return;
                }

                setData(FileSystem::readUnsigned(inFilepath));
            }

            void Instance::setData(const Audio::Raw& inData)
            {
                if (inData.empty())
                {
                    return;
                }

                m_data = inData;

                getXML().text().set(Base64::encode(inData));
            }

            void Instance::fetchVendor()
            {
                if (getFilepath().empty() || isEmpty())
                {
                    return;
                }

                std::string vendor = Xml::getAttribute(
                    VENDOR_ATTRIBUTE_NAME,
                    getXML()
                ).as_string();

                if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
                {
                    m_vendor = Audio::Vendor::Undefined;

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

                m_data = Base64::decodeToUnsigned(getXML().text().as_string());
            }
        }
    }
}