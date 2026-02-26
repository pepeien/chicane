#include "Chicane/Box/Font.hpp"

#include <algorithm>
#include <unordered_map>

#include "Chicane/Box/Font/TrueType.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<FontVendor, String> EXTENSIONS = {
            {FontVendor::Undefined, "N/A"},
            {FontVendor::OpenType,  "OTF"},
            {FontVendor::TrueType,  "TTF"},
        };

        FontVendor Font::parseVendor(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return FontVendor::Undefined;
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

            return FontVendor::Undefined;
        }

        const String& Font::getVendorExtension(FontVendor inValue)
        {
            const auto& found = EXTENSIONS.find(inValue);

            if (found == EXTENSIONS.end())
            {
                return EXTENSIONS.at(FontVendor::Undefined);
            }

            return found->second;
        }

        Font::Font(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(FontVendor::Undefined),
              m_data({})
        {
            fetchVendorFromXML();
            fetchDataFromXML();
        }

        FontVendor Font::getVendor() const
        {
            return m_vendor;
        }

        void Font::setVendor(const String& inValue)
        {
            setVendor(parseVendor(inValue));
        }

        void Font::setVendor(FontVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, getVendorExtension(m_vendor));
        }

        const FontFamily& Font::getData() const
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
            if (!getXML().text().set(Base64::encode(inData).toChar()))
            {
                throw std::runtime_error("Failed to save the font [" + m_header.filepath.string() + "] data");
            }

            m_data = parseData(inData);
        }

        void Font::fetchVendorFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_vendor = parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Font::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_data = parseData(Base64::decodeToUnsigned(getXML().text().as_string()));
        }

        FontFamily Font::parseData(const FontRaw& inValue) const
        {
            switch (m_vendor)
            {
            case FontVendor::OpenType:
            case FontVendor::TrueType:
                return FontTrueType::parse(getId(), inValue);

            default:
                throw std::runtime_error("Failed to parse Font due to invalid vendor");
            }
        }
    }
}