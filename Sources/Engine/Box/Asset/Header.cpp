#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        AssetHeader::AssetHeader(const FileSystem::Path& inFilepath)
            : AssetHeader()
        {
            if (inFilepath.isEmpty())
            {
                return;
            }

            const XmlDocument document = Xml::load(inFilepath);
            const XmlNode     root     = document.getFirstChild();

            filepath = inFilepath;
            fetchVersion(root);
            fetchId(root);
            fetchType();
        }

        AssetHeader::AssetHeader()
            : filepath(""),
              version(Asset::CURRENT_VERSION),
              id(""),
              type(AssetType::Undefined)
        {}

        void AssetHeader::fetchVersion(const XmlNode& inRoot)
        {
            if (inRoot.isEmpty())
            {
                version = Asset::CURRENT_VERSION;

                return;
            }

            version = inRoot.parseUint(Asset::VERSION_ATTRIBUTE_NAME, 0);
            if (version > 0)
            {
                return;
            }

            version = Asset::CURRENT_VERSION;
        }

        void AssetHeader::fetchId(const XmlNode& inRoot)
        {
            if (inRoot.isEmpty())
            {
                id = "";

                return;
            }

            id = inRoot.getAttribute(Asset::ID_ATTRIBUTE_NAME);
        }

        void AssetHeader::fetchType()
        {
            type = getTypeFromExtension(filepath);
        }
    }
}
