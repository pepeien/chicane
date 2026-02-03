#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Type.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Asset
        {
        public:
            static constexpr inline const char*         TAG = "Asset";

            static constexpr inline const std::uint32_t CURRENT_VERSION = 1;

            static constexpr inline const char*         VERSION_ATTRIBUTE_NAME = "version";
            static constexpr inline const char*         ID_ATTRIBUTE_NAME      = "id";

        public:
            Asset(const FileSystem::Path& inFilepath);
            virtual ~Asset() = default;

        public:
            bool isType(AssetType inType) const;
            bool isEmpty() const;

            const AssetHeader& getHeader() const;
            void setHeader(const AssetHeader& inHeader);

            const FileSystem::Path& getFilepath() const;
            void setFilepath(const FileSystem::Path& inFilepath);

            std::uint32_t getVersion() const;
            void setVersion(std::uint32_t inVersion);

            const String& getId() const;
            void setId(const String& inId);

            AssetType getType() const;
            void setType(AssetType inType);

            pugi::xml_node getXML() const;
            void saveXML();

        private:
            void createXML(const FileSystem::Path& inFilepath);
            void fetchXML(const FileSystem::Path& inFilepath);

        protected:
            AssetHeader        m_header;
            pugi::xml_document m_xml;
        };
    }
}