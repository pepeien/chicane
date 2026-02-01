#include "Chicane/Box/Font.hpp"

#include <algorithm>
#include <unordered_map>

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<String, FontVendor> VENDOR_MAP{
            {"TTF", FontVendor::TrueType}
        };

        Font::Font(const FileSystem::Path& inFilepath)
            : Asset(inFilepath)
        {
            fetchVendor();
            fetchData();
        }

        FontVendor Font::getVendor() const
        {
            return m_vendor;
        }

        void Font::setVendor(FontVendor inVendor)
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

        const FontRaw& Font::getData() const
        {
            return m_data;
        }

        void Font::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                return;
            }

            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Font::setData(const FontRaw& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXML().text().set(Base64::encode(inData));
        }

        void Font::fetchVendor()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            String vendor = Xml::getAttribute(VENDOR_ATTRIBUTE_NAME, getXML()).as_string();

            if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
            {
                m_vendor = FontVendor::Undefined;

                return;
            }

            m_vendor = VENDOR_MAP.at(vendor);
        }

        void Font::fetchData()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}