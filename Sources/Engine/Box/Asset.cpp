#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        Asset::Asset(const FileSystem::Path& inSource)
        {
            if (inSource.isEmpty())
            {
                return;
            }

            if (!FileSystem::exists(inSource))
            {
                createXML(inSource);

                return;
            }

            fetchXML(inSource);

            m_header = AssetHeader(inSource);
        }

        bool Asset::isType(AssetType inType) const
        {
            return m_header.type == inType;
        }

        bool Asset::isXMLEmpty() const
        {
            const XmlNode root = getXML();
            if (Xml::isEmpty(root))
            {
                return true;
            }

            return root.getFirstChild() == root.getLastChild() && Xml::isEmpty(root.getFirstChild());
        }

        const AssetHeader& Asset::getHeader() const
        {
            return m_header;
        }

        void Asset::setHeader(const AssetHeader& inHeader)
        {
            setFilepath(inHeader.filepath);
            setVersion(inHeader.version);
            setId(inHeader.id);
            setType(inHeader.type);
        }

        const FileSystem::Path& Asset::getFilepath() const
        {
            return m_header.filepath;
        }

        void Asset::setFilepath(const FileSystem::Path& inFilepath)
        {
            m_header.filepath = inFilepath;
            Serializable::setFilepath(inFilepath);
        }

        std::uint32_t Asset::getVersion() const
        {
            return m_header.version;
        }

        void Asset::setVersion(std::uint32_t inVersion)
        {
            if (inVersion <= 0)
            {
                return;
            }

            m_header.version = inVersion;
            setAttribute(VERSION_ATTRIBUTE_NAME, String::sprint("%u", inVersion));
        }

        const String& Asset::getId() const
        {
            return m_header.id;
        }

        void Asset::setId(const String& inId)
        {
            if (inId.isEmpty())
            {
                return;
            }

            m_header.id = inId;
            setAttribute(ID_ATTRIBUTE_NAME, m_header.id);
        }

        AssetType Asset::getType() const
        {
            return m_header.type;
        }

        void Asset::setType(AssetType inType)
        {
            m_header.type = inType;
        }

        void Asset::saveXML()
        {
            save(getFilepath());
        }

        void Asset::syncProperties()
        {
            Serializable::syncProperties();

            if (hasAttribute(VERSION_ATTRIBUTE_NAME))
            {
                const std::uint32_t version = getUint(VERSION_ATTRIBUTE_NAME, m_header.version);
                if (version > 0)
                {
                    m_header.version = version;
                }
            }

            if (hasAttribute(ID_ATTRIBUTE_NAME))
            {
                const String id = getString(ID_ATTRIBUTE_NAME, m_header.id);
                if (!id.isEmpty())
                {
                    m_header.id = id;
                }
            }
        }

        String Asset::getXmlAttribute(const String& inId) const
        {
            return getAttribute(inId);
        }

        void Asset::setXmlAttribute(const String& inId, const String& inData)
        {
            if (inId.isEmpty() || inData.isEmpty())
            {
                return;
            }

            setAttribute(inId.trim(), inData);
        }

        XmlNode Asset::getXML() const
        {
            return getSource();
        }

        String Asset::getPayload() const
        {
            const XmlNode root = getXML();
            if (root.isEmpty())
            {
                return "";
            }

            const XmlNode value = root.getChild(VALUE_TAG);
            if (!value.isEmpty())
            {
                return value.getText();
            }

            for (XmlNode child = root.getFirstChild(); child; child = child.getNextSibling())
            {
                if (!child.isText() && !child.isCData())
                {
                    continue;
                }

                const String text = child.getValue();
                if (text.trim().isEmpty())
                {
                    continue;
                }

                return text;
            }

            return "";
        }

        bool Asset::setPayload(const String& inData)
        {
            XmlNode root = getXML();
            if (root.isEmpty())
            {
                return false;
            }

            for (XmlNode child = root.getFirstChild(); child;)
            {
                XmlNode next = child.getNextSibling();
                if (child.isText() || child.isCData())
                {
                    root.removeChild(child);
                }

                child = next;
            }

            XmlNode value = root.getChild(VALUE_TAG);
            if (value.isEmpty())
            {
                value = root.appendChild(VALUE_TAG);
            }

            value.setText(inData);

            return true;
        }

        void Asset::createXML(const FileSystem::Path& inFilepath)
        {
            if (inFilepath.isEmpty() || !m_document.isEmpty())
            {
                return;
            }

            createRoot(TAG);
            setAttribute(VERSION_ATTRIBUTE_NAME, String::sprint("%u", CURRENT_VERSION));
            setFilepath(inFilepath);
        }

        void Asset::fetchXML(const FileSystem::Path& inFilepath)
        {
            load(inFilepath);

            const XmlNode root = getXML();

            const bool bIsRoot  = root.getParent() == root.getRoot();
            const bool bIsAlone = bIsRoot && root.getNextSibling().isEmpty();
            if (!bIsRoot || !bIsAlone)
            {
                throw std::runtime_error("Asset files root element must not have any siblings");
            }

            if (!root.getName().equals(TAG))
            {
                throw std::runtime_error(
                    String::sprint("Asset files root element must be have [%s] as a tag", TAG).toStandard()
                );
            }
        }
    }
}
