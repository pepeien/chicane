#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        class Asset
        {
        public:
            enum class Type
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
                static Header fromXml(const std::string& inFilepath, const pugi::xml_document& inDocument);

            private:
                void fetchVersion(const pugi::xml_node& inRoot);
                void fetchId(const pugi::xml_node& inRoot);
                void fetchType();

            public:
                std::filesystem::path filepath;
                std::string           version;
                std::string           id;
                Type                  type;
            };

        public:
            static std::string TAG;
            static std::string CURRENT_VERSION;

        public:
            Asset(const std::string& inFilepath);

        public:
            const Header& getHeader() const;

        private:
            void fetchHeader(const std::string& inFilepath);
            void fetchXML(const std::string& inFilepath);

        protected:
            Header             m_header;
            pugi::xml_document m_xml;
        };
    }
}