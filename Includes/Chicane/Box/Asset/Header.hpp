#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Essential.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            CHICANE_BOX bool isFileAsset(const std::string& inFilepath);
            CHICANE_BOX Type getType(const std::string& inFilepath);

            struct CHICANE_BOX Header
            {
            public:
                static Header fromFilepath(const std::string& inFilepath);
                static Header fromXML(const std::string& inFilepath, const pugi::xml_document& inDocument);

            private:
                void fetchVersion(const pugi::xml_node& inRoot);
                void fetchId(const pugi::xml_node& inRoot);
                void fetchType();

            public:
                std::filesystem::path filepath = "";
                std::uint32_t         version  = CURRENT_VERSION;
                std::string           id       = "";
                Type                  type     = Type::Undefined;
            };
        }
    }
}