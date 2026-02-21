#include "Chicane/Box/Font.hpp"

#include <algorithm>
#include <unordered_map>

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
            if (EXTENSIONS.find(inValue) == EXTENSIONS.end())
            {
                return EXTENSIONS.at(FontVendor::Undefined);
            }

            return EXTENSIONS.at(inValue);
        }

        Font::Font(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(FontVendor::Undefined),
              m_data({})
        {
            fetchVendor();
            fetchData();
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

            m_vendor = parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Font::fetchData()
        {
            if (isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}