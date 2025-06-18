#pragma once

#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AssetHeader
        {
        public:
            static bool isFileAsset(const FileSystem::Path& inFilepath);
            static AssetType getType(const FileSystem::Path& inFilepath);

        public:
            AssetHeader(const FileSystem::Path& inFilepath);
            AssetHeader();

        private:
            void fetchVersion(const pugi::xml_node& inRoot);
            void fetchId(const pugi::xml_node& inRoot);
            void fetchType();

        public:
            FileSystem::Path filepath;
            std::uint32_t    version;
            String           id;
            AssetType        type;
        };
    }
}