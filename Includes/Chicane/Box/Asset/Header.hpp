#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Type.hpp"

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AssetHeader
        {
        public:
            AssetHeader(const FileSystem::Path& inFilepath);
            AssetHeader();

        private:
            void fetchVersion(const XmlNode& inRoot);
            void fetchId(const XmlNode& inRoot);
            void fetchType();

        public:
            FileSystem::Path filepath;
            std::uint32_t    version;
            String           id;
            AssetType        type;
        };

        inline AssetHeader peek(const FileSystem::Path& inFilepath)
        {
            AssetHeader header;
            header.filepath = inFilepath;
            header.type     = getTypeFromExtension(inFilepath);

            return header;
        }
    }
}
