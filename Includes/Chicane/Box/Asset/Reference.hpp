#pragma once

#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AssetReference
        {
        public:
            using List = std::vector<AssetReference>;

        public:
            static constexpr inline const char* ASSET_SOURCE_ATTRIBUTE_NAME = "source";

        public:
            AssetReference();

        public:
            bool isValid() const;

            void saveTo(pugi::xml_node& outNode) const;
            void setFrom(const pugi::xml_node& inNode);

            const FileSystem::Path& getSource() const;
            void setSource(const FileSystem::Path& inValue);

            const String& getReference() const;
            void setReference(const String& inValue);

        private:
            FileSystem::Path m_source;
            String           m_reference;
        };
    }
}