#pragma once

#include "Core.hpp"
#include "Box/Asset/Base.hpp"
#include "Box/Asset/Type.hpp"
#include "Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            struct CHICANE Header
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