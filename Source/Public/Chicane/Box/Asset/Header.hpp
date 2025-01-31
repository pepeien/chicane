#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Base.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            struct Header
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