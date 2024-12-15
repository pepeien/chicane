#include "Chicane/Box/Assets/Model.hpp"

#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto VENDOR_ATTRIBUTE_NAME = "vendor";

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

        const std::vector<unsigned char>& Model::getData() const
        {
            return m_data;
        }

        void Model::fetchVendor()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                return;
            }

            std::uint8_t vendor = XML::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                m_xml.first_child()
            ).as_uint();

            m_vendor = (Chicane::Model::Vendor) vendor;
        }

        void Model::fetchData()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                return;
            }

            auto current = m_xml.first_child().child_value();

            if (!current)
            {
                return;
            }

            while (*current != '\0')
            {
                m_data.push_back(*current);

                current++;
            }
        }
    }
}