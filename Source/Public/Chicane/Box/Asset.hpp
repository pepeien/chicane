#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        class Asset
        {
        public:
            enum class Type : std::uint8_t
            {
                Undefined,
                Mesh,    // Two entries [Model, Texture] both being refNames
                Cubemap, // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
                Texture, // Image
                Model,   // 3D Model,
                Audio    // .MP3 Audio
            };

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

        public:
            static std::string   TAG;
            static std::uint32_t CURRENT_VERSION;

        public:
            Asset(const std::string& inFilepath);

        public:
            bool isType(Type inType) const;

            const Header& getHeader() const;
            void setHeader(const Header& inHeader);

            const std::filesystem::path& getFilepath() const;
            void setFilepath(const std::filesystem::path& inFilepath);

            std::uint32_t getVersion() const;
            void setVersion(std::uint32_t inVersion);

            const std::string& getId() const;
            void setId(const std::string& inId);

            Type getType() const;
            void setType(Type inType);

            void saveXML();

        protected:
            bool isXMLEmpty();
            pugi::xml_node getXMLRoot();

        private:
            void createXML(const std::string& inFilepath);
            void fetchXML(const std::string& inFilepath);
            void fetchHeader(const std::string& inFilepath);

        protected:
            Header             m_header;
            pugi::xml_document m_xml;
        };
    }
}