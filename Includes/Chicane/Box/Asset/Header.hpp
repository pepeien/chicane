#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            CHICANE_BOX bool isFileAsset(const FileSystem::Path& inFilepath);
            CHICANE_BOX Type getType(const FileSystem::Path& inFilepath);

            struct CHICANE_BOX Header
            {
            public:
                Header(const FileSystem::Path& inFilepath);
                Header();

            private:
                void fetchVersion(const pugi::xml_node& inRoot);
                void fetchId(const pugi::xml_node& inRoot);
                void fetchType();

            public:
                FileSystem::Path filepath;
                std::uint32_t    version;
                std::string      id;
                Type             type;
            };
        }
    }
}