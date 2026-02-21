#include "Chicane/Box/Font.hpp"

#include <algorithm>
#include <unordered_map>

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<FontVendor, String> VENDORS = {
            {FontVendor::OpenType,  "OTF"},
            {FontVendor::TrueType,  "TTF"},
            {FontVendor::Undefined, "N/A"}
        };

        const String& Font::getVendorExtension(FontVendor inValue)
        {
            if (VENDORS.find(inValue) == VENDORS.end())
            {
                return VENDORS.at(FontVendor::Undefined);
            }

            return VENDORS.at(inValue);
        }

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

        void Font::setVendor(const String& inExtension)
        {
            if (inExtension.isEmpty())
            {
                setVendor(FontVendor::Undefined);

                return;
            }

            const String& value = inExtension.trim().toUpper();

            for (const auto& [type, extension] : VENDORS)
            {
                if (!value.contains(extension))
                {
                    continue;
                }

                setVendor(type);

                return;
            }

            setVendor(FontVendor::Undefined);
        }

        void Font::setVendor(FontVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, getVendorExtension(m_vendor));
        }

        const FontRaw& Font::getData() const
        {
            return m_data;
        }

        void Font::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Font source file was not found");
            }

            setVendor(inFilepath.extension().string());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Font::setData(const FontRaw& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            if (!getXML().text().set(Base64::encode(m_data).toChar()))
            {
                throw std::runtime_error("Failed to save the font [" + m_header.filepath.string() + "] data");
            }
        }

        void Font::fetchVendor()
        {
            if (isEmpty())
            {
                return;
            }

            setVendor(Xml::getAttribute(VENDOR_ATTRIBUTE_NAME, getXML()).as_string());
        }

        void Font::fetchData()
        {
            if (isEmpty())
            {
                return;
            }

            setData(Base64::decodeToUnsigned(getXML().text().as_string()));
        }
    }
}